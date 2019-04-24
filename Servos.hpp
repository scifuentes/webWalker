#ifndef SERVOS_HPP
#define SERVOS_HPP

#include <Servo.h> 
#include "commandInterpreter.hpp"

std::vector<Servo> servos(8);

void cmd_setServo(const String& command, const std::vector<int>& spaces)
{
    Serial.println(String(__func__)+": "+command);

    int index = command.substring(spaces[0],spaces[1]).toInt();
    int value = command.substring(spaces[1],spaces[2]).toInt();

    Serial.println(String(__func__)+": "+index+","+value);

    servos[index].write(value);

}

void cmd_setServos(const String& command, const std::vector<int>& spaces)
{
    Serial.println(String(__func__)+": "+command);
    String sValues;

    int value;
    for(int i=0; i<spaces.size()-1; i++)
    {
        String sv = command.substring(spaces[i],spaces[i+1]);
        servos[i].write(sv.toInt());
        sValues+=" "+sv;
    }
    Serial.println(String(__func__)+" values: "+sValues);

}

void servos_zero()
{
  for(auto& servo : servos)
    servo.write(90);
}

std::vector<Servo>& setupServos(
    CommandHandlers& cmdHandlers)
{
  servos[0].attach(D1);
  servos[1].attach(D2);
  servos[2].attach(D3);
  servos[3].attach(D4);
  
  servos[4].attach(D5);
  servos[5].attach(D6);
  servos[6].attach(D7);
  servos[7].attach(D8);

  cmdHandlers.add("servo.set", &cmd_setServo);
  cmdHandlers.add("servos.set", &cmd_setServos);

  return servos;

}

#endif