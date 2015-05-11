#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H
#include "MyEnums.h"

#include "cocos2d.h"
#include <set>
#include <vector>
#include <map>

struct MyPointStruct
{
	int x;
	int y;
};

struct InfluenceStruct
{
	std::string type;
	std::string target;
	int value;
};

bool operator < (const MyPointStruct & thisOne, const MyPointStruct & another);

//use this in vector
struct PathNodeStruct
{
	MyPointStruct point;
	int indexParent;
	int stepLeft;
};

/*
bool operator < (const PathNodeStruct & thisOne, const PathNodeStruct & another)
{
	return (thisOne.stepLeft < another.stepLeft);
}
*/

//use it in vector
struct TechTreeNodeStruct
{
	TechEnum techName;
	std::vector<int> indexParents;
	bool isUnclocked;
};

/*
bool operator < (const TechTreeNodeStruct & thisOne, const TechTreeNodeStruct & another)
{
	return (thisOne.techName < another.techName);
}
*/

struct UnitPropertyStruct
{
	int numHitPoint;
	int numDefence;
	int numAttack;
	int numRangeAttack;
	int numRangeMove;
	int numPopulation;
};

struct ResourcesStruct
{
	int numFixedResource;
	int numRandomResource;
	int numProductivity;
	int numResearchLevel;
};

struct Unit
{
	UnitEnum type;
	UnitPropertyStruct property;
	UnitStateEnum state;
	cocos2d::Sprite * sprite;
};

#endif // !MYSTRUCTS_H
