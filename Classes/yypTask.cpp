
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
		
		while (true)
		{
			if (!server.acceptConnect())
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
			else
				break;
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
		cout << "tech = " << server.getTech() << endl;

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
		cout << "soldier = " << server.getNewSoldier().unit << endl;
		cout << "x1 = " << server.getNewSoldier().loc.x << endl;
		cout << "y1 = " << server.getNewSoldier().loc.y << endl;

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

		cout << "p: x1 = " << server.getPoints().first.x << endl;
		cout << "p: x2 = " << server.getPoints().first.y << endl; 
		cout << "p: y1 = " << server.getPoints().second.x << endl; 
		cout << "p: y2 = " << server.getPoints().second.y << endl; 
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
		cout << server.getWhich() << endl;
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
		newSoldier.loc.x = 2;
		newSoldier.loc.y = 23;
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
		points.first.x = 21;
		points.first.y = 23;
		points.second.x = 54;
		points.second.y = 76;
		Sleep(20);
		while (!client.sedTwoPoint(points))
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

		Sleep(20);
		while (!client.sedEnd())
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

