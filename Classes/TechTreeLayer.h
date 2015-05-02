#ifndef TECHTREELAYER_H
#define TECHTREELAYER_H

#include "cocos2d.h" 
#include <map>
#include "json/document.h"
#include "MyEnums.h"
#include <cmath>

using namespace cocos2d;

class TechTreeLayer : public Layer
{
public:
	TechTreeLayer();
	~TechTreeLayer();
	virtual bool init();
	CREATE_FUNC(TechTreeLayer);
	//method
	//�Լ�����������˹��λ��
	bool containPoint(Vec2 mousePoint);
	//���ع����ͣ�ĿƼ�������������ʱ����
	TechEnum getTechContainingPoint(Vec2 mousePoint);
	//�л��Ƽ���ʾ״̬
	void setTechState(TechEnum tech, TechStateEnum techState);
	//��û���������²���
	bool blockClick()
	{
		return true;
	}


private:
	float width;
	float height;
	float sideLength;
	float spaceLength;
	float directLength;
	float triLength;
	float halfLength;
	//ͼƬ����
	std::map<TechEnum, Sprite *> mTechSpriteMap;
	//�������ϲ��һ�飬�������������
	std::map<TechEnum, Sprite *> mClickAreaMap;
	std::map<TechEnum, TechStateEnum> mTechStateMap;
	Dictionary * dictionary;
};

#endif // !TECHTREELAYER_H
