#include "MyStructs.h"

bool operator < (const MyPointStruct & thisOne, const MyPointStruct & another)
{
	if (thisOne.x != another.x)
		return (thisOne.x < another.x);
	else
		return (thisOne.y < another.y);
}
bool operator == (const MyPointStruct & thisOne, const MyPointStruct & another)
{
	return ((thisOne.x == another.x) && (thisOne.y == another.y));
}
bool operator != (const MyPointStruct & thisOne, const MyPointStruct & another)
{
	return !(thisOne == another);
}

bool operator == (const PathNodeStruct & thisOne, const PathNodeStruct & another)
{
	return ( (thisOne.point==another.point) && (thisOne.indexParent==another.indexParent) && (thisOne.stepLeft==another.stepLeft) );
}
bool operator != (const PathNodeStruct & thisOne, const PathNodeStruct & another)
{
	return !(thisOne==another);
}
UnitPropertyStruct operator + (const UnitPropertyStruct & thisOne, const UnitPropertyStruct & another)
{
	return UnitPropertyStruct{
		thisOne.numHitPoint + another.numHitPoint,
		thisOne.numDefence + another.numDefence,
		thisOne.numAttack  + another.numAttack,
		thisOne.numRangeAttack  + another.numRangeAttack,
		thisOne.numRangeMove + another.numRangeMove,
		thisOne.numPopulation  + another.numPopulation
	};
}
