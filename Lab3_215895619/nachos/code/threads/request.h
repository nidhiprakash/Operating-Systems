#include "../lib/list.h"
#include "../lib/bitmap.h"

class Request
{
    private:
        int uniqueID;
        int noOfPassengers;
        //int seatNumber;
        char deptCity;
        char destCity;

    public:
        Request(int val);
        Request() {

        };
        static int rUID;
        //----getters-----
        int getuniqueID();
        int getnoOfPassengers();
        //int getseatNumber();
        List<int> *getseatNumber();
        int getdeptCity();
        int getdestCity();

      
        int probabilityGen(int val);
        List<int> *seatsList = new List<int>();

        //comparing 2 request - using unique ID
         bool operator==(Request cmp){
            if(this->getuniqueID() == cmp.getuniqueID())
                return TRUE;
            else    
                return FALSE;
        }

};