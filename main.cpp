#include "global.h"
#include "math.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <type_traits>
#include <vector>
using namespace std;

#define PI acos(-1)

vector<WorkStation *> workStations;
Robot* robots = new Robot[4]{0, 1, 2,  3};
std::unordered_map<int,position> itemPos;
int frameID;
int money;
int workStationNum=0;
char line[1024];
string instruction[] = {"forward", "rotate", "buy", "sell", "destroy"};

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

void readRobot() {
  for (int i = 0; i < 4; i++) {
    robots[i].workStationID = qReadInt();
    robots[i].itemID = qReadInt();
    robots[i].timeValue = qReadDouble();
    robots[i].collisionValue = qReadDouble();
    robots[i].angleSpeed = qReadDouble();
    robots[i].lineSpeed_x = qReadDouble();
    robots[i].lineSpeed_y = qReadDouble();
    robots[i].towards = qReadDouble();
    robots[i].pos.x = qReadDouble();
    robots[i].pos.y = qReadDouble();
  }
}

int getProductType(int type){
  if (type>=1&&type<=7){
    return type; 
  }else if(type==8||type==9){
    return 0;
  }
  return -1;
}

void getRawMaterialType(int type,bool *rawMaterialType){
  switch (type){
    case 1:
    case 2:
    case 3:
      break;
    case 4:
      rawMaterialType[1]=true;
      rawMaterialType[2]=true;
      break;
    case 5:
      rawMaterialType[1]=true;
      rawMaterialType[3]=true;
      break;
    case 6:
      rawMaterialType[2]=true;
      rawMaterialType[3]=true;
      break;
    case 7:
      rawMaterialType[4]=true;
      rawMaterialType[5]=true;
      rawMaterialType[6]=true;;
      break;
    case 8:
      rawMaterialType[7]=true;
      break;
    case 9:
      for (int i=1;i<8;++i){
        rawMaterialType[i]=true;
      }
  }
}


void getRawMaterialStatus(int status,bool * rawMaterialStatus){
  int a[8]={0,0,0,0,0,0,0,0};
  for(int i=0; status>0; i++)    
    {    
        a[i]=status%2;    
        status= status/2;  
    }    
  for (int i=1;i<8;++i){
    if(a[i]==1){
      rawMaterialStatus[i]=true;
    }
  }
}

void readWorkStation(){
  workStationNum=qReadInt();
  for (int i = 0; i < workStationNum; i++) {
    workStations[i]->ID=i+1;
    getRawMaterialType(workStations[i]->type, workStations[i]->rawMaterialType);
    workStations[i]->type=qReadInt();
    workStations[i]->productType=getProductType(workStations[i]->type);
    workStations[i]->pos.x=qReadDouble();
    workStations[i]->pos.y=qReadDouble();
    workStations[i]->leftWorkTime=qReadInt();
    getRawMaterialStatus(qReadInt(), workStations[i]->rawMaterialStatus);
    workStations[i]->productStatus=qReadInt();
  }
}

void readPerFrame() {
  fgets(line,sizeof line,stdin);
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
    if (i->type== workStationType) {
      auto distance = getDistance(robotPos, i->pos);
      if (distance < minDistance) {
        minDistance = distance;
        aimID = i->ID;
      }
    }
  }
  return aimID;
}
//剩余角度(绝对值)和角速度(绝对值)对应关系
double angleSpeedFuc(double angle)
{
  if(abs(angle)>=PI)
    return  PI;
  else{
    double angleSpeed=0;
    angleSpeed=sqrt(angle*(2*PI-angle));
    return angleSpeed;
  }
}
//检查角度是否在合法范围内
bool anglePass(position now,position target,double towards)
{
  double distance=getDistance(now,target);
  double deltaAng=asin(0.4/distance);
  double targetAng=atan2((target.y-now.y),(target.x-now.x));
  if(abs(targetAng-towards)<=deltaAng)
    return true;
  else
    return false;
}
/*
 * 机器人移动函数
 * @传入参数：int robotID,positon aimPos 
 */
