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
	//必须调用设置最大时间
	void setMaxTime(float maxTime);
	void setEndName(std::string endName);
	//没点到的时候不能阻塞主地图
	bool blockClick()
	{
		return false;
	}
	//只需判断结束回合有没有包含
	bool containPoint(Vec2 mousePoint);
	//关灯的函数：计时环不显示，结束半圆颜色变暗
	void shutDown();
	//从头开始计时。
	void start();
	//返回是否时间到，时间到为true。GameScene的update中检查，结束的话强制结束回合。
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
	//初始化的最大时间
	float mMaxTime = 60;
	//剩余时间
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
	效果见图，详情咨询丁力。
*/