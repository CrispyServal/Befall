#include "MyStructs.h"

bool operator < (const MyPointStruct & thisOne, const MyPointStruct & another)
{
	if (thisOne.x != another.x)
		return (thisOne.x < another.x);
	else
		return (thisOne.y < another.y);
}
