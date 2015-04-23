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
	//��ʣ���� - ��ǰ������
	void refresh(int currentProductivity);
	//���mTimer <= 0���������
	bool finished();
	//���ص�ǰ����ĵ�λ
	UnitEnum getFinishedUnit();
	//ȡ����ǰ����
	void cancelNowUnit();
	//��ʼһ������, ȥ����Ҫ��ʱ��
	void addNewUnit(UnitEnum unit);
	//
	bool unitExistence();
private:
	//�ȴ��ж���
	UnitEnum mBuldingUnit;
	//�����е�ʣ����
	int mTimer;
	bool mUnitExistence;
	map<UnitEnum, int> timeTable;

};

#endif // !UNITFACTORY_H
