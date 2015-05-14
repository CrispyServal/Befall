#include "InfoMapLayer.h"
#include <string>

InfoMapLayer::InfoMapLayer()
{
}


InfoMapLayer::~InfoMapLayer()
{
}

bool InfoMapLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	// find location
	auto director = Director::getInstance();
	width = director->getWinSize().width;
	height = director->getWinSize().height;
	//background sprite
	mBackground = DrawNode::create();
	mBackground->drawSolidRect(Vec2(-100, -120), Vec2(100, 120), Color4F(0, 0, 0, 0.4));
	mBackground->setPosition(width / 2, height / 2);
	addChild(mBackground, 1);
	//sprite intialize

	//// pic
	//mUnitSprite = Sprite::create("1pixel.png");//100 * 100
	//mUnitSprite->setPosition(width / 2 - 25, height / 2 -10);
	//addChild(mUnitSprite, 2);

	//unitname
	mUnitNameLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 20);
	mUnitNameLabel->setPosition(width / 2, height / 2 + 80);
	addChild(mUnitNameLabel, 3);
	//
	mHpBarBg = DrawNode::create();
	mHpBarBg->drawSolidRect(Vec2(-12, -2), Vec2(12, 102), Color4F(1, 1, 1, 1));
	mHpBarBg->setPosition(width / 2 + 65, height / 2 - 60);
	addChild(mHpBarBg, 4);
	//mhpbar
	mHPBar = DrawNode::create();
	mHPBar->drawSolidRect(Vec2(-10, 0), Vec2(10, 100), Color4F(0, 0, 0, 1));
	mHPBar->setPosition(mHpBarBg->getPosition());
	addChild(mHPBar, 5);
	//mnowhp
	mNowHpBar = DrawNode::create();
	mNowHpBar->drawSolidRect(Vec2(-10, 0), Vec2(10, 100), Color4F(1, 0, 0, 1));
	mNowHpBar->setPosition(mHpBarBg->getPosition());
	addChild(mNowHpBar, 6);
	//
	mTechNameLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 20);
	mTechNameLabel->setPosition(width / 2, height / 2 + 80);
	addChild(mTechNameLabel, 7);
	//
	mTechIntroLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 12);
	mTechIntroLabel->setPosition(width / 2, height / 2 - 20);	
	mTechIntroLabel->setDimensions(134, 100);//
	mTechIntroLabel->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
	addChild(mTechIntroLabel, 8);
	//
	mTechTurnLeft = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 20);
	mTechTurnLeft->setPosition(width / 2, height / 2 - 95);
	addChild(mTechTurnLeft, 9);
	//
	mTechNameLabel->setVisible(false);
	mTechIntroLabel->setVisible(false);
	mTechTurnLeft->setVisible(false);
	mUnitNameLabel->setVisible(false);
//	mUnitSprite->setVisible(false);
	mHpBarBg->setVisible(false);
	mHPBar->setVisible(false);
	mNowHpBar->setVisible(false);
	return true;
}

void InfoMapLayer::displayUnitInfo(string unitName, int hpNow, int hpAll)
{
	mTechNameLabel->setVisible(false); 
	mTechIntroLabel->setVisible(false);
	mTechTurnLeft->setVisible(false);
	// pic
	//CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage(fileName);
	//mUnitSprite->setTexture(texture);
	//mUnitSprite->setVisible(true);
	//unitname
	mUnitNameLabel->setString(unitName);
	mUnitNameLabel->setVisible(true);
	//
	mHpBarBg->setVisible(true);
	//mhpbar
	mHPBar->setVisible(true);
	//mnowhp
	mNowHpBar->setScale(1, (float)hpNow / (float)hpAll);//nupdata
	mNowHpBar->setVisible(true);
}

void InfoMapLayer::displayText(string techName, string techIntroduction, string techTurnLeft)
{
	mUnitNameLabel -> setVisible(false);
//	mUnitSprite -> setVisible(false);
	mHpBarBg->setVisible(false);
	mHPBar -> setVisible(false);
	mNowHpBar -> setVisible(false);
	//
	mTechNameLabel->setString(techName);
	mTechNameLabel->setVisible(true);
	//
	mTechIntroLabel->setString(techIntroduction);
	mTechIntroLabel->setVisible(true);
	//
	mTechTurnLeft->setString(techTurnLeft);
	mTechTurnLeft->setVisible(true);
}

void InfoMapLayer::clearAllInfo()
{
	mTechNameLabel->setVisible(false);
	mTechIntroLabel->setVisible(false);
	mTechTurnLeft->setVisible(false);
	mUnitNameLabel->setVisible(false);
//	mUnitSprite->setVisible(false);
	mHpBarBg->setVisible(false);
	mHPBar->setVisible(false);
	mNowHpBar->setVisible(false);
}
