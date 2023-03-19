#include <unordered_map>

struct position{
    double x;
    double y;
};
class WorkStation{
public:
    int ID;            //从1开始，唯一标识每个工作站
    int type;
    int productType;   //对应生产产品的种类，如果是0意味着不生产产品
    bool rawMaterialType[8]={false,false,false,false,false,false,false,false};
    int leftWorkTime;
    bool rawMaterialStatus[5]={false,false,false,false,false};
    int productStatus;
    position pos;

    WorkStation(){};
};

class Robot{
public:
    int ID;
    position pos;
    double angleSpeed;
    double lineSpeed_x;
    double lineSpeed_y;
    int workStationID;
    int itemID;
    double timeValue;
    double collisionValue;
    double towards;
    int lastBuy;
    bool isMovingToDst=false;
    Robot(int _ID):ID(_ID){}
    position getPos(){return pos;};
};

// Robot r1(0),r2(1),r3(2),r4(3);
// WorkStation* workStation = new WorkStation[50];



