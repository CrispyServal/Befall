#ifndef GAMETIMER_H
#define GAMETIMER_H
#include <string>
#include "cocos2d.h"

using namespace cocos2d;

class GameTimer : public Layer
{
public:
	GameTimer();
	~GameTimer();
	virtual bool init();
	CREATE_FUNC(GameTimer);
	//��������������ʱ��
	void setMaxTime(float maxTime);
	void setEndName(std::string endName);
	//û�㵽��ʱ������������ͼ
	bool blockClick()
	{
		return false;
	}
	//ֻ���жϽ����غ���û�а���
	bool containPoint(Vec2 mousePoint);
	//�صƵĺ�������ʱ������ʾ��������Բ��ɫ�䰵
	void shutDown();
	//��ͷ��ʼ��ʱ��
	void start();
	//�����Ƿ�ʱ�䵽��ʱ�䵽Ϊtrue��GameScene��update�м�飬�����Ļ�ǿ�ƽ����غϡ�
	bool isEnded();
	void refresh(float deltaTime);
	void setTimerColor(int turnF)
	{
		mBgCircle->setTexture(mbgTexture[turnF]);
	}
	float getNowTime(){
		return mNowTime;
	}
	void setFonts(const std::string & fontsFilePath);
private:
	float width;
	float  height;
	//��ʼ�������ʱ��
	float mMaxTime = 60;
	//ʣ��ʱ��
	float mNowTime;
	Label * mEndLabel;
	Sprite * mBgCircle;
	Sprite * mCoverCircle;
	Sprite * mEndTurnButton;
	Texture2D * mbgTexture[2];
	DrawNode * mVitualCircle;
	bool mTimerState;
	float radius;
	std::string mEndName;
	std::string mFonts;
};

#endif // !GAMETIMER_H

/**
	Ч����ͼ��������ѯ������
*/