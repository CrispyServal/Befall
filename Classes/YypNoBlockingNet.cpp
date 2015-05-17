#include "YypNoBlockingNet.h"
#define MSIZE 24 // standard message size = sizeof(sendData) 

YypNoBlockingNet::YypNoBlockingNet()
{
}

YypNoBlockingNet::~YypNoBlockingNet()
{
}

TechEnum YypNoBlockingNet::getTech()
{
	return mTech;
}

newSoldierStruct YypNoBlockingNet::getNewSoldier()
{
	return mNewSoldier;
}

twoPointStruct YypNoBlockingNet::getPoints()
{
	return mPoints;
}

MyPointStruct YypNoBlockingNet::getOnePoint()
{
	return mOnePoint;
}

whichEnum YypNoBlockingNet::getWhich()
{
	return which;
}

bool YypNoBlockingNet::isLocked()
{
	return lock;
}

bool YypNoBlockingNet::sendTech(TechEnum tech)
{
	int sendData[6];
	sendData[0] = 0;
	sendData[1] = 0;
	sendData[2] = 0;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = (int) tech;
	int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
	if (ret == SOCKET_ERROR)
		return false;
	else
		return true;

}

bool YypNoBlockingNet::sendNewSoldier(newSoldierStruct newSoldier)
{
	int sendData[6];
	sendData[0] = 1;
	sendData[1] = newSoldier.loc.x;
	sendData[2] = newSoldier.loc.y;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = (int) newSoldier.unit;
	int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
	if (ret == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool YypNoBlockingNet::sendTwoPoint(twoPointStruct points)
{
	int sendData[6];
	sendData[0] = 2;
	sendData[1] = points.first.x;
	sendData[2] = points.first.y;
	sendData[3] = points.second.x;
	sendData[4] = points.second.y;
	sendData[5] = 0;
	int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
	if (ret == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool YypNoBlockingNet::sendOnePoint(MyPointStruct onePoint)
{
	int sendData[6];
	sendData[0] = 3;
	sendData[1] = onePoint.x;
	sendData[2] = onePoint.y;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = 0;
	int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
	if (ret == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool YypNoBlockingNet::sendEnd()
{
	int sendData[6];
	sendData[0] = 4;
	sendData[1] = 0;
	sendData[2] = 0;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = 0;
	int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
	if (ret == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool YypNoBlockingNet::sendYouWin()
{
	int sendData[6];
	sendData[0] = 5;
	sendData[1] = 0;
	sendData[2] = 0;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = 0;
	int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
	if (ret == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool YypNoBlockingNet::read()
{
	char revData[255];// need to adjust size
	int ret = recv(sclient, revData, 255, 0);
	if (ret == SOCKET_ERROR)
		return false;
	else
	{
		if (ret > 0 && ret != MSIZE)//invalid data
			return false;
		if (ret == MSIZE)
		{
			int *mess = (int *)revData;
			int flag = mess[0];
			which = (whichEnum)flag;
			int x1 = mess[1];
			int y1 = mess[2];
			int x2 = mess[3];
			int y2 = mess[4];
			int enu = mess[5];
			switch (flag)
			{
			case 0:mTech = (TechEnum)enu; break;
			case 1:mNewSoldier.unit = (UnitEnum)enu; mNewSoldier.loc.x = x1; mNewSoldier.loc.y = y1; break;
			case 2:mPoints.first.x = x1; mPoints.first.y = y1; mPoints.second.x = x2; mPoints.second.y = y2; break;
			case 3:mOnePoint.x = x1; mOnePoint.y = y1; break;
			}
			lock = false;
		}
		return true;
	}
}

bool YypNoBlockingNet::startServer(int pot)
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return false;
	}
	//创建套接字
	slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		//printf("socket error !");
		return false;
	}
	//non blocking mode
	unsigned long nNonBlocking = 1;
	if (ioctlsocket(slisten, FIONBIO, &nNonBlocking) == SOCKET_ERROR)
	{
		//printf("set nonblocking mode error\n");
		closesocket(slisten);
		return false;
	}
	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(pot);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		//printf("bind error !");
		return false;
	}
	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		//printf("listen error !");
		return false;
	}
	//循环接收数据
	return true;
}

bool YypNoBlockingNet::acceptConnect()
{
	nAddrlen = sizeof(remoteAddr);
	sclient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
	if (sclient == SOCKET_ERROR)
		return false;
	else
		return true;
}

bool YypNoBlockingNet::endServer()
{
	if (!closesocket(sclient) && !WSACleanup())
		return true;
	else
		return false;;
}

bool YypNoBlockingNet::startConnect(char *IP, int pot)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return false;
	}
	sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sclient == INVALID_SOCKET)
	{
		//printf("invalid socket !");
		return false;
	}
	//non blocking
	unsigned long ul = 1;
	if (ioctlsocket(sclient, FIONBIO, &ul) == SOCKET_ERROR)
	{
		closesocket(sclient);
		return false;
	}
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(pot);
	serAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	return true;
}
bool YypNoBlockingNet::makeConnect()
{
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}
bool YypNoBlockingNet::deleteConnect()
{
	if (!closesocket(sclient) && !WSACleanup())
		return true;
	else
		return false;
}
