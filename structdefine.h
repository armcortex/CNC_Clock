
#ifndef STRUCTDEFINE_H
#define STRUCTDEFINE_H

#include <Arduino.h>

#define XLimitHome 7
#define XLimitEnd 6

#define YLimitHome 5
#define YLimitEnd 4

#define ZLimitHome 4 

// Xaxis	step = 142
#define Xhome 0
#define Xend 1

// Yaxis	step = 244
#define Yhome 1 
#define Yend 1

// Zaxis	step = 100
#define Zhome 0
#define zStep 100
#define zDraw 100

typedef struct sCNCData
{
	int xPos, yPos, zPos;
	int xCnt, yCnt, zCnt;
    int xSet, ySet, zSet;
    int xDir, yDir, zDir;
	boolean xOk, yOk, zOk;
    int dir, temp, temp2;
    String str;
};

typedef struct sPos
{
	int xPos, yPos, zPos;
};

typedef struct sTime
{
	int Hour, Min, Sec;
    int H1, H2, M1, M2;
};

const byte OneCnt[] = { 0x01, 0x02,0x04, 0x08 };
const byte TwoCnt[] = { 0x03, 0x06,0x0c, 0x09 };
const byte OneTwoCnt[] = {0x01, 0x03, 0x02, 0x06, 
						  0x04, 0x0c, 0x08, 0x09 };
						  
#endif						  