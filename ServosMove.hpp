#ifndef SERVOSMOVE_HPP
#define SERVOSMOVE_HPP

#include "Servos.hpp"
#include <functional>
#include "SoftTasks.hpp"
#include "commandInterpreter.hpp"

//#define TraceServos
#ifdef TraceServos
#define TRACE(foo) Serial.println(foo)
#else
#define TRACE(foo) ;
#endif

TaskQueue moveQueue(true);

void move_stop()
{
    moveQueue.hold=true;
}

class ServoMove
{
public:
    ServoMove(Servo& servo, int finalPosition, int speed)
    :servo(servo)
    ,fpos(finalPosition)
    ,delta(abs(speed))
    {
        TRACE(String("New ServoMove"));
    }

    #ifdef TarceServo
    ServoMove(const ServoMove& other)
    : servo(other.servo)
    , fpos(other.fpos)
    , delta(other.delta)
    {
        TRACE(String("ServoMove const Copy"));
    }
    #endif

    #ifdef TarceServo
    ~ServoMove()
    {
        TRACE(String("ServoMove Destroyed"));
    }
    #endif

    bool move()
    {
        int pos = servo.read();
        if(abs(pos-fpos)<=delta)
        {
            servo.write(fpos);
            
            TRACE(String("Servo=")+fpos+"*");
            
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

            TRACE(String("Servo=")+pos);
            
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
    bool done;

    ServosMove(std::vector<Servo>& servos)
    :servos(servos)
    ,done(true)
    {}


    ServosMove(std::vector<Servo>& servos, std::vector<int> finalPositions, int maxSpeed)
    :servos(servos)
    ,fpos(finalPositions)
    ,maxDelta(abs(maxSpeed))
    ,done(true)
    {
    }

    void set()
    {
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
        TRACE(String("MaxTravel: ")+maxTravel);
        TRACE(String("MaxTravelindex: ")+maxTravelIndex);
        done=false;
    }

    bool move()
    {
        if(done)
            set();

        int maxTravel = remTravel(maxTravelIndex);
        if(maxTravel<=maxDelta)
        {
            for(int i=0;i<servos.size();i++)
            {
                servos[i].write(fpos[i]);
                TRACE(String("Servo[")+i+"]="+fpos[i]+"!");
            }
            done = true;
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
                TRACE(String("Servo[")+i+"]="+pos);
            }
        }


        return !done;
    }

private:
    std::vector<Servo>& servos;
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

//std::shared_ptr<ServoMove> instance;
void cmd_moveServo(const String& command, const std::vector<int>& spaces, SoftTasks& sTasks)
{
   TRACE(String(__func__)+": "+command);

    int index = command.substring(spaces[0],spaces[1]).toInt();
    int pos = command.substring(spaces[1],spaces[2]).toInt();
    int speed = command.substring(spaces[2],spaces[3]).toInt();

    TRACE(String(__func__)+": "+index+", "+pos+", "+speed);

    /*
    std::shared_ptr<ServoMove> instance;
    instance = std::make_shared<ServoMove>(servos[index], pos, speed);
    std::function<int()> m = std::bind(&ServoMove::move, instance);
    sTasks.add(m,20,"MoveServo");
    */
    /*
    ServoMove servoMove(servos[index], pos, speed);
    sTasks.addBool([servoMove]()mutable{return servoMove.move();}
                   ,20,"MoveServo");
    */
    /*
    std::function<bool()> f = std::bind(&ServoMove::move, ServoMove(servos[index], pos, speed));
    TRACE("..Adding");
    sTasks.addBool(std::move(f),20,"MoveServo");
    */
    
    sTasks.addBool(std::bind(&ServoMove::move, ServoMove(servos[index], pos, speed))
                   ,20,"MoveServo");
    
    TRACE(String(__func__)+" Done");

}

void cmd_moveServos(const String& command, const std::vector<int>& spaces, SoftTasks& sTasks)
{
    TRACE(String(__func__)+": "+command);
    String sValues;

    int maxSpeed = command.substring(spaces[0],spaces[1]).toInt();

    std::vector<int> finalPos(spaces.size()-1);
    for(int i=0; i<finalPos.size(); i++)
    {
        String sv = command.substring(spaces[i+1],spaces[i+2]);
        finalPos[i]=sv.toInt();
        sValues+=" "+sv;
    }
    TRACE(String(__func__)+" speed: "+maxSpeed+" values: "+sValues);    

    sTasks.addBool(std::bind(&ServosMove::move, 
                            ServosMove(servos,finalPos,maxSpeed)),50);

}

void cmd_moveAdd(const String& command, const std::vector<int>& spaces)
{
    TRACE(String(__func__)+": "+command);
    String sValues;

    int maxSpeed = command.substring(spaces[0],spaces[1]).toInt();

    std::vector<int> finalPos(spaces.size()-1);
    for(int i=0; i<finalPos.size(); i++)
    {
        String sv = command.substring(spaces[i+1],spaces[i+2]);
        finalPos[i]=sv.toInt();
        sValues+=" "+sv;
    }
    TRACE(String(__func__)+" speed: "+maxSpeed+" values: "+sValues);    

    moveQueue.addBool(std::bind(&ServosMove::move, 
                                ServosMove(servos,finalPos,maxSpeed)),50);

}

void cmd_moveWait(const String& command, const std::vector<int>& spaces)
{
    TRACE(String(__func__)+": "+command);
    String sValues;

    int sleep = command.substring(spaces[0],spaces[1]).toInt();

    TRACE(String(__func__)+" - sleep: "+sleep);    

    moveQueue.addInt(std::bind(&SleepTask::run, 
                                SleepTask(sleep)));

}

std::vector<Servo>& setupServosMove(
    CommandHandlers& cmdHandlers,
    SoftTasks& sTasks)
{
  TRACE("add moveQueue");
  sTasks.addInt([](){return moveQueue.run();},100, "Move Queue");

  using namespace std::placeholders;

  cmdHandlers.add("servo.move", std::bind(&cmd_moveServo, _1, _2, std::ref(sTasks)));
  cmdHandlers.add("servos.move", std::bind(&cmd_moveServos, _1, _2, std::ref(sTasks)));
  cmdHandlers.add("mv.add", &cmd_moveAdd);
  cmdHandlers.add("mv.clear", [](){moveQueue.clear();});
  cmdHandlers.add("mv.go", [](){moveQueue.hold=false;});
  cmdHandlers.add("mv.pause", [](){moveQueue.hold=true;});
  cmdHandlers.add("mv.loop", [](){moveQueue.loop=true;});
  cmdHandlers.add("mv.once", [](){moveQueue.loop=false;});
  cmdHandlers.add("mv.wait", &cmd_moveWait);

  return servos;

}

#endif