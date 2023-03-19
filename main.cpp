#include "global.h"
#include "math.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <vector>
using namespace std;

#define PI acos(-1)
const double r = 0.45, R = 0.53, p = 20,
             F = 250; //半径和密度和最大作用力（小写无货，大写有货）
const double m = r * r * PI * p, M = R * R * PI * p; //质量：m无货，M有货
const double a = F / m, A = F / M;                   //最大加速度
const double s_min = 18 / a, S_min = 18 / A; //最短加速（减速）距离
const double ang_a = 5 / (PI * r * r * r * r),
             ang_A = 5 / (PI * R * R * R * R); //最大角加速度

vector<WorkStation *> workStations;
Robot *robots = new Robot[4]{0, 1, 2, 3};
std::unordered_map<int, position> itemPos;
int frameID;
int money;
int workStationNum = 0;
char line[1024];
string instruction[] = {"forward", "rotate", "buy", "sell", "destroy"};

double qReadDouble();
int qReadInt();
position getDst(int robotType);
void readRobot();
int getProductType(int type);
void getRawMaterialType(int type, bool *rawMaterialType);
void getRawMaterialStatus(int status, bool *rawMaterialStatus);
void readWorkStation();
void readPerFrame();
bool readMap();
double getDistance(position start, position end);
int getNearestWorkStation(int workStationType, position robotPos);
int getAimWorkStationType(int robotID, int cnt);
void moveToTest(int robotID, position aimPos);

int getAimWorkStationType(int robotID, int cnt) {
  if (robotID == 0) {
    switch (cnt) {
    case 1:
      return 1;
      break;
    case 2:
      return 4;
      break;
    case 3:
      return 2;
      break;
    case 4:
      return 4;
    }
  } else if (robotID == 1) {
    switch (cnt) {
    case 1:
      return 1;
      break;
    case 2:
      return 5;
      break;
    case 3:
      return 3;
      break;
    case 4:
      return 5;
      break;
    }
  } else if (robotID == 2) {
    switch (cnt) {
    case 1:
      return 2;
      break;
    case 2:
      return 6;
      break;
    case 3:
      return 3;
      break;
    case 4:
      return 6;
      break;
    }
  } else if (robotID == 3) {
    switch (cnt) {
    case 1:
      return 4;
      break;
    case 2:
      return 7;
      break;
    case 3:
      return 5;
      break;
    case 4:
      return 7;
      break;
    case 5:
      return 6;
      break;
    case 6:
      return 7;
      break;
    case 7:
      return 8;
      break;
    case 8:
      return 9;
      break;
    }
  }
  return -1;
}

double qReadDouble() {
  double res = 0;
  bool f = 0;
  char c = 0;
  c = fgetc(stdin);
  if (c == '-') {
    f = 1;
    c = fgetc(stdin);
  }
  while (c >= '0' && c <= '9') {
    res = res * 10 + c - 48;
    c = fgetc(stdin);
  }
  if (c == '.') {
    c = fgetc(stdin);
    double tmp = 0.1;
    while (c >= '0' && c <= '9') {
      res += tmp * (c - 48);
      tmp /= 10;
      c = fgetc(stdin);
    }
  }
  if (f)
    res = -res;
  return res;
}

int qReadInt() {
  int res = 0;
  bool f = 0;
  char c = 0;
  c = fgetc(stdin);
  if (c == '-') {
    f = 1;
    c = fgetc(stdin);
  }
  while (c >= '0' && c <= '9') {
    res = res * 10 + c - 48;
    c = fgetc(stdin);
  }
  if (f)
    res = -res;
  return res;
}

/*
根据机器人种类得到目的地位置
*/
int getDst(int robotType, int cnt) {
  auto dstWorkStationType=getAimWorkStationType(robotType, cnt);
  return getNearestWorkStation(dstWorkStationType,
                               robots[robotType].getPos());
}

