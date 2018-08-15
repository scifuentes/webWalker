#ifndef SERVOAUX_HPP
#define SERVOAUX_HPP

#include <Servo.h> 
#include "SoftTasks.hpp"
#include <functional>

class ServoMove
{
public:
    bool done;

    ServoMove(Servo& servo, int finalPosition, int speed, SoftTasks& st)
    :servo(servo)
    ,st(st)
    ,fpos(finalPosition)
    ,delta(abs(speed))
    ,done(false)
    {
        taskId = st.add([this](){move();},20);
    }

    void move()
    {
        int pos = servo.read();
        if(abs(pos-fpos)<=delta)
        {
            servo.write(fpos);
            Serial.println(String("Servo=")+fpos+"*");
            st.remove(taskId);
            done = true;
        }
        else
        {
            if(fpos>pos)
                pos+=delta;
            else
                pos-=delta;
            servo.write(pos);
            //Serial.println(String("Servo=")+pos);
        }
    }

private:
    Servo& servo;
    SoftTasks& st;
    const int fpos;
    const int delta;
    const std::function<void()> endCall;
    int taskId;

};

class ServosMove
{
public:
    bool done;

    ServosMove(std::vector<Servo>& servos, SoftTasks& st)
    :servos(servos)
    ,st(st)
    ,done(true)
    {}


    ServosMove(std::vector<Servo>& servos, SoftTasks& st, std::vector<int> finalPositions, int maxSpeed)
    :servos(servos)
    ,st(st)
    ,done(false)
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

        done = false;
        taskId = st.add([this](){move();},20);   
    }
    void move()
    {
        int maxTravel = remTravel(maxTravelIndex);
        if(maxTravel<=maxDelta)
        {
            for(int i=0;i<servos.size();i++)
            {
                servos[i].write(fpos[i]);
                Serial.println(String("Servo[")+i+"]="+fpos[i]+"*");
            }
            st.remove(taskId);
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
                Serial.println(String("Servo[")+i+"]="+pos);
            }
        }
    }

private:
    std::vector<Servo>& servos;
    SoftTasks& st;
    std::vector<int> fpos;
    int maxDelta;
    int maxTravelIndex;
    int taskId;

    int remTravel(int i)
    {
        return abs(fpos[i]-servos[i].read());
    }

};



class QuadMove
{
public:
    QuadMove(std::vector<Servo>& servos, SoftTasks& st)
    :servos(servos)
    ,zero({90,90,90,90,90,90,90,90})
    ,dir({1,-1,-1,1,-1,1,1,-1})
    ,base({0,30,0,10,0,-10,0,-30})
    ,sTasks(sTasks)
    ,move(servos,st)
    ,seq(0)
    {}

    void loop()
    {
        if(go==1)
            moveFwd();
    }

    void stop()
    { 
        go = 0;
    }

    void stepFwd()
    {
        go = 1;
    }

    void setZero()
    {
        for(int i=0; i<servos.size(); i++)
            servos[i].write(zero[i]);
    }

    void setBase()
    {
        for(int i=0; i<servos.size(); i++)
            servos[i].write(zero[i]+dir[i]*base[i]);
    }

    void moveFwd()
    {
        if(!move.done)
            return;

        std::vector<int> p;
        const int x3=40,x2=20,x1=10,x0=0,x_1=-x1,x_2=-x2,x_3=-x3;
        const int z0=0, z1=30;
        if(seq==0)
            p={z0,x3,z0,x1,z0,x_1,z0,x_3};
        else if(seq==1)
            p={z0,x2,z0,x0,z0,x_2,z1,x0};
        else if(seq==2)
            p={z0,x1,z0,x_1,z0,x_3,z0,x3};
        else if(seq==3)
            p={z0,x0,z0,x_2,z1,x0,z0,x2};
        else if(seq==4)
            p={z0,x_1,z0,x_3,z0,x3,z0,x1};
        else if(seq==5)
            p={z0,x_2,z1,x0,z0,x2,z0,x0};
        else if(seq==6)
            p={z0,x_3,z0,x3,z0,x1,z0,x_1};
        else if(seq==7)
            p={z1,x0,z0,x2,z0,x0,z0,x_2};

        for(int i=0;i<p.size();i++)
            p[i]=zero[i]+dir[i]*p[i];

        move.set(p,3);
        seq++;
        if(seq>7)
            seq=0;

    }

private:
    std::vector<int> zero,dir,base;
    std::vector<Servo>& servos;
    SoftTasks& sTasks;
    ServosMove move;
    int go;
    int seq;

};

#endif