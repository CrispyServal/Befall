#ifndef TECHTREELAYER_H
#define TECHTREELAYER_H

#include "cocos2d.h" 
#include <map>
#include "MyEnums.h"

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
	//����û����
	/*
	void onTouchBegan(Vec2 mousePoint);
	void onTouchMoved(Vec2 mousePoint);
	void onTouchEnded(Vec2 mousePoint);
	void onMouseMoved(Vec2 mousePoint);
	*/

private:
	//ͼƬ����
	std::map<TechEnum, Sprite *> mTechSpriteMap;
	//�������ϲ��һ�飬�������������
	std::map<TechEnum, Sprite *> mClickAreaMap;
	//��û���������²���
	bool blockClick = true;
	int flag;

};

#endif // !TECHTREELAYER_H
