#ifndef PROCESSSCHEDULER_H
#define PROCESSSCHEDULER_H

#pragma once
#include "../lib/copyright.h"
#include "../lib/list.h"
#include "process.h"

class Process;
class ProcessScheduler {
  public:
    ProcessScheduler();               
    ~ProcessScheduler();              
    void ReadyToRun(Process* process);
    Process* FindNextProcessToRun();
    void Run(Process* nextProcess, bool finishing);
    void CheckToBeDestroyed();
    void Print();               
    void CheckIfParentUnblocked();
  private:
    
    SortedList<Process *> *pReadyList;  
    Process *pToBeDestroyed;
};

#endif 
