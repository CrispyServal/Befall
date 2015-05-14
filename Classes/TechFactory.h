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
	//��ʣ���� - ��ǰ������
	void refresh(int currentProductivity);
	//���mTimer <= 0���������
	bool finished();
	//���ص�ǰ����ĵ�λ
	TechEnum getFinishedTech();
	//get making tech
	TechEnum getMakingTech()
	{
		return mBuldingTech;
	}
	//ȡ����ǰ����
	void cancelNowTech();
	//��ʼһ������, ȥ����Ҫ��ʱ��
	void addNewTech(TechEnum tech);
	//
	bool techExistence();
private:
	//�ȴ��ж���
	TechEnum mBuldingTech;
	//�����е�ʣ����
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
