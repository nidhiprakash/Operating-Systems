#include "kernel.h"
#include "main.h"
#include "process.h"

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
    }
    //kernel->currentProcess->currentThread->Yield();
    kernel->currentProcess->Terminate();

   // }
}

void
SimplePrint(int loop){
    for(int i=0;i<2; i++)
	cout << "Hello this is process : " << loop << endl;
}

void
ThreadTest()
{
    cout << "\n\nIn threadtest main\n";
    Process *p1 = new Process(4, "Process p1");
    p1->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    
    Process *p2 = new Process(3, "Process p2");
    p2->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
     
    Process *p3 = new Process(1, "Process p3");
    p3->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    
    Process *p4 = new Process(5, "Process p4");
    p4->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    
    Process *p5 = new Process(2, "Process p5");
    p5->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
     

    p1->createChildP();
    p1->createChildT();
    

    p2->createChildP();
    p2->createChildT();

    p3->createChildP();
    p3->createChildT();

    p4->createChildP();
    p4->createChildT();

    p5->createChildP();
    p5->createChildT();

    kernel->currentProcess->Terminate();
}