int getRobotxWorkStationNum(int robotType) {
  if (robotType == 0 || robotType == 1 || robotType == 2)
    return 4;
  else
    return 8;
}

void robotProcess(int robotType) {
  static int cnt[4] = {1, 1, 1, 1};
  static position pos[4];
  static int dstWorkStationID[4];
  if (robots[robotType].isMovingToDst == false) {
    robots[robotType].isMovingToDst = true;
    if (cnt[robotType] == getRobotxWorkStationNum(robotType) + 1)
      cnt[robotType] = 1;
    dstWorkStationID[robotType] = getDst(robotType, cnt[robotType]);
    //fprintf(stderr, "robotType:%d  dstWorkStationID:%d\n", robotType,
            //dstWorkStationID[robotType]);
    //fflush(stderr);
    pos[robotType].x = workStations[dstWorkStationID[robotType]]->pos.x;
    pos[robotType].y = workStations[dstWorkStationID[robotType]]->pos.y;
    cnt[robotType]++;
  }
  // fprintf(stderr,"robotType:%d  pos_x:%fpos_y:%f\n",robotType,pos[robotType].x,pos[robotType].y);
  // fflush(stderr);
  moveToTest(robotType, pos[robotType]);
  if (robots[robotType].workStationID == dstWorkStationID[robotType]) {
    robots[robotType].isMovingToDst = false;
    switch (robotType) {
    case 0:
      if (workStations[dstWorkStationID[robotType]]->type== 1&&robots[0].lastBuy!=1)
        printf("buy 0\n");
      else if(workStations[dstWorkStationID[robotType]]->type==2&&robots[0].lastBuy!=2)
        printf("buy 0\n");
      else if (workStations[dstWorkStationID[robotType]]->type== 4)
        printf("sell 0\n");
      break;
    case 1:
      if (workStations[dstWorkStationID[robotType]]->type== 1&&robots[1].lastBuy!=1)
        printf("buy 1\n");
      if(    workStations[dstWorkStationID[robotType]]->type == 3&&robots[1].lastBuy!=3)
        printf("buy 1\n");
      else if (workStations[dstWorkStationID[robotType]]->type == 5)
        printf("sell 1\n");
      break;
    case 2:
      if (workStations[dstWorkStationID[robotType]]->type == 3&&robots[2].lastBuy!=3) 
        printf("buy 2\n");
      if(    workStations[dstWorkStationID[robotType]]->type == 2&&robots[2].lastBuy!=2)
        printf("buy 2\n");
      else if (workStations[dstWorkStationID[robotType]]->type == 6)
        printf("sell 2\n");
      break;
    case 3:
      if (workStations[dstWorkStationID[robotType]]->type == 4&&robots[3].lastBuy!=4)
        printf("buy 3\n");
      
      else if(workStations[dstWorkStationID[robotType]]->type == 5&&robots[3].lastBuy!=5)
        printf("buy 3\n");
      else if(workStations[dstWorkStationID[robotType]]->type == 6&&robots[3].lastBuy!=6)
        printf("buy 3\n");
      else if (workStations[dstWorkStationID[robotType]]->type == 9)
        printf("sell 3\n");
      else if (workStations[dstWorkStationID[robotType]]->type == 7 ||
               workStations[dstWorkStationID[robotType]]->type == 8) {
        printf("buy 3\n");
        printf("sell 3\n");
      }
      break;
    }
  }
}

void dispatch() {
  for (int i = 0; i < 4; ++i) {
    robots[i].lastBuy=robots[i].itemID==0?robots[i].lastBuy:robots[i].itemID;
    //fprintf(stderr, "robot:%d,lastBuy:%d\n",i,robots[i].lastBuy);
    //fflush(stderr);
    robotProcess(i);
  }
}

