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
	int mSizeX;
	int mSizeY;
	float pointSize;
	Color4F mBgColor;
	std::string mFonts;
	DrawNode * mBackground;
	Label * mUnitNameLabel;
	Label * mUnitInfoLabel;
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

	void setFonts(const std::string & fontsFilePath);
	void setPointSize(float size);
	void setMapSize(int width, int height);
	//DisplayUnitInfo in narrative format and HP info
	void displayUnitInfo(string unitName, int hpNow, int hpAll);
	//DisplayText in narrative format
	void displayText(string techName, string techIntroduction, string techTurnLeft);
	//DisplayUnitProperty unitName, hpBar, ATK ,DEF ,ATKRANGE
	void displayUnitProperty(string unitName, int hpNow, int hpAll, string mUnitProperty);
	//Clear Info Display
	void clearAllInfo();
	void setBgColor(Color4F bgColor);
};

#endif
