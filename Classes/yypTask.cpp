
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
		server.read();
		cout << "tech = " << server.tech << endl;
		server.read();
		cout << "soldier = " << server.newSoldier.unit << endl;
		cout << "x1 = " << server.newSoldier.x1 << endl;
		cout << "y1 = " << server.newSoldier.y1 << endl;
		server.read();
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
		client.sendTech(tech);
		newSoldierStruct newSoldier;
		newSoldier.unit = base;
		newSoldier.x1 = 2;
		newSoldier.y1 = 23;
		cout << "soldier = " << newSoldier.unit << endl;
		client.sendNewSoldier(newSoldier);
		twoPointStruct points;
		points.x1 = 21;
		points.y1 = 23;
		points.x2 = 54;
		points.y2 = 76;
		client.sedTwoPoind(points);
		client.deleteConnect();
		system("pause");
	}
}