void readRobot() {
  for (int i = 0; i < 4; i++) {
    cin>>robots[i].workStationID; //= qReadInt();
    cin>>robots[i].itemID;// = qReadInt();
    cin>>robots[i].timeValue;// = qReadDouble();
    cin>>robots[i].collisionValue; //= qReadDouble();
    cin>>robots[i].angleSpeed;// = qReadDouble();
    cin>>robots[i].lineSpeed_x;// = qReadDouble();
    cin>>robots[i].lineSpeed_y;// = qReadDouble();
    cin>>robots[i].towards ;//= qReadDouble();
    cin>>robots[i].pos.x ;//= qReadDouble();
    cin>>robots[i].pos.y;// = qReadDouble();
  }
}

int getProductType(int type) {
  if (type >= 1 && type <= 7) {
    return type;
  } else if (type == 8 || type == 9) {
    return 0;
  }
  return -1;
}

void getRawMaterialType(int type, bool *rawMaterialType) {
  switch (type) {
  case 1:
  case 2:
  case 3:
    break;
  case 4:
    rawMaterialType[1] = true;
    rawMaterialType[2] = true;
    break;
  case 5:
    rawMaterialType[1] = true;
    rawMaterialType[3] = true;
    break;
  case 6:
    rawMaterialType[2] = true;
    rawMaterialType[3] = true;
    break;
  case 7:
    rawMaterialType[4] = true;
    rawMaterialType[5] = true;
    rawMaterialType[6] = true;
    ;
    break;
  case 8:
    rawMaterialType[7] = true;
    break;
  case 9:
    for (int i = 1; i < 8; ++i) {
      rawMaterialType[i] = true;
    }
  }
}

void getRawMaterialStatus(int status, bool *rawMaterialStatus) {
  int a[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; status > 0; i++) {
    a[i] = status % 2;
    status = status / 2;
  }
  for (int i = 1; i < 8; ++i) {
    if (a[i] == 1) {
      rawMaterialStatus[i] = true;
    }
  }
}

void readWorkStation() {
  workStationNum = qReadInt();
  for (int i = 0; i < workStationNum; i++) {
    workStations[i]->ID = i;
    getRawMaterialType(workStations[i]->type, workStations[i]->rawMaterialType);
    cin>>workStations[i]->type;// = qReadInt();
    workStations[i]->productType = getProductType(workStations[i]->type);
    cin>>workStations[i]->pos.x;// = qReadDouble();
    cin>>workStations[i]->pos.y ;//= qReadDouble();
    cin>>workStations[i]->leftWorkTime ;//= qReadInt();
    int tmp=0;
    cin>>tmp;
    getRawMaterialStatus(tmp/*qReadInt()*/, workStations[i]->rawMaterialStatus);
    cin>>workStations[i]->productStatus;// = qReadInt();
  }
}

void readPerFrame() {
  fgets(line, sizeof line, stdin);
  money=std::atoi(line);
  readWorkStation();
  readRobot();
  /*for (int i = 0; i < 4; i++) {
    fprintf(stderr, "Robot[%d]******:\n", i);
    fprintf(stderr, "WorkStationID:%d\n", robots[i].workStationID);
    fprintf(stderr, "itemID:%d\n", robots[i].itemID);
    fprintf(stderr, "timeValue:%f\n", robots[i].timeValue);
    fprintf(stderr, "collisionValue:%f\n", robots[i].collisionValue);
    fprintf(stderr, "angleSpeed:%f\n", robots[i].angleSpeed);
    fprintf(stderr, "lineSpeed_x:%f\n", robots[i].lineSpeed_x);
    fprintf(stderr, "lineSpeed_y:%f\n", robots[i].lineSpeed_y);
    fprintf(stderr, "pos_x: %f\n", robots[i].pos.x);
    fprintf(stderr, "pos_y: %f\n", robots[i].pos.y);
  }*/
  while (fgets(line, sizeof line, stdin)) {
    if (line[0] == 'O' && line[1] == 'K') {
      return;
    }
  }
}

