#include "../lib/debug.h"
#include "../machine/interrupt.h"
#include "process.h"
#include "../lib/copyright.h"
#include "thread.h"
#include "switch.h"
#include "synch.h"
#include "../lib/sysdep.h"
#include "processScheduler.h"

int Process::pidC=0;

void ProcessPrint(Process *p) { p->Print(); }

Process::Process(int priority, char* name) {
    std::cout << "\nIn process "<<name<<" constructor\n";
    this->priority = priority;
    this->name = name;
    status = PJUST_CREATED;
    PID = pidC + 1;
    childThreads = new List<Thread*>();
    childProcess = new List<Process*>();
    Thread *t = new Thread("main thread");
    // cThreads->Append(t);
    this->currentThread = t;
    //this->currentThread->setStatus(RUNNING); 
    this->processScheduler = new Scheduler();
}

void Process::Fork(VoidFunctionPtr func, void *arg) {

    std::cout << "In process "<<this->name<<" fork\n";
    Interrupt *interrupt = kernel->interrupt;
    ProcessScheduler *scheduler = kernel->scheduler;
    IntStatus oldLevel;
    //Thread *t = new Thread("forked thread");
    currentThread->Fork(this, (VoidFunctionPtr) func, (void *) arg);
    childThreads->Append(currentThread);
    // this->currentThread = t;
    oldLevel = interrupt->SetLevel(IntOff);
    this->currentThread = this->processScheduler->FindNextToRun();
    //oldLevel = interrupt->SetLevel(IntOff);
    scheduler->ReadyToRun(this);
    scheduler->Print();
    //kernel->currentProcess->Yield();
    //this->pScheduler->ReadyToRun(this->currentThread);
    (void) interrupt->SetLevel(oldLevel);

    funcPtr = func;
    args = arg;
}

void Process::Terminate() {
    std::cout << "In process terminate\n";
    (void) kernel->interrupt->SetLevel(IntOff);     
    ASSERT(this == kernel->currentProcess);
    DEBUG(dbgThread, "Terminating Process: " << name);
    isCompleted = true;
    Sleep(TRUE);
}


void
Process::Sleep (bool finishing)
{
    std::cout << "In process sleep\n";
    Process *nextProcess;

    ASSERT(this == kernel->currentProcess);
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    
    status = PBLOCKED;
    while ((nextProcess = kernel->scheduler->FindNextProcessToRun()) == NULL)
        kernel->interrupt->Idle();	// no one to run, wait for an interrupt

    // returns when it's time for us to run
    kernel->scheduler->Run(nextProcess, finishing);
    //this->currentThread->Sleep(finishing);
}

void
Process::Yield ()
{
    Process *nextProcess;
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    
    ASSERT(this == kernel->currentProcess);
    
    DEBUG(dbgThread, "Yielding process: " << name);
    kernel->scheduler->Print();
    nextProcess = kernel->scheduler->FindNextProcessToRun();
    if (nextProcess != NULL) {

	    kernel->scheduler->ReadyToRun(this);
	    kernel->scheduler->Run(nextProcess, FALSE);
            //this->currentThread->Yield();
    }
    (void) kernel->interrupt->SetLevel(oldLevel);
}

void
Process::createChildT (VoidFunctionPtr func, void *arg) {
    Thread *t = new Thread("Child Thread");
    t->Fork(this, func, arg);
    childThreads->Append(t);
}

void
Process::createChildP() {
    Process *p = new Process(this->priority, "Child process");
    p->parentID = this->PID;
    p->Fork(this->funcPtr, this->args);
    childProcess->Append(p);
}

void
Process::Join() {
    kernel->proclist->Append(this);
    this->Sleep(FALSE);
}

Process::~Process() {
    delete childThreads;
    delete childProcess;
    delete processScheduler;
    delete currentThread;
}
