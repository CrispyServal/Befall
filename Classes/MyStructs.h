#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H
#include "MyEnums.h"

#include <set>
#include <map>

struct MyPointStruct
{
	int x;
	int y;
};


/*
bool operator < (const MyPointStruct & thisOne, const MyPointStruct & another)
{
	if (thisOne.x != another.x)
		return (thisOne.x < another.x);
	else
		return (thisOne.y < another.y);
}
*/

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
	std::set<int> indexParents;
	std::set<int> indexChildren;
	bool isUnclocked = false;
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
};

#endif // !MYSTRUCTS_H