void moveTo(int robotID,position aimPos) {

  //auto aimID = getNearestWorkStation(9, itemPos[robotID]);
  double dstx = aimPos.x, dsty = aimPos.y;
  fprintf(stderr, "aimPos:(%f,%f)\n\n",dstx,dsty);
  fflush(stderr);
  double nowx = robots[robotID].pos.x, nowy = robots[robotID].pos.y;
  double angleSpeed = robots[robotID].angleSpeed;
  double lineSpeedX = robots[robotID].lineSpeed_x;
  double lineSpeedY = robots[robotID].lineSpeed_y;
  double towards = robots[robotID].towards;

  double outLineSpeed, outAngleSpeed; // 要输出的速度

  double radiusLight = 0.45, radiusHeavy = 0.53, density = 20;
  // 面积与质量
  double squareLight = PI * radiusLight * radiusLight;
  double squareHeavy = PI * radiusHeavy * radiusHeavy;
  double weightLight = squareLight * density;
  double weightHeavy = squareHeavy * density;
  // 角加速度
  double maxAngleAccelerationLight = 50.0 / squareLight;
  double maxAngleAccelerationHeavy = 50.0 / squareHeavy;
  // 线加速度
  double maxLineAccelerationLight = 250.0 / squareLight;
  double maxLineAccelerationHeavy = 250.0 / squareHeavy;
  // 目标与x轴夹角
  double angle;
  if (dstx != nowx) {
    angle = atan((dsty - nowy) / (dstx - nowx));
    if (dstx < nowx) {
      angle = dsty >= nowy ? (PI + angle) : (angle - PI);
    }
  } else {
    if (dsty > nowy) {
      angle = PI / 2;
    } else {
      angle = -PI / 2;
    }
  }
  // 目标与机器人朝向夹角
  double angle_towards = fabs(towards - angle);
#if 1
  // 线速度
  if (angle_towards > PI / 2)
    outLineSpeed = -2;
  else
    outLineSpeed = 6;
  // double result_x = (dstx - nowx) * lineSpeedX;
  // double result_y = (dsty - nowy) * lineSpeedY;
  // if (result_x >= 0 && result_y >= 0) {
  //   outLineSpeed = 6;
  // } else if(result_x < 0 && result_y < 0){
  //   outLineSpeed = -2;
  // }

  // 角速度
  double derta1 = angleSpeed * angleSpeed -
                  (maxAngleAccelerationHeavy + maxAngleAccelerationLight) *
                      (angle_towards - 2 * PI);
  double derta2 =
      angleSpeed * angleSpeed +
      (maxAngleAccelerationHeavy + maxAngleAccelerationLight) * angle_towards;
  double time1 = sqrt(derta1) - fabs(angleSpeed); // 加速度与速度方向相同
  double time2 = sqrt(derta2) + fabs(angleSpeed); // 加速度与速度方向相反
  if (time1 < time2) {
    outAngleSpeed = angleSpeed > 0 ? PI : -PI;
  } else {
    outAngleSpeed = angleSpeed > 0 ? -PI : PI;
  }
#endif

  // 输出指令
  printf("%s %d %f\n", instruction[0].c_str(), robotID, outLineSpeed);
  printf("%s %d %f\n", instruction[1].c_str(), robotID, outAngleSpeed);
  fflush(stdout);

  //fprintf(stderr, "%s %d %f\n", instruction[0].c_str(), robotID, outLineSpeed);
  //fprintf(stderr, "%s %d %f\n", instruction[1].c_str(), robotID, outAngleSpeed);
  //fprintf(stderr,
          //"------------------------------------------------------------");
  //fflush(stderr);
}

int main() {
  readMap();
  puts("OK");
  fflush(stdout);
  while (scanf("%d", &frameID ) != EOF) {
    readPerFrame();
    printf("%d\n", frameID);

    for (int i = 0; i < 4; i++)
      moveTo(i,workStations[9]->pos);

    printf("OK\n");
    fflush(stdout);
  }
  return 0;
}
