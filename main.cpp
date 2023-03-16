#include <iostream>
#include "global.h"
#include "math.h"
using namespace std;


#define PI acos(-1)

double qReadDouble()
{
    double res=0;
    bool f=0;
    char c=0;
    c=fgetc(stdin);
    if(c=='-')
        f=1;
    while(c>='0' && c<='9')
    {
        res=res*10+c-48;
    }
    if(c=='.')
    {
        c=fgetc(stdin);
        double tmp=0.1;
        while(c>='0' && c<='9')
        {
            res+=tmp*(c-48);
            tmp/=10;
        }
    }
    if(f)
        res=-res;
    return res;
}
int qReadInt()
{
    int res=0;
    bool f=0;
    char c = 0;
    c=fgetc(stdin);
    if(c=='-')
        f=1;
    while(c>='0' && c<='9')
    {
        res=res*10+c-48;
    }
    if(f)
        res=-res;
    return res;
}

bool readUntilOK() {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //do something

    }
    return false;
}


bool readMap(){
    char line[1024];
    int row=0;
    while (fgets(line, sizeof line, stdin)) {
        if (line[0]=='0'&& line[1]=='K'){
            return true;
        }
        int robotID=0;
        for (int i=0;i<100;++i){
            if (line[i]=='A'){
               itemPos[0]=position{i*0.5+0.25,50-row*0.5-0.25}; 
               robotID++;
            }else if(line[i]>='1'&&line[i]<='9'){
               itemPos[10+line[i]-'0']=position{i*0.5+0.25,50-row*0.5-0.25}; 
            }
        }
        ++row;
    }
    return false;
}

//输出机器人移动指令
void moveTo(int i){

    string instruction[] = {"forward", "rotate", "buy", "sell", "destroy"};
    double dstx = itemPos.at(19).x, dsty = itemPos.at(19).y;
    double nowx = robot1234[i].pos_x, nowy = robot1234[i].pos_y;
    double angleSpeed = robot1234[i].angleSpeed;
    double lineSpeedX = robot1234[i].lineSpeed_x;
    double lineSpeedY = robot1234[i].lineSpeed_y;
    double towards = robot1234[i].towards;

    double outLineSpeed, outAngleSpeed;//要输出的速度

    double radiusLight = 0.45, radiusHeavy = 0.53, density = 20;
    //面积与质量
    double squareLight = PI * radiusLight * radiusLight;
    double squareHeavy = PI * radiusHeavy * radiusHeavy;
    double weightLight = squareLight * density;
    double weightHeavy = squareHeavy * density;
    //角加速度
    double maxAngleAccelerationLight = 50.0 / squareLight;
    double maxAngleAccelerationHeavy = 50.0 / squareHeavy;
    //线加速度
    double maxLineAccelerationLight = 250.0 / squareLight;
    double maxLineAccelerationHeavy = 250.0 / squareHeavy;
    //目标与当前夹角
    double angle;
    if(dstx != nowx){
        angle = atan((dsty - nowy) / (dstx - nowx));
    }
    else{
        if(dsty > nowy){
            angle = PI / 2;
        }
        else{
            angle = -PI / 2;
        }
    }
#if 1
    //线速度
    if((dstx - nowx) * lineSpeedX >= 0 && (dsty - nowy) * lineSpeedY >= 0){
        outLineSpeed = 6;
    }
    else{
        outLineSpeed = -2;
    }
    //角速度
    double derta1 = angleSpeed * angleSpeed - 2 * maxAngleAccelerationHeavy * (fabs(angle) + fabs(towards) - 2 * PI);
    double derta2 = angleSpeed * angleSpeed + 2 * maxAngleAccelerationHeavy * (fabs(angle) + fabs(towards));
    double time1 = sqrt(derta1) - fabs(angleSpeed);//加速度与速度方向相同
    double time2 = sqrt(derta2) + fabs(angleSpeed);//加速度与速度方向相反
    if(time1 < time2){
        outAngleSpeed = angleSpeed > 0 ? PI : -PI;
    }
    else{
        outAngleSpeed = angleSpeed > 0 ? -PI : PI;
    }
#endif

    //输出指令
    cout << instruction[0] << " " << i << " " << outLineSpeed << endl;  
    cout << instruction[1] << " " << i << " " << outAngleSpeed << endl;  


}

int main() {
    readMap();
    puts("OK");
    fflush(stdout);
    int frameID;
    while (scanf("%d", &frameID) != EOF) {
        readUntilOK();
        printf("%d\n", frameID);

        for(int i = 0; i < 4; i++)
            moveTo(i);


        printf("OK\n");
        fflush(stdout);
    }
    return 0;
}
