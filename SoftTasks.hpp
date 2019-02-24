#ifndef SoftTasks_HPP
#define SoftTasks_HPP

#include <list>
#include <vector>
#include <functional>
#include <algorithm>


struct TaskData
{
    TaskData(std::function<int()>&& call, int interval, const std::string& name="")
    :call(std::move(call)), interval(interval), last_time(0), name(name)
    {
        Serial.println(String("New&& Task Created with Name ")+name.c_str());
    }
    ~TaskData()
    {
        Serial.println(String("Task Destroyed with Name ")+name.c_str());
    }

    std::function<int()> call;
    int interval;
    int last_time;
    int id=-1;
    const std::string name;
};


class SoftTasks
{
public:
    void loop()
    {
        int now = millis();
        bool triggerClean = false;
        for(auto& tsk : tasks)
            if(now-tsk->last_time >= tsk->interval)
            {
                int newInterval;
                tsk->last_time = now;
                newInterval = tsk->call();
                tsk->interval = newInterval;
                triggerClean |= (newInterval <0);
            }

        if(triggerClean)
        {
            Serial.println("Cleaning finished tasks");
            tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                                       [](const std::shared_ptr<TaskData>& t){return t->interval<0;}),
                        tasks.end());
        }
    }

    int add(std::function<int()> &&call, int interval_ms=0, const std::string& name="")
    {
         addInt(std::move(call),interval_ms,name);
    }
    int add(std::function<bool()> &&call, int interval_ms=0, const std::string& name="")
    {
         addBool(std::move(call),interval_ms,name);
    }
    int add(std::function<void()> &&call, int interval_ms=0, const std::string& name="")
    {
         addVoid(std::move(call),interval_ms,name);
    }

    int addInt(std::function<int()> &&call, int interval_ms=0, const std::string& name="")
    {
        Serial.println(String("Adding int task ")+name.c_str());
        return add(std::make_shared<TaskData>(std::move(call), interval_ms, name));
    }

    int addBool(std::function<bool()> &&call, int interval_ms=0, const std::string& name="")
    {
        Serial.println(String("Adding boolean task ")+name.c_str());
        return add(std::make_shared<TaskData>([call=std::move(call),interval_ms](){return call()?interval_ms:-1;}, interval_ms, name));
    }

    int addVoid(std::function<void()>&& call, int interval_ms=0, const std::string& name="")
    {
        Serial.println(String("Adding void task ")+name.c_str());
        return add(std::make_shared<TaskData>([call=std::move(call),interval_ms](){call(); return interval_ms;}, interval_ms, name));
    }

    int add(std::shared_ptr<TaskData> t)
    {
        t->id=idCnt++;
        tasks.push_back(t);

        Serial.println(String("Added task ")+t->name.c_str()+ " with Id "+ t->id + " and interval " + t->interval);

        return t->id;
    }

    void kill(int taskId)
    {
        int l0 = tasks.size();

        std::list<std::shared_ptr<TaskData> >::iterator it;
        for(it=tasks.begin(); it!=tasks.end(); it++)
            if((*it)->id == taskId)
                break;

        if(it!=tasks.end())
            tasks.erase(it);

        if(tasks.size()>=l0)
        {
            Serial.print("Failed to remove task:");
            Serial.println(taskId);
        }
    }

private:

    std::list<std::shared_ptr<TaskData> > tasks;
    int idCnt;
};


class TaskQueue
{
public:
    TaskQueue(bool cyclic)
    :repeat(cyclic), hold(false)
    {
    }

    int go()
    {
        if(hold)
            return 100;

        if(!qtasks.empty())
        {
            bool keepGoing = qtasks.front().call();
            if(!keepGoing)
            {
                if(repeat)
                    qtasks.push_back(qtasks.front());

                qtasks.pop_front();

                if(!qtasks.empty())
                    return qtasks.front().interval;
                else
                    return 100;
            }
        }
    }

    void add(std::function<int()> task, int interval_ms=0)
    {
        //qtasks.push_back(TaskData(task,interval_ms));
    }    

    void clear()
    {
        qtasks.clear();
    }

    bool repeat;
    bool hold;
private:
    std::list<TaskData> qtasks;


};


#endif