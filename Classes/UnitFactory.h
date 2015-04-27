#ifndef UNITFACTORY_H
#define UNITFACTORY_H

#include "MyEnums.h"
#include <map>

using std::map;

class UnitFactory
{
public:
	UnitFactory();
	~UnitFactory();
	//methods
	//用剩余量 - 当前生产力
	void refresh(int currentProductivity);
	//如果mTimer <= 0，建造结束
	bool finished();
	//返回当前建造的单位
	UnitEnum getFinishedUnit();
	//取消当前建造
	void cancelNowUnit();
	//开始一个建造, 去查需要的时间
	void addNewUnit(UnitEnum unit);
	//
	bool unitExistence();
private:
	//等待中队列
	UnitEnum mBuldingUnit;
	//建造中的剩余量
	int mTimer;
	bool mUnitExistence;
	map<UnitEnum, int> mUnitTimeTable = { { 
			{ farmer, 10 }, 
			{ longrangeunit1, 10 },
			{ longrangeunit2, 10 },
			{ longrangeunit3, 10 },
			{ shortrangeunit1, 10},
			{ shortrangeunit2, 10},
			} };
};

#endif // !UNITFACTORY_H