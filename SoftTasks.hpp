#ifndef SoftTasks_HPP
#define SoftTasks_HPP

#include <list>
#include <vector>
#include <functional>
#include <algorithm>

#define TraceTasks
#ifdef TraceTasks
#define TRACE(foo) Serial.println(foo)
#else
#define TRACE(foo) ;
#endif

struct TaskData
{
    TaskData(std::function<int()>&& call, int interval, const std::string& name="")
    :call(std::move(call)), interval(interval), last_time(0), name(name)
    {
        TRACE(String("New&& Task Created with Name ")+name.c_str());
    }
    
    #ifdef TraceTasks
    ~TaskData()
    {
        TRACE(String("Task Destroyed with Name ")+name.c_str());
    }
    #endif

    std::function<int()> call;
    int interval;   //in ms
    int last_time;
    int id=-1;
    const std::string name;
};

class TasksContainer
{
public:
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
        TRACE(String("Adding int task ")+name.c_str());

        return add(std::make_shared<TaskData>(std::move(call), interval_ms, name));
    }

    int addBool(std::function<bool()> &&call, int interval_ms=0, const std::string& name="")
    {
        TRACE(String("Adding boolean task ")+name.c_str());

        return add(std::make_shared<TaskData>([call=std::move(call),interval_ms](){return call()?interval_ms:-1;}, interval_ms, name));
    }

    int addVoid(std::function<void()>&& call, int interval_ms=0, const std::string& name="")
    {
        TRACE(String("Adding void task ")+name.c_str());

        return add(std::make_shared<TaskData>([call=std::move(call),interval_ms](){call(); return interval_ms;}, interval_ms, name));
    }

    int add(std::shared_ptr<TaskData> t)
    {
        t->id=idCnt++;
        tasks.push_back(t);

        TRACE(String("Added task ")+t->name.c_str()+ " with Id "+ t->id + " and interval " + t->interval);

        return t->id;
    }

protected:

    std::list<std::shared_ptr<TaskData> > tasks;
    int idCnt;

};

class SoftTasks : public TasksContainer
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
            TRACE("Cleaning finished tasks");

            tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                                       [](const std::shared_ptr<TaskData>& t){return t->interval<0;}),
                        tasks.end());
        }
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

};


class TaskQueue : public TasksContainer
{
public:
    TaskQueue(bool loop)
    :loop(loop), hold(false)
    {
    }

    int run()
    {
        if(hold)
            return 100;

        if(!tasks.empty())
        {

            int currentInterval = tasks.front()->call();
            bool keepCurrent = (currentInterval >= 0);

            TRACE(String("QTask ")+tasks.front()->id+":"+currentInterval);

            if(keepCurrent)
                return currentInterval;
            else
            {
                if(loop)
                    tasks.push_back(tasks.front());

                tasks.pop_front();

                if(!tasks.empty())
                    return tasks.front()->interval;
            }
        }
        
        return 100;
    }

    void clear()
    {
        tasks.clear();
        hold=true;
    }

    bool loop;
    bool hold;
};

class SleepTask
{
public:
    SleepTask(int rounds, int round_interval=100)
    :rounds(rounds)
    ,round_interval(round_interval)
    ,counter(0)
    {}

    int run()
    {
        counter++;
        if(counter<rounds)
            return round_interval;
        else
        {
            counter = 0;
            return -1;
        }

    }
protected:
    int rounds;
    int round_interval;
    int counter;
};

#endif