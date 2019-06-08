#include "../lib/debug.h"
#include "processScheduler.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------

int comp(Process *p1, Process *p2){
    
    if(p1->getPriority() < p2->getPriority())
        return 1;
    else if(p1->getPriority() > p2->getPriority())
        return -1;
    else return 0;
}

ProcessScheduler::ProcessScheduler()
{    
    pReadyList = new SortedList<Process *>(comp);
    pToBeDestroyed = NULL;
} 

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

ProcessScheduler::~ProcessScheduler()
{ 
    delete pReadyList;
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
ProcessScheduler::ReadyToRun (Process *process)
{
    cout<<"\nIn "<<process->getName()<<" ready to run \n";
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting process on ready list: " << process->getName());
    cout << "Pushing process on readylist: " << process->getName() << endl;
    process->setStatus(PREADY);
    pReadyList->Insert(process);
    //processReadyList->Append(process);
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Process *
ProcessScheduler::FindNextProcessToRun ()
{
    //cout << "In process find next to run\n";
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (pReadyList->IsEmpty()) {
	return NULL;
    } 
    else {
    	return pReadyList->RemoveFront();
    }
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void
ProcessScheduler::Run (Process *nextProcess, bool finishing)
{
    Process *oldProcess = kernel->currentProcess;
    cout << "In process "<<oldProcess->getName()<<" run with priority"<<oldProcess->getPriority()<<"\n";
    Thread * oldthead = kernel->currentProcess->currentThread;
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {	// mark that we need to delete current thread
         //ASSERT(processToBeDestroyed == NULL);
	 //processToBeDestroyed = oldProcess;
    }
    
    kernel->currentProcess = nextProcess;  // switch to the next thread
    nextProcess->setStatus(PRUNNING);      // nextThread is now running
    
    Thread *nextThread = nextProcess->currentThread;
    DEBUG(dbgThread, "Switching from: " << oldProcess->getName() << " to: " << nextProcess->getName());
    kernel->currentProcess->currentThread = nextThread;  
    nextProcess->currentThread->setStatus(RUNNING);    
    SWITCH(oldthead, nextThread);

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldProcess->getName());

    //CheckIfParentUnblocked();
    CheckToBeDestroyed();	// check if thread we were running
}

/*void
ProcessScheduler::CheckIfParentUnblocked() {
    bool allDone = true;
    int parentID = kernel->currentProcess->parentID;
    Process* torun;
    ListIterator<Process*> *l = new ListIterator<Process*>(kernel->proclist);
    List<Process*> *list1;
    while(!l->IsDone()){
        if(l->Item()->getPID()==parentID){
            torun = l->Item();
            *list1 = torun->getchildProcess();
        }
        l->Next();
    }

    ListIterator<Process*> *li= new ListIterator<Process*>(list1);
    while (!li->IsDone()) {
        if(!li->Item()->isCompleted)
            allDone = false;
        li->Next();
    }

    if(allDone)
        ReadyToRun(torun);
}*/

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
ProcessScheduler::CheckToBeDestroyed()
{
    if (pToBeDestroyed != NULL) {
        delete pToBeDestroyed;
	pToBeDestroyed = NULL;
    }
}
 
//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
ProcessScheduler::Print()
{
    cout << "\nProcess readylist has \n";
    pReadyList->Apply(ProcessPrint);
}