bool readMap() {
  while (fgets(line, sizeof line, stdin)) {
    if (line[0] == 'O' && line[1] == 'K') {
      return true;
    }
    for (int i = 0; i < 100; ++i) {
      if (line[i] >= '1' && line[i] <= '9') {
        workStationNum++;
        workStations.push_back(new WorkStation);
      }
    }
  }
  return false;
}

double getDistance(position start, position end) {
  auto distance_x = start.x - end.x;
  auto distance_y = start.y - end.y;
  return sqrt(distance_x * distance_x + distance_y * distance_y);
}

int getNearestWorkStation(int workStationType, position robotPos) {
  double minDistance = 10000;
  int aimID = 0;
  for (auto i : workStations) {
    if (i->type == workStationType) {
      auto distance = getDistance(robotPos, i->pos);
      fprintf(stderr,"workStationID:%d  pos_x:%f  pos_y:%f\n",i->ID,i->pos.x,i->pos.y);
      fflush(stderr);
      if (distance < minDistance) {
        minDistance = distance;
        aimID = i->ID;
      }
    }
  }
  return aimID;
}

//获取机器人速度大小
double _getLineSPeed(double linespeed_x, double linespeed_y) {
  double lineSpeed =
      sqrt(linespeed_x * linespeed_x + linespeed_y * linespeed_y);
  return lineSpeed;
}
double getLineSpeed(int robotID) {
  return _getLineSPeed(robots[robotID].lineSpeed_x,
                       robots[robotID].lineSpeed_y);
}
// 剩余角度(绝对值)和角速度(绝对值)对应关系
double angleSpeedFuc(double angle) {
  if (abs(angle) >= PI * 0.5)
    return PI;
  else {
    double angleSpeed = 0;
    angleSpeed = 2 * sqrt(angle * (PI - angle));
    return angleSpeed;
  }
}
// // 检查角度是否在合法范围内
// bool anglePass(position now, position target, double towards) {
//   double distance = getDistance(now, target);
//   double deltaAng = asin(0.4 / distance);
//   double targetAng = atan2((target.y - now.y), (target.x - now.x));
//   if (abs(targetAng - towards) <= deltaAng)
//     return true;
//   else
//     return false;
// }

