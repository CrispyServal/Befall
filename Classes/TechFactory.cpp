#include "TechFactory.h"
#include <map>

using std::map;

TechFactory::TechFactory()
{
};
TechFactory::~TechFactory()
{
};
void TechFactory::refresh(int currentProductivity)
{
	if (mTechExistence)
		mTimer -= currentProductivity;
};
bool TechFactory::finished()
{
	if (mTimer <= 0 && mTechExistence)
	{
		return true;
		mTechExistence = false;
	}
	else
		return false;
}
TechEnum TechFactory::getFinishedTech()
{
	return mBuldingTech;
}
void TechFactory::cancelNowTech()
{
	mTechExistence = false;
	mTimer = 99;
}
void TechFactory::addNewTech(TechEnum tech)
{
	mTechExistence = true;
	mBuldingTech = tech;
	mTimer = mTechTimeTable[tech];
}
bool TechFactory::techExistence()
{
	return mTechExistence;
}