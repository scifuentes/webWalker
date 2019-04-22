#ifndef WEBHANDLERS_HPP
#define WEBHANDLERS_HPP

#include "webElements.hpp"

void printServerRequest(ESP8266WebServer& server)
{
  Serial.println(String("-URI: ") + server.uri());
  Serial.println(String("-Method: ") + (server.method() == HTTP_GET ? "GET" : "POST"));
  Serial.println(String("-Arguments: ")+ server.args());
  for (uint8_t i = 0; i < server.args(); i++) 
  {
    Serial.println(String("-- ") + server.argName(i) + ": " + server.arg(i));
  }
}


class WebHandlers
{
public:
    WebHandlers(ESP8266WebServer& server_)
      : server(server_)
    {
        mainWeb = "<h1>WebServos</h1>";
        mainWeb += WebElements::movePad();
        mainWeb += WebElements::servoSlider(0,"setServo");
        mainWeb += WebElements::servoSlider(1,"setServo");
        mainWeb += WebElements::servoSlider(2,"setServo");
        mainWeb += WebElements::servoSlider(3,"setServo");
        mainWeb += WebElements::servoSlider(4,"setServo");
        mainWeb += WebElements::servoSlider(5,"setServo");
        mainWeb += WebElements::servoSlider(6,"setServo");
        mainWeb += WebElements::servoSlider(7,"setServo");
        mainWeb += WebElements::textArea("commands","Commands");
        mainWeb += WebElements::manualQuery("listCommands","List Commands");
        mainWeb += WebElements::browserCounter(); 
        mainWeb += WebElements::refreshQuery("serverTime",1000);
        mainWeb += WebElements::refreshQuery("cycles",1000);
    }

    void notFound()
    {
        server.send(404);
        Serial.println("Unknown request");
        printServerRequest(server);
    }

    void root()
    {
        server.send(200, "text/html", mainWeb); //OK and sends back a the webpage
        Serial.println("Root Handle");
    }

    void setServo(std::vector<Servo>& servos)
    {
      server.send(204);
      Serial.println(__func__);
      //printServerRequest(server);

      for (uint8_t i = 0; i < server.args(); i++) 
      {
        if(server.argName(i).startsWith("servo_"))
        {
          int s = server.argName(i).substring(6).toInt();
          int v = server.arg(i).toInt();
          servos[s].write(v);
          Serial.println(String("Servo[")+s+"] => "+v);
        }
      } 
    }

private:
    ESP8266WebServer& server;
    String mainWeb;


};

#endif