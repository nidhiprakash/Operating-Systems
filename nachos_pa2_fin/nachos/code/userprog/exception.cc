// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------


void Exit_POS(int val){
	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	kernel->scheduler->ReadyToRun(kernel->mapTracker[val]);
	kernel->currentThread->Finish();
	(void) kernel->interrupt->SetLevel(oldLevel);
}

void ForkTest1(int id)
{
	printf("ForkTest1 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest1 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	
	Exit_POS(id);
}

void ForkTest2(int id)
{
	printf("ForkTest2 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest2 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	
	Exit_POS(id);
}

void ForkTest3(int id)
{
	printf("ForkTest3 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest3 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	
	Exit_POS(id);
}

//Because an exception will arise when they are accessed, 
//the loading should be done as an exception handling in userprog/exception.cc.
void pageFaulthandler(int va){
	int virtualp = (unsigned) va / PageSize;
	int physical = kernel->bm->FindAndSet();
	char *buf = new char[PageSize];


	TranslationEntry *te = kernel->currentThread->space->getTVPNentry(virtualp);
	if(physical == -1){
		int first = kernel->ppn->RemoveFront(); //first ..
		Thread *t = kernel->ptmap[first];
		TranslationEntry *te2 = t->space->getTPPNentry(first); //first..
		int s = te2->swapLoc;
		kernel->sF->WriteAt(&kernel->machine->mainMemory[first * PageSize], 
					PageSize, s * PageSize);
		te2->physicalPage = -1;
		te2->valid = FALSE;
		te2->use = FALSE;
		kernel->bm->Clear(first);
		physical = kernel->bm->FindAndSet();
	}

	kernel->sF->ReadAt(buf, PageSize, te->swapLoc * PageSize);
	te->valid = TRUE;
	te->use = TRUE;
	te->physicalPage = physical;
	bcopy(buf, &(kernel->machine->mainMemory[te->physicalPage * PageSize]), PageSize);
	kernel->ptmap.insert(std::pair<int, Thread *> (kernel->currentThread->space->pageTable[virtualp].physicalPage, 
											kernel->currentThread));
        kernel->ppn->Append(te->physicalPage);
}


void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      	switch(type) {
      	case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
			ASSERTNOTREACHED();
		break;

      	case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));
			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);
	
			/* Modify return point */
			{
	  		/* set previous programm counter (debugging only)*/
	  			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  		/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
	  			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			}
			return;
			ASSERTNOTREACHED();
		break;

		/*********************************/
	//Fork_POS()enables the user program to fork a kernel child thread. It takes integer 1, 2 or 3 as 
	//parameter, creates a corresponding kernel thread, which executes one of the test functions basing 
	//on the parameter value, and returns an integer as the process ID of the child. For example, when 
	//child = Fork_POS(2) is called in a user program, a kernel thread will be created in ExceptionHandler() 
	//and this thread will call the ForkTest2 function (using the Fork()call of thread.cc). 
	//The child process ID will be assigned to child.


	//FORK POS - thread gets created,Forks and placed into the readytorun list; control goes back to 
	//main process
	// Require: id as parameter - getid getter method and variable
	//2)switch case or if statement to run test function according to function pointer.

		case SC_FORK_POS:
			{
				
				int num = (int)kernel->machine->ReadRegister(4);
				//result = 4;
				//cout<<"Entering fork_os and val of v is:"<<v<<"\n";
				Thread *t = new Thread("forked thread");
				int id = t->getThreadId();
				switch(num){
				case 1:
				t->Fork((VoidFunctionPtr) ForkTest1,(void *)t->getThreadId());
				break;
				case 2:
				t->Fork((VoidFunctionPtr) ForkTest2,(void *)t->getThreadId());
				break;
				case 3:
				t->Fork((VoidFunctionPtr) ForkTest3,(void *)t->getThreadId());
				break;
				default:
				break;
				}
				kernel->machine->WriteRegister(2, (int)t->getThreadId());
				//int i = childTh->getThreadID;
				kernel->mapTracker[t->getThreadId()] = kernel->currentThread;
				
				/* Modify return point */
				{
				/* set previous programm counter (debugging only)*/
					kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
					kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
				/* set next programm counter for brach execution */
					kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
				}
				return;
				ASSERTNOTREACHED();
			}
		break;

	//•	Wait_POS() provides the routine for the user program to wait for the execution of its child thread. 
	//It takes the child process ID as parameter. The parent will be put into sleep until the child awakens it. 
	//The child wakes up the parent when it finishes by calling Exit_POS, which you’ll implement. Wait_POS() and 
	//Exit_POS() should be able to handle certain errors such as invalid child process ID or no parent process waiting.

	//wait fork: the parent process is put to sleep and searches the findNextTorun list 
	//runs the next element,
	//Also implementing the validations to make sure if parent of the chld exists in the map and
	//if the child belongs to the right parent

		case SC_WAIT_POS:
			{
				int c = (int)kernel->machine->ReadRegister(4);
				if ( kernel->mapTracker.find(c) == kernel->mapTracker.end() ) {

  					cout<<"The child ID given does not exist or is not valid or child does not have any parent"<<"\n";
					Exit(0);
				} 
				else 
					if(kernel->mapTracker[c]==kernel->currentThread)
					{
						IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
						cout << "Sleep thread" << endl;
						kernel->currentThread->Sleep(FALSE);

						(void) kernel->interrupt->SetLevel(oldLevel);


						{
	  					/* set previous programm counter (debugging only)*/
	  					kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  					/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  					kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
						/* set next programm counter for brach execution */
	  					kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
						}
					}
	
				
			return;
			}
		break;

		//Exit_POS() is called by a child and wakes up its parent. 
		//This function is not a system call but you need to implement in exception.cc

		//exit pos: child finishes and wakes up the parent

		case SC_Exit:
		{
				cerr<<"The program has ended and is about to exit"<<"\n";
				int exitVal = kernel->machine->ReadRegister(4);
				cout<<"exiting value:"<<exitVal<<"\n";
				Exit(exitVal);
		}
		break;


		//•	Write()writes string to a speciﬁed destination. We’re faking it using printf, 
		//therefore it always writes to the screen. This system call takes three arguments: 
		//a char* buffer, an int size, and an OpenFileId. Since we’re faking it, we can ignore the 
		//OpenFileId parameter. You need to obtain the ﬁrst two arguments from the user program and 
		//print an character at a time using printf.
		case SC_Write:
		{
			int o;
			cout<<"In write"<<endl;
			IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
			int loc = (int)kernel->machine->ReadRegister(4);
			int bytesToread  = (int)kernel->machine->ReadRegister(5);
			o = loc;
			cout<<"\n";

			for(int i=0; i<bytesToread; i++)
			{
				if(kernel->machine->ReadMem(loc, 1, &o))
					printf("%c", o);
					loc++;
			}
						
			cout << endl;
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
				
				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			}
			(void) kernel->interrupt->SetLevel(oldLevel);
			return;	
			ASSERTNOTREACHED();
		}
		break;
		

      	default:
			cerr << "Unexpected system call " << type << "\n";
		break;
      	}
      	break;
		case PageFaultException:
      {
        IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
		pageFaulthandler((int)kernel->machine->ReadRegister(39));
		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

		/* set next programm counter for brach execution */
		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
		(void)kernel->interrupt->SetLevel(oldLevel);
		return;
	  }
		break;      
      
    	default:
      		cerr << "Unexpected user mode exception" << (int)which << "\n";
      	break;
    }
    ASSERTNOTREACHED();
}