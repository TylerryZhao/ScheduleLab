#include "policy.h"//对于timer的处理，参考了github上的内容：https://github.com/DannieGuo/ICS-Labs/blob/main/schedlab/main.cc
#include <iostream>
#include <map>
#include <algorithm>
using namespace std;

map<int, Event::Task> cpu_todo;
map<int, Event::Task> io_todo;
// bool cmp(Event::Task &a, Event::Task &b) //原来使用vector存储cpu_todo 和 io_todo，所以需要排序函数，但是换成map就不需要
// {
//     if (a.arrivalTime == b.arrivalTime)
//     {
//         if (a.deadline == b.deadline)
//         {
//             return a.priority < b.priority;
//         }
//         return a.deadline < b.deadline;
//     }
//     return a.arrivalTime < b.arrivalTime;
// }
int now_time = -1; // 参考了github
Action policy(const std::vector<Event> &events, int current_cpu,
              int current_io)
{
    Action result;
    result.cpuTask = current_cpu;  //初始化为当前执行的任务
    result.ioTask = current_io;
    Event::Task temp;
    int events_num = events.size();
    int i = 0;
    // int j = 0;
    for (i = 0; i < events_num; i++)
    {
        int event_type = (int)events[i].type;
        // switch (event_type)  //使用switch case语句在平台上跑会出错，必须使用if语句才不会出错
        // {
        // case 0:
        if (event_type == 0)
        {
            now_time = events[i].time;
        }

        // case 1: // 用户请求新的任务 kTaskArrival
        if (event_type == 1)
        {
            cpu_todo.insert(map<int, Event::Task>::value_type(events[i].task.deadline, events[i].task));
        }

        /* code */

        // case 2: // 任务完成 kTaskFinish
        if (event_type == 2)
        {
            map<int, Event::Task>::iterator iter;
            iter = cpu_todo.begin();
            while (iter != cpu_todo.end())
            {
                if (iter->second.taskId == events[i].task.taskId)
                {
                    cpu_todo.erase(iter);
                    break;
                }
                iter++;
            }
        }

        // case 3: // 需要占用IO资源 kIoRequest
        if (event_type == 3)
        {
            io_todo.insert(map<int, Event::Task>::value_type(events[i].task.deadline, events[i].task));
            map<int, Event::Task>::iterator iter;
            iter = cpu_todo.begin();
            while (iter != cpu_todo.end())
            {
                if (iter->second.taskId == events[i].task.taskId)
                {
                    cpu_todo.erase(iter);
                    break;
                }
                iter++;
            }
        }

        // case 4: // 完成了IO任务并且需要占用CPU资源 kIoEnd
        if (event_type == 4)
        {
            cpu_todo.insert(map<int, Event::Task>::value_type(events[i].task.deadline, events[i].task));
            map<int, Event::Task>::iterator iter;
            iter = io_todo.begin();
            while (iter != io_todo.end())
            {
                if (iter->second.taskId == events[i].task.taskId)
                {
                    io_todo.erase(iter);
                    break;
                }
                iter++;
            }
        }

        // default:
        //     break;
    }
    map<int, Event::Task>::iterator result_cpu;
    map<int, Event::Task>::iterator result_io;
    result_cpu = cpu_todo.begin();
    result_io = io_todo.begin();
    if (current_io == 0)
    {
        if (!io_todo.empty())
        {
            result_io = io_todo.begin();
            while (result_io != io_todo.end())
            {
                if (result_io->first > now_time)

                    break;

                result_io++;
            }
            if (result_io == io_todo.end())
                result_io = io_todo.begin();
            result.ioTask = result_io->second.taskId;
        }
    }
    result_cpu = cpu_todo.begin();
    while (result_cpu != cpu_todo.end()) // 参考了github上的部分内容
    {
        if (result_cpu->first > now_time)
        {
            break;
        }
        result_cpu++;
    }
    if (result_cpu == cpu_todo.end())
    {
        result_cpu = cpu_todo.begin();
    }
    result.cpuTask = result_cpu->second.taskId;
    // if (current_io != result_cpu->second.taskId)  //最朴素的check一下当前任务之后直接把deadline最小的一个作为下一个执行
    // {
    //     result.cpuTask = result_cpu->second.taskId;
    //     if (current_io != 0)
    //     {
    //         result.ioTask = current_io;
    //         return result;
    //     }
    //     else
    //     {
    //         if (result.cpuTask != result_io->second.taskId)
    //         {
    //             result.ioTask = result_io->second.taskId;
    //             return result;
    //         }
    //     }
    // }
    

    

    return result;
    // return Action();
}
