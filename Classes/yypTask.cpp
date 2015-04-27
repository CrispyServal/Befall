
#include <iostream>
#include "YypNoBlockingNet.h"

using std::cin;
using std::cout;
using std::endl;

int main()
{
	YypNoBlockingNet server;
	YypNoBlockingNet client;
	cout << " choose server or client (0/1)" << endl;
	int netType;
	cin >> netType;
	if (netType == 0)
	{
		cout << "input pots:";
		int pots;
		cin >> pots;
		server.startServer(pots);
		
		while (!server.acceptConnect())
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				//std::cout<<"未收到客户端的连接请求。"<<std::endl;  
				continue;
			}
			else
			{
				//std::cout << "未知错误" << std::endl;
				getchar();
				return -1;
			}
		}
		cout << "接受到一个连接：" << endl;
		while (!server.read())
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				continue;
			else if (err == WSAETIMEDOUT)//超时。  
			{
				cout << "error";
				break;
			}
			else if (err == WSAENETDOWN)//连接断开。  
			{
				cout << "error";
				break;
			}
			else//其他错误。  
			{
				cout << "error";
				break;
			}
		}
		cout << "tech = " << server.tech << endl;

		while (!server.read())
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				continue;
			else if (err == WSAETIMEDOUT)//超时。  
			{
				cout << "error";
				break;
			}
			else if (err == WSAENETDOWN)//连接断开。  
			{
				cout << "error";
				break;
			}
			else//其他错误。  
			{
				cout << "error";
				break;
			}
		}
		cout << "soldier = " << server.newSoldier.unit << endl;
		cout << "x1 = " << server.newSoldier.x1 << endl;
		cout << "y1 = " << server.newSoldier.y1 << endl;

		while (!server.read())
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				continue;
			else if (err == WSAETIMEDOUT)//超时。  
			{
				cout << "error";
				break;
			}
			else if (err == WSAENETDOWN)//连接断开。  
			{
				cout << "error";
				break;
			}
			else//其他错误。  
			{
				cout << "error";
				break;
			}
		}

		cout << "p: x1 = " << server.points.x1 << endl;
		cout << "p: x2 = " << server.points.y1 << endl; 
		cout << "p: y1 = " << server.points.x2 << endl; 
		cout << "p: y2 = " << server.points.y2 << endl; 
		server.endServer();
		system("pause");
	}
	else if (netType == 1)
	{
		cout << "input IP:";
		char* IP = new char[100];
		cin >> IP;
		cout << "input pots:";
		int pots;
		cin >> pots;
		client.makeConnect(IP, pots);
		TechEnum tech = defbase2;
		cout << "tech = " << tech << endl;

		while (!client.sendTech(tech))
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

		newSoldierStruct newSoldier;
		newSoldier.unit = base;
		newSoldier.x1 = 2;
		newSoldier.y1 = 23;
		cout << "soldier = " << newSoldier.unit << endl;
		Sleep(20);
		while (!client.sendNewSoldier(newSoldier))
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

		twoPointStruct points;
		points.x1 = 21;
		points.y1 = 23;
		points.x2 = 54;
		points.y2 = 76;
		Sleep(20);
		while (!client.sedTwoPoind(points))
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

		client.deleteConnect();
		system("pause");
	}
}

