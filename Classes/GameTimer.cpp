#include "GameTimer.h"

GameTimer::GameTimer()
{

}
GameTimer::~GameTimer()
{

}
bool GameTimer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	// find location
	auto director = Director::getInstance();
	width = director->getWinSize().width;
	height = director->getWinSize().height;
	//
	mbgTexture[0] = director->getTextureCache()->addImage("/uiComponent/timer_blue.png");
	mbgTexture[1] = director->getTextureCache()->addImage("/uiComponent/timer_red.png");
	//BgCircle
	//mBgCircle = Sprite::create("/uiComponent/timer.png");
	mBgCircle = Sprite::createWithTexture(mbgTexture[0]);
	//mBgCircle->setScale(0.15, 0.15);	
	mBgCircle->setPosition(width / 2, height / 2);
	addChild(mBgCircle, 1);
	//EndLabel
	mEndLabel = Label::createWithTTF("END", "fonts/STXIHEI.TTF", 30);
	mEndLabel->setPosition(mBgCircle->getPositionX(), mBgCircle->getPositionY()+30);
	mEndLabel->enableGlow(Color4B(255,255,255,155));
	addChild(mEndLabel, 2);
	radius = 90 * mBgCircle->getScaleX() * this->getScaleX();
	mVitualCircle = DrawNode::create();
	mVitualCircle->drawSolidCircle(mBgCircle->getPosition(), radius, CC_DEGREES_TO_RADIANS(90), 50, Color4F(0, 0, 0, 0.5));	
	addChild(mVitualCircle, 3);
	//cover
	mCoverCircle = Sprite::create("/uiComponent/time_locked.png");
	//mCoverCircle->setScale(mBgCircle->getScale());
	mCoverCircle->setPosition(mBgCircle->getPosition());
	addChild(mCoverCircle, 4);
	//visible
	mBgCircle->setVisible(true);
	mEndLabel->setVisible(true);
	mVitualCircle->setVisible(false);
	mCoverCircle->setVisible(false);
	return true;
}
//必须调用设置最大时间
void GameTimer::setMaxTime(float maxTime)// f
{
	mMaxTime = maxTime;
}
void GameTimer::setEndName(std::string endName)
{
	mEndLabel->setString(endName);
}
//只需判断结束回合有没有包含
bool GameTimer::containPoint(Vec2 mousePoint)
{
	radius = 90 * mBgCircle->getScaleX() * this->getScaleX();
	Point wpoint = mBgCircle->getParent()->convertToWorldSpace(mBgCircle->getPosition());
	if ((wpoint.x - mousePoint.x)*(wpoint.x - mousePoint.x) + (wpoint.y - mousePoint.y)*(wpoint.y - mousePoint.y) <= (radius)*(radius))
		return true;
	else
		return false;
}
//关灯的函数：计时环不显示，结束半圆颜色变暗
void GameTimer::shutDown()//f
{
	mCoverCircle->setVisible(true);
	mBgCircle->setRotation(0);
	mTimerState = false;
}
//从头开始计时。
void GameTimer::start()//f
{
	mNowTime = mMaxTime;	
	mBgCircle->setRotation(0);
	mCoverCircle->setVisible(false);
	mTimerState = true;
}
//返回是否时间到，时间到为true。GameScene的update中检查，结束的话强制结束回合。
bool GameTimer::isEnded()// f
{
	if (mNowTime <= 0)
		mTimerState = false;
	if (!mTimerState)
		return true;
	else
		return false;
}

void GameTimer::refresh(float deltaTime)//f ?
{
	mNowTime -= deltaTime;
	if (mNowTime > 0)
		mBgCircle->setRotation((mMaxTime - mNowTime) / mMaxTime * 180);
	else
		mBgCircle->setRotation(180);
}