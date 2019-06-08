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
    processStates status; //for states - RUNNING,READY,JUST CREATED,BLOCKED
    char* name;
    int PID;
    static int pidC;
    List<Process*> *childProcess;
    VoidFunctionPtr funcPtr;
    void *args;

public:
    Process(int priority, char* name);
    Process();
    virtual ~Process();

    Scheduler *processScheduler;
    Thread *currentThread;

   //setters declared
    void setStatus(processStates st) { status = st; }
	void setPriority(int p);
    void setPID(int i);
	void setName(char* c);
    //void ProcessPrint(Process *p) { p->Print(); }

    //getters declared
    processStates getStatus();
	int getPriority();
    int getPID();
    char* getName() { return (name); }
    //int getPriority() const { return priority; };
    List<Thread*> *childThreads;
    List<Thread*> getchildThreads() { return *childThreads; }
    List<Process*> getchildProcess() { return *childProcess; }

    //functions used from thread.cc
    void Fork(VoidFunctionPtr func, void *arg);
    void Terminate();
    void Sleep(bool finishing);
    void Print() { cout << name; }
    void Yield();
    
    //int getPid() const { return PID; };
    
    void createChildT(VoidFunctionPtr func, void *arg);
    void createChildP();
    int parentID;
    bool isCompleted;

    bool operator == (Process p) {
        if(this->getPID() == p.getPID())
            return true;
        else return false;
    }

    //void Join();
};

extern void ProcessPrint(Process *p);	

#endif //NACHOS_PROCESS_H
