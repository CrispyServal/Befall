#ifndef TECHFACTORY_H
#define TECHFACTORY_H

#include "MyEnums.h"
#include <map>

using std::map;

class TechFactory
{
public:
	TechFactory();
	~TechFactory();
	//methods
	//用剩余量 - 当前生产力
	void refresh(int currentProductivity);
	//如果mTimer <= 0，建造结束
	bool finished();
	//返回当前建造的单位
	TechEnum getFinishedTech();
	//get making tech
	TechEnum getMakingTech()
	{
		return mBuldingTech;
	}
	//取消当前建造
	void cancelNowTech();
	//开始一个建造, 去查需要的时间
	void addNewTech(TechEnum tech);
	//
	bool techExistence();
private:
	//等待中队列
	TechEnum mBuldingTech;
	//建造中的剩余量
	int mTimer;
	bool mTechExistence;
	map<TechEnum, int> mTechTimeTable = { {
			{ techroot, 10 },
			{ techtree1, 10 },
			{ techtree2, 10 },
			{ techtree3, 10 },
			{ unlocklongrangeunit1, 10 },
			{ unlocklongrangeunit2, 10 },
			{ unlocklongrangeunit3, 10 },
			{ unlockshortrangeunit1, 10 },
			{ unlockshortrangeunit2, 10 },
			{ productivity1, 10 },
			{ productivity2, 10 },
			{ productivity3, 10 },
			{ resourcefixed1, 10 },
			{ resourcefixed2, 10 },
			{ resourcefixed3, 10 },
			{ resourcerandom1, 10 },
			{ resourcerandom2, 10 },
			{ resourcerandom3, 10 },
			{ hplongrange1, 10 },
			{ hplongrange2, 10 },
			{ arglongrange1, 10 },
			{ arglongrange2, 10 },
			{ atklongrange1, 10 },
			{ atklongrange2, 10 },
			{ atkshortrange1, 10 },
			{ atkshortrange2, 10 },
			{ defshortrange1, 10 },
			{ defshortrange2, 10 },
			{ mrgshortrange1, 10 },
			{ mrgshortrange2, 10 },
			{ defbase1, 10 },
			{ defbase2, 10 }
			} };
};

#endif // !TECHFACTORY_H
