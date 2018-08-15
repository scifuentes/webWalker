#ifndef SOFTTaskDataS_HPP
#define SOFTTaskDataS_HPP

#include <vector>
#include <functional>

//typedef void(*VoidCall)();
typedef std::function<void()> VoidCall;

class SoftTasks
{
public:
    void loop()
    {
        int now = millis();
        for(TaskData& cd : calls)
        {
            if(now-cd.last_time >= cd.interval)
            {
                cd.last_time = now;
                (cd.call)();
            }
        }
    }

    int add(VoidCall call, int interval_ms=0)
    {
        TaskData t = TaskData(call,interval_ms,idCnt++);
        calls.push_back(t);
        return t.id;
    }

    void remove(int taskId)
    {
        int l0 = calls.size();

        std::vector<TaskData>::iterator it;
        for(it=calls.begin(); it!=calls.end(); it++)
            if(it->id == taskId)
                break;
        if(it!=calls.end())
            calls.erase(it);

        if(calls.size()>=l0)
        {
            Serial.print("Failed to remove task:");
            Serial.println(taskId);
        }
    }

private:
    struct TaskData
    {
        TaskData(VoidCall call, int interval, int id)
        :call(call),interval(interval),last_time(0),id(id)
        {}

        VoidCall call;
        int interval;   //ms
        int last_time;
        int id;
    };

    std::vector<TaskData> calls;
    int idCnt;
};





#endif