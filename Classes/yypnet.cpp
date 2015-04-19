#include "yypnet.h"
#define MSIZE 24 // standard message size = sizeof(sendData) 

YypNet::YypNet()
	:	
	connected(false),
	lock(true)
{
}
YypNet::~YypNet()
{
}
bool YypNet::sendTech(techNameEnum tech)
{
	int sendData[6];
	sendData[0] = 0;
	sendData[1] = 0;
	sendData[2] = 0;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = (int) tech;
	int sen = send(sclient, (char *)&sendData, sizeof(sendData) / sizeof(char), 0);
	if (sen < 0)// NO CONNECT
		return false;
	else if (sen != MSIZE)//invalid data
		return false;
	else
		return true;
}
bool YypNet::sendNewSoldier(newSoldierStruct newSoldier)
{
	int sendData[6];
	sendData[0] = 1;
	sendData[1] = newSoldier.x1;
	sendData[2] = newSoldier.y1;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = (int) newSoldier.soldierType;
	int sen = send(sclient, (char *)&sendData, sizeof(sendData) / sizeof(char), 0);//??
	if (sen < 0)// no connect
		return false;
	else if (sen != MSIZE)
		return false;
	else
		return true;
}
bool YypNet::sedTwoPoind(twoPointStruct points)
{
	int sendData[6];
	sendData[0] = 2;
	sendData[1] = points.x1;
	sendData[2] = points.y1;
	sendData[3] = points.x2;
	sendData[4] = points.y2;
	sendData[5] = 0;
	int sen = send(sclient, (char *)&sendData, sizeof(sendData) / sizeof(char), 0);//??
	if (sen < 0)
		return false;
	else if (sen != MSIZE)
		return false;
	else
		return true;
}
bool YypNet::read()
{
	char revData[255];// need to adjust size
	int ret = recv(sclient, revData, 255, 0);
	if (ret < 0 )// means no connect
		return false;
	if (ret > 0 && ret != MSIZE)//invalid data
		return false;
	if (ret == MSIZE)
	{
		int *mess = (int *)revData;
		int flag = mess[0];
		which = (whichEnum) flag;
		int x1 = mess[1];
		int y1 = mess[2];
		int x2 = mess[3];
		int y2 = mess[4];
		int enu = mess[5];
		switch (flag)
		{
		case 0:tech = (techNameEnum) enu; break;
		case 1:newSoldier.soldierType = (SoldierEnum)enu; newSoldier.x1 = x1; newSoldier.y1 = y1; break;
		case 2:points.x1 = x1; points.y1 = y1; points.x2 = x2; points.y2 = y2; break;
		}
		lock = false;
	}
	return true;
}
bool YypNet::startServer(int pot)
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
	//创建套接字
	slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		//printf("socket error !");
		return 0;
	}
	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(pot);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		//printf("bind error !");
	}
	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}
	/*
	//循环接收数据
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	while (true)
	{
		printf("等待连接...\n");
		sclient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sclient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
		//
		//need to do sth
		return 1;
	}
	*/
	return true;
}

bool YypNet::serverListen(int pot)
{
	//循环接收数据
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	//printf("等待连接...\n");
	//noneblock
	sclient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
	if (sclient == INVALID_SOCKET)
	{
		//printf("accept error !");
		return true;
	}
	//printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
	//
	//need to do sth
	connected = true;
	return true;
}

bool YypNet::endServer()
{
	if (!closesocket(sclient) && !WSACleanup())
		return true;
	else
		return false;;
}
bool YypNet::makeConnect(char *IP, int pot)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		printf("invalid socket !");
		return 0;
	}
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(pot);
	serAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
		return 0;
	}
	return 1;
}
bool YypNet::deleteConnect()
{
	if (!closesocket(sclient) && !WSACleanup())
		return true;
	else
		return false;
}
