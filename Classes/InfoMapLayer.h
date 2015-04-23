#ifndef _INFOMAP_LAYER_H_
#define _INFOMAP_LAYER_H_

#include "cocos2d.h"
#include <string>
using namespace cocos2d;
using std::string;

class InfoMapLayer : public cocos2d::Layer
{
	float width;
	float height;
	DrawNode * mBackground;
	Label * mUnitNameLabel;
	Sprite * mUnitSprite;
	DrawNode * mHpBarBg;
	DrawNode * mHPBar;
	DrawNode * mNowHpBar;
	Label * mTechNameLabel;
	Label * mTechIntroLabel;
	Label * mTechTurnLeft;//还剩几回合出
public:
	
	InfoMapLayer();
	~InfoMapLayer();

	CREATE_FUNC(InfoMapLayer);
	virtual bool init();

	void displayUnitINfo(string fileName, string unitName, int hpNow, int hpAll);
	void displayTech(string techName, string techIntroduction, int techTurnLeft);
};

#endif
