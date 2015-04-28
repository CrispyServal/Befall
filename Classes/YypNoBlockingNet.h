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
