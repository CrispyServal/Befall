#include "TechFactory.h"
#include <map>

using std::map;

TechFactory::TechFactory()
{
		mTechTimeTable[techroot] = 10;
		mTechTimeTable[techtree1] = 10;
		mTechTimeTable[techtree2] = 10;
		mTechTimeTable[techtree3] = 10;
		mTechTimeTable[unlocklongrangeunit1] = 10;
		mTechTimeTable[unlocklongrangeunit2] = 10;
		mTechTimeTable[unlockshortrangeunit1] = 10;
		mTechTimeTable[unlockshortrangeunit2] = 10;
		mTechTimeTable[unlocklongrangeunit3] = 10;
		mTechTimeTable[productivity1] = 10;
		mTechTimeTable[productivity2] = 10;
		mTechTimeTable[productivity3] = 10;
		mTechTimeTable[resourcefixed1] = 10;
		mTechTimeTable[resourcefixed2] = 10;
		mTechTimeTable[resourcefixed3] = 10;
		mTechTimeTable[resourcerandom1] = 10;
		mTechTimeTable[resourcerandom2] = 10;
		mTechTimeTable[resourcerandom3] = 10;
		mTechTimeTable[hplongrange1] = 10;
		mTechTimeTable[hplongrange2] = 10;
		mTechTimeTable[arglongrange1] = 10;
		mTechTimeTable[arglongrange2] = 10;
		mTechTimeTable[atklongrange1] = 10;
		mTechTimeTable[atklongrange2] = 10;
		mTechTimeTable[atkshortrange1] = 10;
		mTechTimeTable[atkshortrange2] = 10;
		mTechTimeTable[defshortrange1] = 10;
		mTechTimeTable[defshortrange2] = 10;
		mTechTimeTable[mrgshortrange1] = 10;
		mTechTimeTable[mrgshortrange2] = 10;
		mTechTimeTable[defbase1] = 10;
		mTechTimeTable[defbase2] = 10;
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