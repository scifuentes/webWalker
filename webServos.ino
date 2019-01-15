/*
 *  Simple HTTP get webclient test
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <pins_arduino.h>
#include <Servo.h> 
#include "globals.h"
#include "SoftTasks.hpp"
#include "WebHandlers.hpp"
#include "ServoAux.hpp"
#include "walk.hpp"
#include "commandInterpreter.hpp"
#include <vector>
#include <regex>

#define ssid      "VFNL-FDD180"
#define password  "95K9XSZ8CYHL2"
 
ESP8266WebServer server(83);

std::vector<Servo> servos(8);

SoftTasks sTasks;

QuadMove quad(servos,sTasks);

unsigned int cycleCounter = 0;
unsigned int overshootCounter;

void setupSerial()
{
  Serial.begin(115200);
  delay(100);
}

void setupServos()
{
  servos[0].attach(D1);
  servos[1].attach(D2);
  servos[2].attach(D3);
  servos[3].attach(D4);
  
  servos[4].attach(D5);
  servos[5].attach(D6);
  servos[6].attach(D7);
  servos[7].attach(D8);
}

void setupWebServer()
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
  Serial.println(WiFi.localIP());

  static WebHandlers handlers(server);

  server.on("/", [](){handlers.root();});
  server.on("/setServo", [](){handlers.setServo(servos);});
  server.on("/serverTime", [](){server.send(200, "text/html", String("Server Time: ")+(millis()/1000.));});
  server.on("/cycles", [](){server.send(200, "text/html", String("Cycles per seccond= ")+ cycleCounter +"; overshoot= "+ overshootCounter);});
  server.onNotFound([](){handlers.notFound();});

  server.on("/zero", [](){quad.setZero();});
  server.on("/one", [](){quad.setBase();});
  server.on("/fwd", [](){quad.stepFwd();});
  server.on("/stop", [](){quad.stop();});
  server.on("/commands", [](){handlers.parseCommands(&commandInterpreter);});

  server.begin();
  Serial.println("Server started");
}


void trackCyclesPerSecond(bool collect)
{
  static unsigned int localCycleCounter;
  if(collect)
  {
    overshootCounter = localOvershotCounter;
    cycleCounter = localCycleCounter;
    localCycleCounter = 0;
    localOvershotCounter = 0;
  }
  else
  {
    localCycleCounter++;
  }
}






//-----------------------------------------------


void setup() 
{
  setupSerial();
  setupWebServer();
  quad.setZero();
  setupServos();
  quad.setZero();

  init_WalkCycle();

  interpreter.add("wc.flush", [](){walkCycle.clear();});
  interpreter.add("ws.add", &cmd_add_walk_step);
  
  sTasks.add([](){server.handleClient();});
  sTasks.add([](){trackCyclesPerSecond(false);});//just count
  sTasks.add([](){trackCyclesPerSecond(true);},1000);//collect

  sTasks.add([](){quad.loop();},100);
}

void loop() {
  sTasks.loop();
}

//-----------------------------------------------

