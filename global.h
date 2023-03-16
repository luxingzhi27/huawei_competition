#include <unordered_map>

class WorkStation{
    int ID;
    int productID;
    int rawMaterialID[7];

    WorkStation(int _ID,int _productID,int _rawMaterialID[7]):ID(_ID),productID(_productID){
        for (int i=0;i<7;++i){
            rawMaterialID[i]=_rawMaterialID[i];
        }
    }
};

class Robot{
public:
    int ID;
    double pos_x;
    double pos_y;
    double angleSpeed;
    double lineSpeed_x;
    double lineSpeed_y;
    int workStationID;
    int itemID;
    double timeValue;
    double collisionValue;
    double towards;

    Robot(int _ID):ID(_ID){}
};

// Robot r1(0),r2(1),r3(2),r4(3);
Robot* robot1234 = new Robot[4]{0, 1, 2, 3};
// WorkStation* workStation = new WorkStation[50];

struct position{
    double x;
    double y;
};

std::unordered_map<int,position> itemPos;