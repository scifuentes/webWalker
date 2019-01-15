#include "globals.h"

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

void cmd_add_walk_step(const String& command, const std::vector<int>& spaces)
{
    Serial.println("ws.add");

    WalkMove move;
    move.s0=command.substring(spaces[0],spaces[1]).toInt();
    move.s1=command.substring(spaces[1],spaces[2]).toInt();
    move.steps=command.substring(spaces[2]).toInt();
    walkCycle.push_back(move);
}

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