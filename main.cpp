#include "global.h"
#include "math.h"
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;

#define PI acos(-1)

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
    robot1234[i].workStationID = qReadInt();
    robot1234[i].itemID = qReadInt();
    robot1234[i].timeValue = qReadDouble();
    robot1234[i].collisionValue = qReadDouble();
    robot1234[i].angleSpeed = qReadDouble();
    robot1234[i].lineSpeed_x = qReadDouble();
    robot1234[i].lineSpeed_y = qReadDouble();
    robot1234[i].towards = qReadDouble();
    robot1234[i].pos_x = qReadDouble();
    robot1234[i].pos_y = qReadDouble();
  }
}
void readPerFrame() {
  char line[1024];
  int K = 0;
  fgets(line, sizeof line, stdin);
  K = qReadInt();
  for (int i = 0; i < K; i++) {
    fgets(line, sizeof line, stdin);
  }
  fprintf(stderr, "K=%d\n", K);
  readRobot();
  for (int i = 0; i < 4; i++) {
    fprintf(stderr, "Robot[%d]******:\n", i);
    fprintf(stderr, "WorkStationID:%d\n", robot1234[i].workStationID);
    fprintf(stderr, "itemID:%d\n", robot1234[i].itemID);
    fprintf(stderr, "timeValue:%d\n", robot1234[i].timeValue);
    fprintf(stderr, "collisionValue:%f\n", robot1234[i].collisionValue);
    fprintf(stderr, "angleSpeed:%f\n", robot1234[i].angleSpeed);
    fprintf(stderr, "lineSpeed_x:%f\n", robot1234[i].lineSpeed_x);
    fprintf(stderr, "lineSpeed_y:%f\n", robot1234[i].lineSpeed_y);
    fprintf(stderr, "pos_x: %f\n", robot1234[i].pos_x);
    fprintf(stderr, "pos_y: %f\n", robot1234[i].pos_y);
  }
  while (fgets(line, sizeof line, stdin)) {
    if (line[0] == 'O' && line[1] == 'K') {
      return;
    }
  }
}

bool readUntilOK() {
  char line[1024];
  while (fgets(line, sizeof line, stdin)) {
    if (line[0] == 'O' && line[1] == 'K') {
      return true;
    }
    // do something
  }
  return false;
}

bool readMap() {
  char line[1024];
  int row = 0;
  int workstationID = 0;
  int robotID = 0;
  while (fgets(line, sizeof line, stdin)) {
    if (line[0] == 'O' && line[1] == 'K') {
      return true;
    }
    for (int i = 0; i < 100; ++i) {
      if (line[i] == 'A') {
        itemPos[robotID] = position{i * 0.5 + 0.25, 50 - row * 0.5 - 0.25};
        robotID++;
      } else if (line[i] >= '1' && line[i] <= '9') {
        itemPos[(10 + line[i] - '0') * 100 + workstationID] =
            position{i * 0.5 + 0.25, 50 - row * 0.5 - 0.25};
        workstationID++;
      }
    }
    ++row;
  }
  return false;
}

double getDistance(position start, position end) {
  auto distance_x = start.x - end.x;
  auto distance_y = start.y - end.y;
  return sqrt(distance_x * distance_x + distance_y * distance_y);
}

int getNearestWorkStation(int workstationID, position robotPos) {
  double minDistance = 10000;
  int aimID = 0;
  for (auto i : itemPos) {
    if ((i.first / 100) % 10 == workstationID) {
      auto distance = getDistance(robotPos, i.second);
      if (distance < minDistance) {
        minDistance = distance;
        aimID = i.first;
      }
    }
  }
  return aimID;
}
/*
 * 机器人移动函数
 * @传入参数：int robotID,positon workstationPos 
 */
void moveTo(int robotID) {

  string instruction[] = {"forward", "rotate", "buy", "sell", "destroy"};
  auto aimID = getNearestWorkStation(9, itemPos[robotID]);
  fprintf(stderr, "workstation9:%d\n", aimID);
  fflush(stderr);
  double dstx = itemPos.at(aimID).x, dsty = itemPos.at(aimID).y;
  double nowx = robot1234[robotID].pos_x, nowy = robot1234[robotID].pos_y;
  fprintf(stderr, "dstx:%f dsty:%f\n", dstx, dsty);
  double angleSpeed = robot1234[robotID].angleSpeed;
  double lineSpeedX = robot1234[robotID].lineSpeed_x;
  double lineSpeedY = robot1234[robotID].lineSpeed_y;
  double towards = robot1234[robotID].towards;

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

  fprintf(stderr, "%s %d %f\n", instruction[0].c_str(), robotID, outLineSpeed);
  fprintf(stderr, "%s %d %f\n", instruction[1].c_str(), robotID, outAngleSpeed);
  fprintf(stderr,
          "------------------------------------------------------------");
  fflush(stderr);
}

int main() {
  readMap();
  puts("OK");
  fflush(stdout);
  int frameID;
  while (scanf("%d", &frameID) != EOF) {
    readPerFrame();
    printf("%d\n", frameID);

    for (int i = 0; i < 4; i++)
      moveTo(i);

    printf("OK\n");
    fflush(stdout);
  }
  return 0;
}
