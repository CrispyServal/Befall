#include "UnitFactory.h"

UnitFactory::UnitFactory():
mUnitExistence(false)
{
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
		mUnitExistence = false;
		return true;
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

void UnitFactory::setUnitTime(UnitEnum unit, int time)
{
	mUnitTimeTable[unit] = time;
}

bool UnitFactory::unitExistence()
{
	return mUnitExistence;
}