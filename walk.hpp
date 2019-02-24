#ifndef walk_HPP
#define walk_HPP

#include "globals.h"
#include "commandInterpreter.hpp"
#include "ServosMove.hpp"
#include "SoftTasks.hpp"

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


void cmd_AddWalkStep(const String& command, const std::vector<int>& spaces)
{
    Serial.println(String(__func__)+": "+command);

    WalkMove move;
    move.s0=command.substring(spaces[0],spaces[1]).toInt();
    move.s1=command.substring(spaces[1],spaces[2]).toInt();
    move.steps=command.substring(spaces[2]).toInt();
    walkCycle.push_back(move);
}


void init_Walk(CommandHandlers& cmdHandlers)
{
  init_WalkCycle();

  cmdHandlers.add("w.flush", [](){walkCycle.clear();});
  cmdHandlers.add("w.add", &cmd_AddWalkStep);

}


class QuadMove
{
public:
    QuadMove(std::vector<Servo>& servos, SoftTasks& st)
    :servos(servos)
    ,zero({90,90,90,90,90,90,90,90})
    ,dir({1,1,-1,-1,-1,-1,1,1})
    ,base({0,30,0,-10,0,10,0,-30})
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
        if(!move.idle)
            return;

        //gait:4231
        std::vector<int> p;
        const int x3=40,x2=20,x1=10,x0=0,x_1=-x1,x_2=-x2,x_3=-x3;
        const int z0=0, z1=30;
        if(seq==0)
            p={z0,x3,z0,x_1,z0,x1,z0,x_3};
        else if(seq==1)
            p={z0,x2,z0,x_2,z0,x0,z1,x0};
        else if(seq==2)
            p={z0,x1,z0,x_3,z0,x_1,z0,x3};
        else if(seq==3)
            p={z0,x0,z1,x0,z0,x_2,z0,x2};
        else if(seq==4)
            p={z0,x_1,z0,x3,z0,x_3,z0,x1};
        else if(seq==5)
            p={z0,x_2,z0,x2,z1,x0,z0,x0};
        else if(seq==6)
            p={z0,x_3,z0,x1,z0,x3,z0,x_1};
        else if(seq==7)
            p={z1,x0,z0,x0,z0,x2,z0,x_2};

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


/*
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
*/

#endif