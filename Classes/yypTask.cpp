#include "MyStructs.h"
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
			{
				cout << "non blocking\n";
				continue;
			}
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
		
		TechEnum tech = defbase1;
		Sleep(500);
		cout << "tech2 =" << tech << endl;
		while (!server.sendTech(tech))
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
				break;
			}
		}

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
		client.startConnect(IP, pots);
		cout << "start connect correct\n";
		while (true)
		{
			if (!client.makeConnect())
			{
				int r = WSAGetLastError();
				if (r == WSAEWOULDBLOCK || r == WSAEINVAL)
				{
					//std::cout<<"未收到客户端的连接请求。"<<std::endl;  
					continue;
				}
				else if (r == WSAEISCONN)//套接字原来已经连接！！  
				{
					break;
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
		cout << "make connect correct\n";
		TechEnum tech = defbase2;
		cout << "tech = " << tech << endl;
		Sleep(500);
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
				system("pause");
				break;
			}	
		}

		while (!client.read())
		{
			auto err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				cout << "non blocking\n";
				continue;
			}
			else if (err == WSAETIMEDOUT)//超时。  
			{
				cout << "error\n";
				break;
			}
			else if (err == WSAENETDOWN)//连接断开。  
			{
				cout << "error\n";
				break;
			}
			else//其他错误。  
			{
				cout << "error\n";
				break;
			}
		}
		cout << "tech = " << client.getTech() << endl;

		client.deleteConnect();
		system("pause");
	}
}

