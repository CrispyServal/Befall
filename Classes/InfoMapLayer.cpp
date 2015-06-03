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
	pointSize = 10;
	mSizeX = 22;
	mSizeY = 22;

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
	mBackground->drawSolidRect(Vec2(-88, -88), Vec2(88, 88), Color4F(0, 0, 0, 0.4));
	mBackground->setPosition(width / 2, height / 2);
	addChild(mBackground, 1);
	//sprite intialize

	//// pic
	//mUnitSprite = Sprite::create("1pixel.png");//100 * 100
	//mUnitSprite->setPosition(width / 2 - 25, height / 2 -10);
	//addChild(mUnitSprite, 2);

	//Unit Info Display
	mUnitInfoLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 16);
	mUnitInfoLabel->setPosition(width / 2, height / 2);
	mUnitInfoLabel->setDimensions(134, 108);//
	mUnitInfoLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
	addChild(mUnitInfoLabel, 2);

	//unitname
	mUnitNameLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 20);
	mUnitNameLabel->setPosition(width / 2, height / 2 + 70);
	addChild(mUnitNameLabel, 3);

	//
	mHpBarBg = DrawNode::create();
	mHpBarBg->drawSolidRect(Vec2(-2, -12), Vec2(102, 12), Color4F(1, 1, 1, 1));
	mHpBarBg->setPosition(width / 2 - 50, height / 2 - 50);
	addChild(mHpBarBg, 4);
	//mhpbar
	mHPBar = DrawNode::create();
	mHPBar->drawSolidRect(Vec2(0, -10), Vec2(100, 10), Color4F(0, 0, 0, 1));
	mHPBar->setPosition(mHpBarBg->getPosition());
	addChild(mHPBar, 5);
	//mnowhp
	mNowHpBar = DrawNode::create();
	mNowHpBar->drawSolidRect(Vec2(0, -10), Vec2(100, 10), Color4F(1, 0, 0, 1));
	mNowHpBar->setPosition(mHpBarBg->getPosition());
	addChild(mNowHpBar, 6);
	//
	mTechNameLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 20);
	mTechNameLabel->setPosition(width / 2, height / 2 + 70);
	addChild(mTechNameLabel, 7);
	//
	mTechIntroLabel = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 16);
	mTechIntroLabel->setPosition(width / 2, height / 2 );	
	mTechIntroLabel->setDimensions(134, 108);//
	mTechIntroLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
	addChild(mTechIntroLabel, 8);
	//
	mTechTurnLeft = Label::createWithSystemFont("", "fonts/STXIHEI.TTF", 20);
	mTechTurnLeft->setPosition(width / 2, height / 2 - 70);
	addChild(mTechTurnLeft, 9);
	//
	mTechNameLabel->setVisible(false);
	mTechIntroLabel->setVisible(false);
	mTechTurnLeft->setVisible(false);
	mUnitNameLabel->setVisible(false);
	mUnitInfoLabel->setVisible(false);
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
	mUnitInfoLabel->setVisible(false);
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
	mNowHpBar->setScale((float)hpNow / (float)hpAll, 1);//nupdata
	mNowHpBar->setVisible(true);
}

void InfoMapLayer::displayUnitProperty(string unitName, int hpNow, int hpAll, string mUnitProperty)
{
	displayUnitInfo(unitName, hpNow, hpAll);
	mUnitInfoLabel->setString(mUnitProperty);
	mUnitInfoLabel->setVisible(true);

}

void InfoMapLayer::displayText(string techName, string techIntroduction, string techTurnLeft)
{
	mUnitNameLabel -> setVisible(false);
	mUnitInfoLabel->setVisible(false);
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
	mUnitInfoLabel->setVisible(false);
}

void InfoMapLayer::setMapSize(int width, int height)
{
	mBackground->clear();
	mSizeX = width;
	mSizeY = height;
	mBackground->drawSolidRect(Vec2(-mSizeX * pointSize / 2.0, -mSizeY * pointSize / 2.0), Vec2(mSizeX * pointSize / 2.0, mSizeY * pointSize / 2.0), Color4F(0, 0, 0, 0.4));
}

void InfoMapLayer::setPointSize(float size)
{
	pointSize = size;
	//mBg
	mBackground->clear();
	mBackground->drawSolidRect(Vec2(-mSizeX * pointSize / 2.0, -mSizeY * pointSize / 2.0), Vec2(mSizeX * pointSize / 2.0, mSizeY * pointSize / 2.0), Color4F(0, 0, 0, 0.4));

	
}