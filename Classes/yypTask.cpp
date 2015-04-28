
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
					//std::cout<<"δ�յ��ͻ��˵���������"<<std::endl;  
					continue;
				}
				else
				{
					//std::cout << "δ֪����" << std::endl;
					getchar();
					return -1;
				}
			}
			else
				break;
		}
		cout << "���ܵ�һ�����ӣ�" << endl;
		while (!server.read())
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				continue;
			else if (err == WSAETIMEDOUT)//��ʱ��  
			{
				cout << "error";
				break;
			}
			else if (err == WSAENETDOWN)//���ӶϿ���  
			{
				cout << "error";
				break;
			}
			else//��������  
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
			else if (err == WSAETIMEDOUT)//��ʱ��  
			{
				cout << "error";
				break;
			}
			else if (err == WSAENETDOWN)//���ӶϿ���  
			{
				cout << "error";
				break;
			}
			else//��������  
			{
				cout << "error";
				break;
			}
		}
		cout << "soldier = " << server.newSoldier.unit << endl;
		cout << "x1 = " << server.newSoldier.loc.x << endl;
		cout << "y1 = " << server.newSoldier.loc.y << endl;

		while (!server.read())
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
				continue;
			else if (err == WSAETIMEDOUT)//��ʱ��  
			{
				cout << "error";
				break;
			}
			else if (err == WSAENETDOWN)//���ӶϿ���  
			{
				cout << "error";
				break;
			}
			else//��������  
			{
				cout << "error";
				break;
			}
		}

		cout << "p: x1 = " << server.points.first.x << endl;
		cout << "p: x2 = " << server.points.first.y << endl; 
		cout << "p: y1 = " << server.points.second.x << endl; 
		cout << "p: y2 = " << server.points.second.y << endl; 
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
				std::cout << "���ݷ���ʧ�ܣ�" << std::endl;
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
				std::cout << "���ݷ���ʧ�ܣ�" << std::endl;
				return false;
			}
		}

		twoPointStruct points;
		points.first.x = 21;
		points.first.y = 23;
		points.second.x = 54;
		points.second.y = 76;
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
				std::cout << "���ݷ���ʧ�ܣ�" << std::endl;
				return false;
			}
		}

		client.deleteConnect();
		system("pause");
	}
}

