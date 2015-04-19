#ifndef GAMETIMER_H
#define GAMETIMER_H

#include "cocos2d.h"

using namespace cocos2d;

class GameTimer : public Layer
{
public:
	GameTimer();
	~GameTimer();
	virtual bool init();
	//��������������ʱ��
	void setMaxTime(int maxTime);
	CREATE_FUNC(GameTimer);
	//û�㵽��ʱ������������ͼ
	bool blockClick = false;
	//ֻ���жϽ����غ���û�а���
	bool containPoint(Vec2 mousePoint);
	//�صƵĺ�������ʱ������ʾ��������Բ��ɫ�䰵
	void shutDown();
	//��ͷ��ʼ��ʱ��
	void start();
	//�����Ƿ�ʱ�䵽��ʱ�䵽Ϊtrue��GameScene��update�м�飬�����Ļ�ǿ�ƽ����غϡ�
	bool isEnded();
private:
	//��ʼ�������ʱ��
	int mMaxTime = 60;
	//ʣ��ʱ��
	int mNowTime;
	Sprite * mHalfRing;
	Sprite * mBgCircle;
	Sprite * mEndTurnButton;
};

#endif // !GAMETIMER_H

/**
	Ч����ͼ��������ѯ������
*/