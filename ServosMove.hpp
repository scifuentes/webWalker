#ifndef SERVOSMOVE_HPP
#define SERVOSMOVE_HPP

#include "Servos.hpp"
#include <functional>
#include "SoftTasks.hpp"
#include "commandInterpreter.hpp"

#define TrackServo    // this enables printing servo positions

TaskQueue moveQueue(true);

class ServoMove
{
public:
    ServoMove(Servo& servo, int finalPosition, int speed)
    :servo(servo)
    ,fpos(finalPosition)
    ,delta(abs(speed))
    {
        Serial.println(String("New ServoMove"));
    }

    ServoMove(const ServoMove& other)
    : servo(other.servo)
    , fpos(other.fpos)
    , delta(other.delta)
    {
        Serial.println(String("ServoMove Copy"));
    }

    ~ServoMove()
    {
        Serial.println(String("ServoMove Destroyed"));
    }


    bool move()
    {
        int pos = servo.read();
        if(abs(pos-fpos)<=delta)
        {
            servo.write(fpos);
            
            #ifdef TrackServo
            Serial.println(String("Servo=")+fpos+"*");
            #endif
            
            //done
            return false;
        }
        else
        {
            if(fpos>pos)
                pos+=delta;
            else
                pos-=delta;
            servo.write(pos);

            #ifdef TrackServo
            Serial.println(String("Servo=")+pos);
            #endif
            
            //keep going
            return true;
        }
    }

private:
    Servo& servo;
    const int fpos;
    const int delta;
};


class ServosMove
{
public:
    bool idle;

    ServosMove(std::vector<Servo>& servos, SoftTasks& st)
    :servos(servos)
    //,st(st)
    ,idle(true)
    {}


    ServosMove(std::vector<Servo>& servos, SoftTasks& st, std::vector<int> finalPositions, int maxSpeed)
    :servos(servos)
    {
        set(finalPositions,maxSpeed);
    }

    void set(std::vector<int> finalPositions, int maxSpeed)
    {
        fpos = finalPositions;
        maxDelta = abs(maxSpeed);

        int maxTravel = -1;
        maxTravelIndex = -1;
        for(int i=0; i<servos.size(); i++)
        {
            int travel = remTravel(i);
            if(travel>maxTravel)
            {
                maxTravel = travel; 
                maxTravelIndex = i;
            }
        }
        //Serial.println(String("MaxTravel: ")+maxTravel);
        //Serial.println(String("MaxTravelindex: ")+maxTravelIndex);
        idle=false;
        //taskId = st.add([this](){move();},20);   
    }

    bool move()
    {
        int maxTravel = remTravel(maxTravelIndex);
        if(maxTravel<=maxDelta)
        {
            for(int i=0;i<servos.size();i++)
            {
                servos[i].write(fpos[i]);
                Serial.println(String("Servo[")+i+"]="+fpos[i]+"*");
            }
            
            //done
            idle = true;
            return false;
        }
        else
        {
            for(int i=0; i<servos.size(); i++)
            {
                int delta = (maxDelta*remTravel(i))/maxTravel;
                int pos = servos[i].read();
                if(fpos[i]>pos)
                    pos+=delta;
                else
                    pos-=delta;
                servos[i].write(pos);
                Serial.println(String("Servo[")+i+"]="+pos);
            }
        
            //keep going
            return true;
        }

    }

private:
    std::vector<Servo>& servos;
    //SoftTasks& st;
    std::vector<int> fpos;
    int maxDelta;
    int maxTravelIndex;
    int taskId;

    int remTravel(int i)
    {
        return abs(fpos[i]-servos[i].read());
    }

};

//================

std::shared_ptr<ServoMove> instance;
void cmd_moveServo(const String& command, const std::vector<int>& spaces, SoftTasks& sTasks)
{
    Serial.println(String(__func__)+": "+command);

    int index = command.substring(spaces[0],spaces[1]).toInt();
    int pos = command.substring(spaces[1],spaces[2]).toInt();
    int speed = command.substring(spaces[2],spaces[3]).toInt();

    Serial.println(String(__func__)+": "+index+", "+pos+", "+speed);

    /*
    std::shared_ptr<ServoMove> instance;
    instance = std::make_shared<ServoMove>(servos[index], pos, speed);
    std::function<int()> m = std::bind(&ServoMove::move, instance);
    sTasks.add(m,20,"MoveServo");
    */
    sTasks.addBool(std::bind(&ServoMove::move, 
                             ServoMove(servos[index], pos, speed))
               ,20,"MoveServo");

    Serial.println(String(__func__)+" Done");

}

void cmd_moveServos(const String& command, const std::vector<int>& spaces, SoftTasks& sTasks)
{
    Serial.println(String(__func__)+": "+command);
    String sValues;

    int maxSpeed = command.substring(spaces[0],spaces[1]).toInt();

    std::vector<int> finalPos(spaces.size()-1);
    for(int i=0; i<finalPos.size(); i++)
    {
        String sv = command.substring(spaces[i+1],spaces[i+2]);
        finalPos[i]=sv.toInt();
        sValues+=" "+sv;
    }
    Serial.println(String(__func__)+" speed: "+maxSpeed+" values: "+sValues);    

    moveQueue.add(std::bind(&ServosMove::move, 
                            ServosMove(servos,sTasks,finalPos,maxSpeed)));

}

std::vector<Servo>& setupServosMove(
    CommandHandlers& cmdHandlers,
    SoftTasks& sTasks)
{
  Serial.println("add moveQueue");
  sTasks.add([](){moveQueue.go();},100, "Move Queue");

  using namespace std::placeholders;

  cmdHandlers.add("mv.one", std::bind(&cmd_moveServo,_1,_2, std::ref(sTasks)));
  cmdHandlers.add("mv.all", std::bind(&cmd_moveServos,_1,_2, std::ref(sTasks)));
  cmdHandlers.add("mv.clear", [](){moveQueue.clear();});
  cmdHandlers.add("mv.pause", [](){moveQueue.hold=true;});
  cmdHandlers.add("mv.go", [](){moveQueue.hold=false;});

  return servos;

}

#endif