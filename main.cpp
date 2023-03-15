#include <iostream>
#include "global.h"
using namespace std;

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

int main() {
    readMap();
    puts("OK");
    fflush(stdout);
    int frameID;
    while (scanf("%d", &frameID) != EOF) {
        readUntilOK();
        printf("%d\n", frameID);
        int lineSpeed = 3;
        double angleSpeed = 1.5;
        for(int robotId = 0; robotId < 4; robotId++){
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
        }
        printf("OK\n");
        fflush(stdout);
    }
    return 0;
}
