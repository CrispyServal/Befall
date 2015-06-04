#ifndef YYPNOBLOCKINGNET_H
#define YYPNOBLOCKINGNET_H
#include <iostream>
#include <thread>
#include <stdio.h>
#include <winsock2.h>
#include "MyStructs.h"

#pragma comment (lib, "ws2_32")

using std::cout;
using std::endl;

struct newSoldierStruct
{
	UnitEnum unit;
	MyPointStruct loc;
};

struct twoPointStruct
{
	MyPointStruct first;
	MyPointStruct second;
};

enum whichEnum
{
	newTech,
	newSoldier,
	twoPoints,
	onePoint,
	end,
	youwin,
	mistnet
};

class YypNoBlockingNet
{
private:
	SOCKET sclient;
	SOCKET slisten;
	sockaddr_in remoteAddr;
	sockaddr_in serAddr;
	int nAddrlen;
	bool lock;
	TechEnum mTech;
	newSoldierStruct mNewSoldier;
	twoPointStruct mPoints;
	MyPointStruct mOnePoint;
	whichEnum which;
	int mistOn;
public:
	TechEnum getTech();
	newSoldierStruct getNewSoldier();
	twoPointStruct getPoints();
	MyPointStruct getOnePoint();
	whichEnum getWhich();
	int getMist();
	void lockOn()
	{
		lock = true;
	}
	bool isLocked();
	bool sendTech(TechEnum tech);
	bool sendNewSoldier(newSoldierStruct newSoldier);
	bool sendTwoPoint(twoPointStruct points);
	bool sendOnePoint(MyPointStruct onePoint);
	bool sendEnd();
	bool sendYouWin();
	bool sendMist(int mistOn);
	bool read();
	bool startServer(int pot);
	bool acceptConnect();
	bool endServer();
	bool startConnect(char* IP, int pot);
	bool makeConnect();
	bool deleteConnect();
	YypNoBlockingNet();
	~YypNoBlockingNet();
};
#endif
