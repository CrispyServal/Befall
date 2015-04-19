#ifndef TECHFACTORY_H
#define TECHFACTORY_H

#include "MyEnums.h"

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
private:
	//�ȴ��ж���
	TechEnum mBuldingTech;
	//�����е�ʣ����
	int mTimer;

};

#endif // !TECHFACTORY_H
