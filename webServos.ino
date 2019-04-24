/*
 *  Simple HTTP get webclient test
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <pins_arduino.h>
#include "globals.h"
#include "SoftTasks.hpp"
#include "WebHandlers.hpp"
#include "Servos.hpp"
#include "ServosMove.hpp"
//#include "walk.hpp"
#include "commandInterpreter.hpp"
#include <vector>
#include <regex>

#define ssid      "VFNL-FDD180"
#define password  "95K9XSZ8CYHL2"
#define serverPort 83



void setupSerial()
{
  Serial.begin(115200);
  delay(100);
}


void setupWebServer(
  CommandHandlers& cmdHandlers,
  std::vector<Servo>& servos,
  ESP8266WebServer& server)//,
  //QuadMove& quad)
{

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  //Set a static IP (optional)
  IPAddress ip(192, 168, 1, 83);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);


  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()+":"+serverPort);

  static WebHandlers handlers(server);

  server.on("/", [](){handlers.root();});
  server.onNotFound([](){handlers.notFound();});

  server.on("/serverTime", [&](){server.send(200, "text/html", String("Server Time: ")+(millis()/1000.));});
  server.on("/cycles", [&](){server.send(200, "text/html", String("Cycles per seccond= ")+ globals.cycleCounter +"; overshoot= "+ globals.overshootCounter);});

  server.on("/setServo", [&](){handlers.setServo(servos);});


  server.on("/zero", [&](){move_stop(); servos_zero();});
/*
  server.on("/one", [&](){quad.setBase();});
  server.on("/fwd", [&](){quad.stepFwd();});
  server.on("/stop", [&](){quad.stop();});
*/

  server.on("/commands", [&](){handleCommandsRequest(server, cmdHandlers);});
  server.on("/listCommands", [&](){server.send(200, "text/html", listCommands(cmdHandlers, ", "));});

  server.begin();
  Serial.println("Server started");
}


void trackCycles(bool collect)
{
  static unsigned int localCycleCounter;
  if(collect)
  {
    globals.overshootCounter = globals.localOvershotCounter;
    globals.cycleCounter = localCycleCounter;
    localCycleCounter = 0;
    globals.localOvershotCounter = 0;
  }
  else
  {
    localCycleCounter++;
  }
}



//-----------------------------------------------

CommandHandlers cmdHandlers;
ESP8266WebServer server(serverPort);
SoftTasks sTasks;
//QuadMove quad(servos,sTasks);


void setup() 
{

  setupSerial();
  std::vector<Servo>& servos = setupServos(cmdHandlers);
  setupServosMove(cmdHandlers, sTasks);

  setupWebServer(cmdHandlers,servos,server);//,quad);
  //quad.setZero();
  //init_Walk(cmdHandlers);
  //quad.setZero();

  sTasks.add([](){server.handleClient();},10, "webHandle");
  sTasks.add([](){trackCycles(false);}, 0, "cycleCounter");//just count, every loop
  sTasks.add([](){trackCycles(true);}, 1000, "cycleStorer");//collect, once a second

  //sTasks.add([](){quad.loop();},100);
}

void loop() {
  //Serial.println("loop");

  sTasks.loop();
}

//-----------------------------------------------

