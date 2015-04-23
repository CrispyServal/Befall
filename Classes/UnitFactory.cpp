#include "UnitFactory.h"

UnitFactory::UnitFactory()
{
	timeTable[farmer] = 10;
	timeTable[longrangeunit1] = 10;
	timeTable[longrangeunit2] = 10;
	timeTable[longrangeunit3] = 10;
	timeTable[shortrangeunit1] = 10;
	timeTable[shortrangeunit2] = 10;
}
UnitFactory::~UnitFactory()
{

}
void UnitFactory::refresh(int currentProductivity)
{
	if (mUnitExistence)
		mTimer -= currentProductivity;
}
bool UnitFactory::finished()
{
	if (mTimer <= 0 && mUnitExistence)
	{
		return true;
		mUnitExistence = false;
	}
	else
		return false;
}
UnitEnum UnitFactory::getFinishedUnit()
{
	return mBuldingUnit;
}
void UnitFactory::cancelNowUnit()
{
	mUnitExistence = false;
	mTimer = 99;
}
void UnitFactory::addNewUnit(UnitEnum unit)
{
	mUnitExistence = true;
	mBuldingUnit = unit;
	mTimer = timeTable[unit];
}
bool UnitFactory::unitExistence()
{
	return mUnitExistence;
}