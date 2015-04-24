#include "YypNoBlockingNet.h"
#define MSIZE 24 // standard message size = sizeof(sendData) 

YypNoBlockingNet::YypNoBlockingNet()
{
}
YypNoBlockingNet::~YypNoBlockingNet()
{
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
	while (true)
	{
		int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				Sleep(20);
				continue;
			}
			else
			{
				std::cout << "数据发送失败！" << std::endl;
				return false;
			}
		}
		else
			return true;
	}
}
bool YypNoBlockingNet::sendNewSoldier(newSoldierStruct newSoldier)
{
	int sendData[6];
	sendData[0] = 1;
	sendData[1] = newSoldier.x1;
	sendData[2] = newSoldier.y1;
	sendData[3] = 0;
	sendData[4] = 0;
	sendData[5] = (int) newSoldier.unit;
	while (true)
	{
		int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				Sleep(20);
				continue;
			}
			else
			{
			std::cout << "数据发送失败！" << std::endl;
			return false;
			}
		}
		else
			return true;
	}	
}
bool YypNoBlockingNet::sedTwoPoind(twoPointStruct points)
{
	int sendData[6];
	sendData[0] = 2;
	sendData[1] = points.x1;
	sendData[2] = points.y1;
	sendData[3] = points.x2;
	sendData[4] = points.y2;
	sendData[5] = 0;
	while (true)
	{
		int ret = send(sclient, (char *)&sendData, sizeof(sendData), 0);//??
		if (ret == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				Sleep(20);
				continue;
			}
			else
			{
				std::cout << "数据发送失败！" << std::endl;
				return false;
			}
		}
		else
			return true;
	}
}
bool YypNoBlockingNet::read()
{
	char revData[255];// need to adjust size
	int ret;
	while (true)
	{
		ret = recv(sclient, revData, 255, 0);
		if (ret == SOCKET_ERROR)
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				continue;
			}
			//else if (err == WSAETIMEDOUT)//超时。  
			//{
				//return false;
			//}
			else if (err == WSAENETDOWN)//连接断开。  
			{
				return false;
			}
			else//其他错误。  
				return false;
		}
		else
			break;//接受成功。 
	}
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
		case 0:tech = (TechEnum) enu; break;
		case 1:newSoldier.unit = (UnitEnum)enu; newSoldier.x1 = x1; newSoldier.y1 = y1; break;
		case 2:points.x1 = x1; points.y1 = y1; points.x2 = x2; points.y2 = y2; break;
		}
		lock = false;
	}
	return true;
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
		printf("socket error !");
		return false;
	}
	//non blocking mode
	unsigned long nNonBlocking = 1;
	if (ioctlsocket(slisten, FIONBIO, &nNonBlocking) == SOCKET_ERROR)
	{
		printf("set nonblocking mode error\n");
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
		printf("bind error !");
	}
	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return false;
	}
	//循环接收数据
	nAddrlen = sizeof(remoteAddr);
	while (true)
	{
		sclient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sclient == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				//std::cout<<"未收到客户端的连接请求。"<<std::endl;  
				continue;
			}
			else
			{
				std::cout << "未知错误，接受客户端连接线程退出。" << std::endl;
				getchar();
				return false;
			}
		}
		printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
		return true;
	}
}

bool YypNoBlockingNet::endServer()
{
	if (!closesocket(sclient) && !WSACleanup())
		return true;
	else
		return false;;
}
bool YypNoBlockingNet::makeConnect(char *IP, int pot)
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
		printf("invalid socket !");
		return false;
	}
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(pot);
	serAddr.sin_addr.S_un.S_addr = inet_addr(IP);
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("connect error !");
		closesocket(sclient);
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
