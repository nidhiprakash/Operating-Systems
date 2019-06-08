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

//void ProcessPrint(Process *p) { p->Print(); }

//Process constructor 
Process::Process(int priority, char* name) {

    cout << "\nIn process "<<name<<"'s constructor\n";
    this->priority = priority;
    this->name = name;
    Thread *t = new Thread("main thread");
    this->currentThread = t;
    status = PJUST_CREATED;
    PID = pidC + 1;
    this->processScheduler = new Scheduler();
    childThreads = new List<Thread*>();
    childProcess = new List<Process*>();
}

/*void setStatus(processStates st) 
{ 
    status = st; 
}*/
processStates Process::getStatus(){
    return status;
}
void Process::setPriority(int p){
    priority = p;
}
int Process::getPriority(){
    return priority;
}
void Process::setPID(int i){
    PID = i;
}
int Process::getPID(){
    return PID;
}
void Process::setName(char* c){
    name = c;
}

//Fork method for process
void Process::Fork(VoidFunctionPtr func, void *arg) {

    cout << "In process "<<this->name<<" fork\n";
    Interrupt *interrupt = kernel->interrupt;
    ProcessScheduler *scheduler = kernel->scheduler;

    IntStatus oldLevel;
    currentThread->Fork(this, (VoidFunctionPtr) func, (void *) arg);
    childThreads->Append(currentThread);

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

//handles creation od child threads
void
Process::createChildT (VoidFunctionPtr func, void *arg) {
    Thread *t = new Thread("Child Thread");
    t->Fork(this, func, arg);
    childThreads->Append(t);
}

//handles creation od child processes
void
Process::createChildP() {
    Process *p = new Process(this->priority, "Child process");
    p->parentID = this->PID;
    p->Fork(this->funcPtr, this->args);
    childProcess->Append(p);
}

// finish process modified for process
void Process::Terminate() {
    cout << "Process Termination\n";
    (void) kernel->interrupt->SetLevel(IntOff);     
    ASSERT(this == kernel->currentProcess);
    DEBUG(dbgThread, "Terminating Process: " << name);
    isCompleted = true;
    Sleep(TRUE);
}

// yeild process defined for process
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

//sleep function modified for process
void
Process::Sleep (bool finishing)
{
    cout << "process sleep\n";
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

//Process destructor
Process::~Process() {
    delete childThreads;
    delete childProcess;
    delete processScheduler;
    delete currentThread;
}