//机器人移动函数（测试）总共包含三部分
void _rotate(int robotID, double diffAng, double deltaAng)
{
  double angleSpeed=robots[robotID].angleSpeed;
  double towards=robots[robotID].towards;
  double acAngSpeed;
  double minAng;    //最短减速/加速距离
  double f= diffAng>0 ? 1 : (-1);
  diffAng=fabs(diffAng);
  if(diffAng>(PI))
    printf("%s %d %f\n", instruction[0].c_str(), robotID, 0.0); //降速
//    fprintf(stderr,"%s %d %f***0\n", instruction[0].c_str(), robotID, 0.0);
  if(!robots[robotID].itemID) //判断是否拿货物
    acAngSpeed=ang_a;
  else
    acAngSpeed = ang_A;
  minAng = angleSpeed * angleSpeed / (2 * acAngSpeed);
  if (fabs(diffAng - minAng) <= deltaAng) //可以减角速度为0
  {
    printf("%s %d %f\n", instruction[1].c_str(), robotID, 0.0);
//    fprintf(stderr, "%s %d %f\n", instruction[1].c_str(), robotID, 0.0);
  }
  else
  {
    double outAngleSpeed=angleSpeedFuc(diffAng)*f;
    printf("%s %d %f\n", instruction[1].c_str(), robotID, outAngleSpeed);
//    fprintf(stderr, "%s %d %f\n", instruction[1].c_str(), robotID, outAngleSpeed);
  }
}
void _run(int robotID, double distance, double deltaDis)
{
  double lineSpeed=getLineSpeed(robotID);
//  fprintf(stderr,"should be accelerating!************LineSpeed= %f",lineSpeed);
  double minDis,acSpeed;      //最小加速/减速距离，最大加速度
  if(!robots[robotID].itemID) //机器人没有货
  {
    minDis = s_min;
    acSpeed = a;
  } else //有货
  {
    minDis = S_min;
    acSpeed = A;
  }
  if (!lineSpeed) //线速度为零，需要加速
  {
    if (distance >= (2 * minDis)) //满足最大加速度
    {
      printf("%s %d %f\n", instruction[0].c_str(), robotID, 6.0);
    //  fprintf(stderr, "%s %d %f :::1\n", instruction[0].c_str(), robotID, 6.0);
    }
    else
    {
      double outLineSpeed=sqrt(distance/acSpeed);   //计算适合的速度
      printf("%s %d %f\n", instruction[0].c_str(), robotID, outLineSpeed);
      // fprintf(stderr, "%s %d %f :::2\n", instruction[0].c_str(), robotID, outLineSpeed);
    }
  } else //线速度不为零，判断是否要减速
  {
    minDis=lineSpeed*lineSpeed/(2*acSpeed);
    if(abs(distance-minDis)<deltaDis) //满足减速条件
    {
      printf("%s %d %f\n", instruction[0].c_str(), robotID, 0.0); 
       //fprintf(stderr, "%s %d %f :::0\n", instruction[0].c_str(), robotID, 0.0);
    }
  }
}
void moveToTest(int robotID, position aimPos) {
  // double lineSpeed=getLineSpeed(robotID);
  position now = robots[robotID].pos;
  double towards = robots[robotID].towards;
  double distance = getDistance(now, aimPos);
  double deltaAng = asin(0.3 / distance);
  double targetAng = atan2((aimPos.y - now.y), (aimPos.x - now.x));
  double diffAngle = targetAng - towards; 
  double DiffAngle = fabs(diffAngle);
  // if(fabs(towards)>PI)
  //   return;
  if (DiffAngle >= 2 * PI) //选择小的角度，角度正负与要旋转的正负相同
  {
    if(diffAngle>0)
      diffAngle-=(2*PI);
    else
      diffAngle+=(2*PI);
    DiffAngle-=(2*PI);  
  }
  if (DiffAngle > deltaAng)  //需要转弯
    _rotate(robotID,diffAngle,deltaAng);
  else                              //角度满足，可以直走
  {
    double real_Dis=cos(DiffAngle)*distance;
    double tmp=sin(DiffAngle)*distance;
    double deltaDis=sqrt(0.16-tmp*tmp);
    _run(robotID,real_Dis,deltaDis);
  }
}
/*
 * 机器人移动函数
 * @传入参数：int robotID,positon aimPos
 */
// void moveTo(int robotID, position aimPos) {

  // auto aimID = getNearestWorkStation(9, itemPos[robotID]);
  // double dstx = aimPos.x, dsty = aimPos.y;
  // // fprintf(stderr, "aimPos:(%f,%f)\n\n", dstx, dsty);
  // fflush(stderr);
  // double nowx = robots[robotID].pos.x, nowy = robots[robotID].pos.y;
  // double angleSpeed = robots[robotID].angleSpeed;
  // double lineSpeedX = robots[robotID].lineSpeed_x;
  // double lineSpeedY = robots[robotID].lineSpeed_y;
  // double towards = robots[robotID].towards;

//   double outLineSpeed, outAngleSpeed; // 要输出的速度

