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
	map<TechEnum, int> mTechTimeTable;
};

#endif // !TECHFACTORY_H
