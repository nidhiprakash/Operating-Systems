#include "kernel.h"
#include "main.h"
#include "thread.h"
#include "person.h"
#include "list.h"

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        //printf("*** thread %d looped %d times\n", which, num);
        kernel->currentThread->Yield();
    }
}

/**
PrintWaiting : user defined function to which List::Apply() will 
 be applied to every element in the list
*/
void PrintWaiting(Person *p){
			cout<<".........................................."<<endl;
			cout << "List Priority: " << p->getPriority() << endl;
            cout << "List Sequence number: " << p->getSequencenumb() << endl;
		}

/**
compr function - to compare 2 person objects based on their priority.
The compare function is used to insert the persons into the sortedList.
*/
static int compr(Person *m,Person *n)
{ 
        if(m->getPriority() < n->getPriority())
        { 
        	return -1;
        }
        else if(m->getPriority() > n->getPriority()) 
        {

        	return 1;
        }
        else 
        	return 0;
}


void sim1()
{
	Person a[20];
	
	//creating sortedlist
    SortedList<Person*> *slp = new SortedList<Person*>(compr);

    int i,time;
    
    cout<<"\n============> Simulation 1 <================"<<endl;
	for(i=0; i<20; i++)
	{
	    int p =(rand() % 9);
      	int q =(rand() % 59 + 1);
      	int r =(rand() % 12 + 1);
      	
        a[i].setSequencenumb(i); //assigned as they arrive
		a[i].setPriority(p); //randomly generated and assigned (between 0 and 9 - 0 being highest)
		a[i].setLaundryTime(q);	//randomly generated
	    a[i].setNumberOfClothes(r); //randomly generated 
	    

	    cout<<"Sequence Number \t\t Priority \t\t Clothes \t\t"<<"Laundry Time"<<"\n";
	    cout<<a[i].getSequencenumb()<<"\t\t\t\t\t"<<a[i].getPriority()<<"\t\t\t"<<a[i].getNumberOfClothes()<<"\t\t\t"<<a[i].getLaundryTime()<<"\n";
	    cout<<"---------------------------------------------------------------------------------------------------"<<endl;

	}
	
    for(i=0;i<20;i++)
    {
     slp->Insert(&a[i]);
    }

    slp->Apply(PrintWaiting);

    Person *w, *z;
    time=0;
    for(i=0;i<20;i++)
    {
        w = slp->Front();
        cout<<"\n Enterance:"<<"\n";
        cout<<"Order "<<"\t\t\t Person with sequence \t\t\t priority \t\t\t no. of clothes \t\t\t Laundry-time"<<"\n";
        cout<<i<<"\t\t\t\t"<<w->getSequencenumb()<<"\t\t\t\t\t"<<w->getPriority()<<"\t\t\t\t"<<w->getNumberOfClothes()<<"\t\t\t\t"<<w->getLaundryTime()<"\n";
        cout<<"\n -----------------------------------------------------------------------------------------------------------------------------------------------------------"<<"\n";
        
        slp->RemoveFront();
        if(i>0){
        	cout<<"\n *********Person with Sequence " << z->getSequencenumb()<<" leaves the room.********************* \n";
            
        }
        time=time+w->getLaundryTime();
        z = w;
    }
    cout<<time<<"\n";
};


void sim2()
{
    Person a[20];
    
    //creating list
    List<Person*> *lp = new List<Person*>;
    int i,time;
    cout<<"\n============> Simulation 2 <================"<<endl;
    for(i=0; i<20; i++)
    {
      
      	int p =(rand() % 9);
      	int q =(rand() % 59 + 1);
      	int r =(rand() % 12 + 1);

        a[i].setSequencenumb(i); //assigned as they arrive
		a[i].setPriority(p); //randomly generated and assigned (between 0 and 9 - 0 being highest)
		a[i].setLaundryTime(q);	//randomly generated
	    a[i].setNumberOfClothes(r); //randomly generated 
	    
	    cout<<"Sequence Number \t\t Priority \t\t Clothes \t\t"<<"Laundry Time"<<"\n";
	    cout<<a[i].getSequencenumb()<<"\t\t\t\t\t"<<a[i].getPriority()<<"\t\t\t"<<a[i].getNumberOfClothes()<<"\t\t\t\t"<<a[i].getLaundryTime()<<"\n";
	    cout<<"---------------------------------------------------------------------------------------------------"<<endl;


    }
    
    int j,n;
    i,j=0;
    for(i=0;i<10;i++){
        for(j=0;j<20;j++)
        {   
            int cP=a[j].getPriority();
            if(cP == i)
            {
                lp->Append(&a[j]);
 
            }
            else{
                continue;
            }
        }
    }

    lp->Apply(PrintWaiting);

    Person *w, *z;
    time=0;
    for(i=0;i<20;i++)
    {
        w = lp->Front();
        cout<<"\n Enterance:"<<"\n";
        cout<<"Order "<<"\t\t\t Person with sequence \t\t\t priority \t\t\t no. of clothes \t\t\t Laundry-time"<<"\n";
        cout<<i<<"\t\t\t\t"<<w->getSequencenumb()<<"\t\t\t\t\t"<<w->getPriority()<<"\t\t\t\t"<<w->getNumberOfClothes()<<"\t\t\t\t"<<w->getLaundryTime()<"\n";
       
        cout<<"\n -----------------------------------------------------------------------------------------------------------------------------------------------------------"<<"\n";
        lp->RemoveFront();
        if(i>0){
            cout<<"\n *********Person with Sequence " << z->getSequencenumb()<<" leaves the room.********************* \n";
        }
        time=time+w->getLaundryTime();
        z = w;
    }
    cout<<time<<"\n";
};

void
ThreadTest()
{
    Thread *t = new Thread("forked thread");
    t->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    
    SimpleThread(0);
    sim1();
    sim2();
}