//   double radiusLight = 0.45, radiusHeavy = 0.53, density = 20;
//   // 面积与质量
//   double squareLight = PI * radiusLight * radiusLight;
//   double squareHeavy = PI * radiusHeavy * radiusHeavy;
//   double weightLight = squareLight * density;
//   double weightHeavy = squareHeavy * density;
//   // 角加速度
//   double maxAngleAccelerationLight = ang_a;
//   double maxAngleAccelerationHeavy = ang_A;
//   // 线加速度
//   double maxLineAccelerationLight = 250.0 / squareLight;
//   double maxLineAccelerationHeavy = 250.0 / squareHeavy;
//   // 目标与x轴夹角
//   double angle;
//   if (dstx != nowx) {
//     angle = atan((dsty - nowy) / (dstx - nowx));
//     if (dstx < nowx) {
//       angle = dsty >= nowy ? (PI + angle) : (angle - PI);
//     }
//   } else {
//     if (dsty > nowy) {
//       angle = PI / 2;
//     } else {
//       angle = -PI / 2;
//     }
//   }
//   // 目标与机器人朝向夹角
//   double angle_towards = fabs(towards - angle);
// #if 1
//   // 线速度
//   if (angle_towards > PI / 2)
//     outLineSpeed = -2;
//   else
//     outLineSpeed = 6;
//   // double result_x = (dstx - nowx) * lineSpeedX;
//   // double result_y = (dsty - nowy) * lineSpeedY;
//   // if (result_x >= 0 && result_y >= 0) {
//   //   outLineSpeed = 6;
//   // } else if(result_x < 0 && result_y < 0){
//   //   outLineSpeed = -2;
//   // }// position getDestination(int robotID) { return workStations[15]->posLight) *
//                       (angle_towards - 2 * PI);
//   double derta2 =
//       angleSpeed * angleSpeed +
//       (maxAngleAccelerationHeavy + maxAngleAccelerationLight) *
//       angle_towards;
//   double time1 = sqrt(derta1) - fabs(angleSpeed); // 加速度与速度方向相同
//   double time2 = sqrt(derta2) + fabs(angleSpeed); // 加速度与速度方向相反
//   if (time1 < time2) {
//     outAngleSpeed = angleSpeed > 0 ? PI : -PI;
//   } else {
//     outAngleSpeed = angleSpeed > 0 ? -PI : PI;
//   }
// #endif

//   // 输出指令
//   printf("%s %d %f\n", instruction[0].c_str(), robotID, outLineSpeed);
//   printf("%s %d %f\n", instruction[1].c_str(), robotID, outAngleSpeed);
//   fflush(stdout);

//   // fprintf(stderr, "%s %d %f\n", instruction[0].c_str(), robotID,
//   // outLineSpeed); fprintf(stderr, "%s %d %f\n", instruction[1].c_str(),
//   // robotID, outAngleSpeed); fprintf(stderr,
//   //"------------------------------------------------------------");
//   // fflush(stderr);
// }

// position getDestination(int robotID) { return workStations[15]->pos; }

// void buyAndSell(int robotID) {
//   // 周围有工作台
//   if (robots[robotID].workStationID != -1) {
//     // 机器人里有货
//     if (robots[robotID].itemID != 0) {
//       // 货物与当前工作台的需求符合且当前工作台缺货,则卖出
//       if (workStations[robots[robotID].workStationID]
//               ->rawMaterialType[robots[robotID].itemID] &&
//           !workStations[robots[robotID].workStationID]
//                ->rawMaterialStatus[robots[robotID].itemID]) {
//         printf("%s %d\n", instruction[3].c_str(), robotID);
//       }
//     } // end of 有货
//     // 机器人没有货物且周围工作台有货
//     else {
//       for (int i = 1; i < 8; i++) {
//         if (workStations[robots[robotID].workStationID]->rawMaterialStatus[i]) {
//           printf("%s %d\n", instruction[2].c_str(), robotID);
//           break;
//         }
//       }
//     } // end of 没货

//   } // end of 有工作台

//   return;
// }

int main() {
  readMap();
  puts("OK");
  fflush(stdout);
  while (scanf("%d", &frameID) != EOF) {
    readPerFrame();
    printf("%d\n", frameID);

    // for (int i = 0; i < 4; i++) {
    //       buyAndSell(i);
    // moveToTest(i, getDestination(i));
    dispatch();

    printf("OK\n");
    fflush(stdout);
  }
  return 0;
}
