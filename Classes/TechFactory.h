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
	map<TechEnum, int> mTechTimeTable;
};

#endif // !TECHFACTORY_H
