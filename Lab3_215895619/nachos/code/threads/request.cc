#include "request.h"
int Request::rUID=0;

Request::Request(int val){
    int cityval = val;
    uniqueID = ++rUID; 
    noOfPassengers = rand()%5 + 1;
    deptCity = cityval;
    destCity = probabilityGen(cityval);
}

int Request::getuniqueID()
{
    return uniqueID;
}
int Request::getnoOfPassengers()
{
    return noOfPassengers;
}

List<int> *Request::getseatNumber() {
    return seatsList;
}

int Request::getdeptCity()
{
    return deptCity;
}

int Request::getdestCity()
{
    return destCity;
}


// to genereate destinations for passgengers in a set of a request based on probability
int Request::probabilityGen(int val)
{
    int destination;
    int random = rand()%100+1;
    if(val == 0)
    {
        if(random > 0 && random < 30) {destination = 4;}
       else if(random > 30 && random < 60) {destination = 3;}
       else if(random > 60 && random < 90) {destination = 2;}
       else destination = 1;
        
    }
    else if(val == 1)
    {
        if(random > 25 && random < 50) {destination = 4;}
        if(random > 51 && random < 80) {destination = 3;}
        else destination = 2;

    }
    else if(val == 2)
    {
        if(random > 0 && random < 80 ) {destination = 3;}
        else destination = 4;
    }
    else 
    {
        destination = 4;
    }

    return destination;
}

