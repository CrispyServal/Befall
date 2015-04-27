#ifndef YYPNOBLOCKINGNET_H
#define YYPNOBLOCKINGNET_H
#include <iostream>
#include <thread>
#include <stdio.h>
#include <winsock2.h>
#include "MyEnums.h"
#pragma comment (lib, "ws2_32")

using std::cout;
using std::endl;

struct newSoldierStruct
{
	UnitEnum unit;
	int x1;
	int y1;
};

struct twoPointStruct
{
	int x1;
	int y1;
	int x2;
	int y2;
};

enum whichEnum
{
	newTech,
	newSoldier,
	twoPoints
};

class YypNoBlockingNet
{
	SOCKET sclient;
	SOCKET slisten;
	sockaddr_in remoteAddr;
	int nAddrlen;
	bool lock;	
public:
	TechEnum tech;
	newSoldierStruct newSoldier;
	twoPointStruct points;
	whichEnum which;
	void lockOn()
	{
		lock = true;
	}
	bool lockState();
	bool sendTech(TechEnum tech);
	bool sendNewSoldier(newSoldierStruct newSoldier);
	bool sedTwoPoind(twoPointStruct points);
	bool read();
	bool startServer(int pot);
	bool acceptConnect();
	bool endServer();
	bool makeConnect(char* IP, int pot);
	bool deleteConnect();
	YypNoBlockingNet();
	~YypNoBlockingNet();
};
#endif
