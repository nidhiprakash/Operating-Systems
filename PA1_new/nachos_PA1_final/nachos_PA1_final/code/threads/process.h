#ifndef NACHOS_PROCESS_H
#define NACHOS_PROCESS_H

#pragma once
#include "thread.h"
#include "../lib/list.h"
#include "scheduler.h"
#include "kernel.h"
#include "../lib/copyright.h"
#include "../lib/utility.h"
#include "../lib/sysdep.h"
#include "../machine/machine.h"
#include "addrspace.h"


enum processStates { PJUST_CREATED, PRUNNING, PREADY, PBLOCKED };

class Thread;
class Scheduler;
class Kernel;

class Process {
    int priority;
    //List<Thread*> *childThreads;
    List<Process*> *childProcess;
    processStates status;
    char* name;
    int PID;
    static int pidC;

    VoidFunctionPtr funcPtr;
    void *args;

public:
    Process(int priority, char* name);
    Process();
    virtual ~Process();
    int getPriority() const { return priority; };
    List<Thread*> *childThreads;
    List<Thread*> getchildThreads() { return *childThreads; }
    List<Process*> getchildProcess() { return *childProcess; }
    void Fork(VoidFunctionPtr func, void *arg);
    void Terminate();
    void setStatus(processStates st) { status = st; }
    char* getName() { return (name); }
    int getPid() const { return PID; };
    void Sleep(bool finishing);
    void Print() { cout << name; }
    void Yield();
    void createChildT(VoidFunctionPtr func, void *arg);
    void createChildP();
   

    Scheduler *processScheduler;
    Thread *currentThread;
    int parentID;
    bool isCompleted;
    

    bool operator == (Process p) {
        if(this->getPid() == p.getPid())
            return true;
        else return false;
    }

    void Join();
};

extern void ProcessPrint(Process *p);	

#endif //NACHOS_PROCESS_H
