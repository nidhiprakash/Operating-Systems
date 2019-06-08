#include "kernel.h"
#include "main.h"
#include "thread.h"
#include "bitmap.h"
//#include "request.h"
#include "request.cc"
#include "list.h"

//Request rq[20];
Bitmap *bm = new Bitmap(20); 
int k = 0;
List<Thread *> *ListB = new List<Thread *>();
List<Thread *> *ListC = new List<Thread *>();
List<Thread *> *ListD = new List<Thread *>();
List<Thread *> *ListE = new List<Thread *>();
List<Request*> *requestsFailed = new List<Request*>();
List<Request*> *requestsPassed = new List<Request*>();

char City(int num){
        if(num == 0) { return'A';}
        else if(num == 1) { return 'B';}
        else if(num == 2) { return 'C';}
        else if(num == 3) { return 'D';}
        else if(num == 4) { return'E';}
        }


void
Reservation(int num)
{
    Request *reqst = new Request(num);

    int seatsLeft=0;
    int seats[5],tester;
            for(int m=0; m<20; m++)
            {
                    if(bm->Test(m)!=TRUE) //Test function of bitmap
                        seatsLeft++;
            }
            cout<<"\n";
            cout<<"Seats available: "<<seatsLeft<<endl;
            cout<<endl;
    
    cout<<endl;
    cout<<"<===================== Request number "<<reqst->getuniqueID()<<" ===========================================================>"<<endl;
    cout<<"\t Unique ID \t\t #Passengers  \t\t Departure From \t\t Destination \t\t"<<endl;
    cout<<"\t\t"<<reqst->getuniqueID()<<"\t\t\t"<<reqst->getnoOfPassengers()<<"\t\t\t"<<City(reqst->getdeptCity())<<"\t\t\t\t"<<City(reqst->getdestCity())<<endl;
    cout<<"<===========================================================================================================================>"<<endl;
    cout<<endl;

    //==================================================

    if(reqst->getnoOfPassengers() <= bm->NumClear())
    {
        cout<<"Seats are available for booking."<<endl;
        for(int i=0;i<reqst->getnoOfPassengers();i++)
        {
            tester = bm->FindAndSet();
            reqst->seatsList->Append(tester);
        }

        ListIterator<int> it = reqst->getseatNumber();
        cout<<"Seat number allocated to the passengers: ";
        while(!it.IsDone())
        {
            cout<<it.Item()<<" ";
            it.Next();
        }
        cout<<"\n";

        cout<<"Available for allocation are the seats: ";
        cout<<bm->NumClear()<<"\n";
        requestsPassed->Append(reqst);
        if(reqst->getdestCity()==1)
                ListB->Append(kernel->currentThread);
        if(reqst->getdestCity()==2)
                ListC->Append(kernel->currentThread);
        if(reqst->getdestCity()==3)
                ListD->Append(kernel->currentThread);
        if(reqst->getdestCity()==4)
                ListE->Append(kernel->currentThread);
        kernel->currentThread->Sleep(0);
        cout<<"Deallocation of seats at Destination city"<<endl;
        ListIterator<int> Siterator = reqst->getseatNumber();
        while(!Siterator.IsDone()){
            bm->Clear(Siterator.Item());
            cout<<"Deallocating seat number: ";
            cout<<Siterator.Item()<<" "<<endl;
            Siterator.Next();}
    }
    else
    {
        cout<<"******Seats not available. Request Denied******";
        requestsFailed->Append(reqst);
        kernel->currentThread->Finish();
    } 
}

void 
FlightThread(int flightval)
{
    for(int i = 0 ; i < flightval ; i++)
    {
        if(i != 0)
        {
                if(i == 1)
                {
                    ListIterator<Thread *> *ifB = new ListIterator<Thread *>(ListB);
                    while(!ifB->IsDone())
                    {
                        Thread *t = ifB->Item();
                        kernel->scheduler->ReadyToRun(t);
                        ifB->Next();
                    }
                }
                        
                else if( i == 2)        
                {
                    ListIterator<Thread *> *ifC = new ListIterator<Thread *>(ListC);
                    while(!ifC->IsDone()) 
                    {
                        Thread *t = ifC->Item();
                        kernel->scheduler->ReadyToRun(t);
                        ifC->Next();
                    }
                }

                else if( i == 3)
                {
                    ListIterator<Thread *> *ifD = new ListIterator<Thread *>(ListD);
                    while(!ifD->IsDone())
                    {
                        Thread *t = ifD->Item();
                        kernel->scheduler->ReadyToRun(t);
                        ifD->Next();
                    }
                }
                        
                else if( i == 4)        
                {
                    ListIterator<Thread *> *ifE = new ListIterator<Thread *>(ListE);
                    while(!ifE->IsDone())
                    {
                        Thread *t = ifE->Item();
                        kernel->scheduler->ReadyToRun(t);
                        ifE->Next();
                    }
         
                }

                else
                cout<<" Error !"<<endl; 
	    }
    for(int k=0;k<5;k++)
    {
        if(i!=4){
            Thread *reservationsT = new Thread("forked thread");
            reservationsT->Fork((VoidFunctionPtr) Reservation, (void *) i);
            kernel -> currentThread -> Yield();
        }
    }

    cout<<endl;
    cout<<"<=============================================================>"<<endl;
    cout<<" \t Summary at City "<<City(i)<<"\t\t\t\t\t\t"<<endl;
    cout<<"<=============================================================>"<<endl;

    ListIterator<Request *> *itr1 = new ListIterator<Request *>(requestsPassed);
        while(!itr1->IsDone())
        {
            if(i == (itr1->Item()->getdestCity()))
            {
                cout<<"Passenger with unique ID "<<itr1->Item()->getuniqueID()<<" is getting off."<<endl;
                cout<<"Seat again available for reservation are: ";
                ListIterator<int> sno = itr1->Item()->getseatNumber();
                while(!sno.IsDone())
                {
                    cout<<sno.Item()<<"\t";
                    sno.Next();
                }
                cout<<"\n";
            }
            itr1->Next();
        }

        ListIterator<Request *> *itr2 = new ListIterator<Request *>(requestsPassed);
        while (!itr2->IsDone()) 
        {
            if (i < (itr2->Item()->getdestCity()))
                cout << "Passenger with unique ID " << itr2->Item()->getuniqueID() << " is still onboard \n"<<endl;
            itr2->Next();
        }

        //Yield current flight thread
        kernel->currentThread->Yield(); 
    }

    //Printing discarded entities 
    ListIterator<Request *> *failedLi = new ListIterator<Request *>(requestsFailed);
    cout << "\n\nThe denied requests denied are: ";
    while (!failedLi->IsDone()) {
        cout << failedLi->Item()->getuniqueID() << "\t";
        failedLi->Next();
    }
    
}

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
       // printf("*** thread %d looped %d times\n", which, num);
        kernel->currentThread->Yield();
    }
}

void
ThreadTest()
{
    Thread *flightt = new Thread("forked thread");
    flightt->Fork((VoidFunctionPtr) FlightThread, (void *) 5);
    kernel -> currentThread -> Yield();
   // SimpleThread(0);
}

