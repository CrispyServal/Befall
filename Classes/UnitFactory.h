#ifndef UNITFACTORY_H
#define UNITFACTORY_H

#include "MyEnums.h"

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
private:
	//�ȴ��ж���
	UnitEnum mBuldingUnit;
	//�����е�ʣ����
	int mTimer;

};

#endif // !UNITFACTORY_H
