/*
 *  Simple HTTP get webclient test
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <pins_arduino.h>
#include <Servo.h> 
#include "SoftTasks.hpp"
#include "WebHandlers.hpp"
#include "ServoAux.hpp"
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
unsigned int localOvershotCounter;

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

struct WalkMove
{
  WalkMove()  {}
  WalkMove(int s0,int s1,int steps)
    :s0(s0),s1(s1),steps(steps)    {}

  int s0;
  int s1;
  int steps;
};
std::vector<WalkMove> walkCycle;

void init_WalkCycle()
{
  walkCycle.push_back(WalkMove(1000,2000,50));
  walkCycle.push_back(WalkMove(1100,1900,30));
  walkCycle.push_back(WalkMove(1200,1800,40));
  walkCycle.push_back(WalkMove(1300,1700,70));
  walkCycle.push_back(WalkMove(1400,1600,60));
  walkCycle.push_back(WalkMove(1500,1500,30));
  walkCycle.push_back(WalkMove(1400,1600,10));
  walkCycle.push_back(WalkMove(1300,1700,50));
  walkCycle.push_back(WalkMove(1200,1800,60));
  walkCycle.push_back(WalkMove(1100,1900,70));
}



void commandInterpreter(const String& command)
{
  Serial.println(String(__func__)+": "+command);
  
  std::vector<int> spaces;
  int p=-1;
  do
  {
    p=command.indexOf(" ",p+1);
    if(p>0)
      spaces.push_back(p);
  } while(p>0);

  if(command.startsWith("wc.flush"))
  {
    walkCycle.clear();
  }
  else if(command.startsWith("ws.add"))
  {
    Serial.println("ws.add");

    WalkMove move;
    move.s0=command.substring(spaces[0],spaces[1]).toInt();
    move.s1=command.substring(spaces[1],spaces[2]).toInt();
    move.steps=command.substring(spaces[2]).toInt();
    walkCycle.push_back(move);
  }
  else
  {
    Serial.println("Unknown command");
  }

}



void moveServos()
{
  static unsigned long zeroTime = millis();
  unsigned int delta = millis()-zeroTime;
  if(delta>=1000/50)
  {
    localOvershotCounter += delta - 1000/50;
    zeroTime= millis();

    static int posCount = 0, subCount=0;

    for(int i=0; i<4; i++)
    {
      int nextCount = posCount + 1;
      if(nextCount>=walkCycle.size())
        nextCount = 0;

      int subs = walkCycle[posCount].steps;
      int p0 = walkCycle[posCount].s0 + ((walkCycle[nextCount].s0 - walkCycle[posCount].s0)*subCount) / subs;
      int p1 = walkCycle[posCount].s1 + ((walkCycle[nextCount].s1 - walkCycle[posCount].s1)*subCount) / subs;
      servos[i*2].write(p0);
      servos[i*2+1].write(p1);
  
      //Serial.println(String("") +p0+", "+posCount+"/"+nextCount+", "+subCount);

      subCount++;
      if(subCount>=subs)
      {
        subCount = 0;
        posCount++;
      }
      if(posCount >= walkCycle.size())
        posCount = 0;
    }
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

  sTasks.add([](){server.handleClient();});
  sTasks.add([](){trackCyclesPerSecond(false);});//just count
  sTasks.add([](){trackCyclesPerSecond(true);},1000);//collect

  sTasks.add([](){quad.loop();},100);
}

void loop() {
  sTasks.loop();
}

//-----------------------------------------------

