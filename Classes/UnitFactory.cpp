#include "UnitFactory.h"

UnitFactory::UnitFactory()
{
	mUnitTimeTable[farmer] = 10;
	mUnitTimeTable[longrangeunit1] = 10;
	mUnitTimeTable[longrangeunit2] = 10;
	mUnitTimeTable[longrangeunit3] = 10;
	mUnitTimeTable[shortrangeunit1] = 10;
	mUnitTimeTable[shortrangeunit2] = 10;
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
	mTimer = mUnitTimeTable[unit];
}
bool UnitFactory::unitExistence()
{
	return mUnitExistence;
}