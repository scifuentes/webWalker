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

#define ssid      "TP-LINK_90B8E8"
#define password  "17133531"
 
ESP8266WebServer server(83);

std::vector<Servo> servos(8);

SoftTasks sTasks;

QuadMove quad(servos,sTasks);

unsigned int cycleCounter = 0;
unsigned int overshootCounter;
unsigned int localOvershotCounter;

void setup() 
{
  setupSerial();
  setupWebServer();
  quad.setZero();
  setupServos();
  quad.setZero();

  sTasks.add([](){server.handleClient();});
  sTasks.add([](){trackCyclesPerSecond(false);});//just count
  sTasks.add([](){trackCyclesPerSecond(true);},1000);//collect

  sTasks.add([](){quad.loop();},100);
}

void loop() {
  sTasks.loop();
}

//-----------------------------------------------


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





void moveServos()
{
  static unsigned long zeroTime = millis();
  unsigned int delta = millis()-zeroTime;
  if(delta>=1000/50)
  {
    localOvershotCounter += delta - 1000/50;
    zeroTime= millis();

    const int posSize = 10;
    static int posCount = 0, subCount=0;
    static int pos[posSize][3] =
    {
      {50,1000,2000},
      {30,1100,1900},
      {40,1200,1800},
      {70,1300,1700},
      {60,1400,1600},
      {30,1500,1500},
      {10,1400,1600},
      {50,1300,1700},
      {60,1200,1800},
      {70,1100,1900}
    };

    for(int i=0; i<4; i++)
    {
      int nextCount = posCount + 1;
      if(nextCount>=posSize)
        nextCount = 0;

      int subs = pos[posCount][0];
      int p0 = pos[posCount][1] + ((pos[nextCount][1] - pos[posCount][1])*subCount) / subs;
      int p1 = pos[posCount][2] + ((pos[nextCount][2] - pos[posCount][2])*subCount) / subs;
      servos[i*2].write(p0);
      servos[i*2+1].write(p1);
  
      //Serial.println(String("") +p0+", "+posCount+"/"+nextCount+", "+subCount);

      subCount++;
      if(subCount>=subs)
      {
        subCount = 0;
        posCount++;
      }
      if(posCount>=posSize)
        posCount = 0;
    }
  }
}

