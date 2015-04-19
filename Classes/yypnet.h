#ifndef YYPNET_H
#define YYPNET_H
#include <iostream>
#include <thread>
#include<stdio.h>
#include <winsock2.h>
//#include "stdafx.h"
#pragma comment (lib, "ws2_32")
//#pragma once

enum techNameEnum// just for test
{
	notech,
	tech1,
	tech2,
	tech3
};

enum SoldierEnum//just for test
{
	nosol,
	sol1,
	sol2,
	sol3
};

struct newSoldierStruct
{
	SoldierEnum soldierType;
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

class YypNet
{
private:
	SOCKET sclient;
	SOCKET slisten;
	bool lock;	
	bool connected;
public:
	YypNet();
	~YypNet();
	techNameEnum tech;
	newSoldierStruct newSoldier;
	twoPointStruct points;
	whichEnum which;
	void lockOn()
	{
		lock = true;
	}
	bool sendTech(techNameEnum tech);
	bool sendNewSoldier(newSoldierStruct newSoldier);
	bool sedTwoPoind(twoPointStruct points);
	bool read();
	bool startServer(int pot);
	bool serverListen(int pot);
	bool endServer();
	bool makeConnect(char* IP, int pot);
	bool deleteConnect();
	bool beConnected()
	{
		return connected;
	}
};
#endif
