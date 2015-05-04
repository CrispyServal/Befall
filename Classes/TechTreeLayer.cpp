#include "TechTreeLayer.h"

using namespace cocos2d;

TechTreeLayer::TechTreeLayer()
{
}
TechTreeLayer::~TechTreeLayer()
{

}
bool TechTreeLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	// find location
	auto director = Director::getInstance();
	width = director->getWinSize().width;
	height = director->getWinSize().height;
	//bg
	mBg = Sprite::create("/uiComponent/tech_tree_bg.png");
	mBg->setOpacity(125);
	mBg->setPosition(width / 2, height / 2);
	addChild(mBg);
	//click area
	for (int i = 0; i < 32; i++)
	{
		mClickAreaMap[(TechEnum)i] = Sprite::create("/uiComponent/techcell_opaque_100px.png");	
		mClickAreaMap[(TechEnum)i]->setScale(0.45, 0.45);
		addChild(mClickAreaMap[(TechEnum)i]);	
	}
	sideLength = 100 * mClickAreaMap[(TechEnum)0]->getScaleX();
	spaceLength = 10;
	directLength = sqrt(3) * sideLength + spaceLength;
	halfLength = 0.5 * directLength;
	triLength = sqrt(3) * halfLength;
	mClickAreaMap[(TechEnum)0]->setPosition(width / 2, height / 2);
	mClickAreaMap[(TechEnum)1]->setPosition(mClickAreaMap[(TechEnum)0]->getPositionX(), mClickAreaMap[(TechEnum)0]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)2]->setPosition(mClickAreaMap[(TechEnum)1]->getPositionX(), mClickAreaMap[(TechEnum)1]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)3]->setPosition(mClickAreaMap[(TechEnum)2]->getPositionX(), mClickAreaMap[(TechEnum)2]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)4]->setPosition(mClickAreaMap[(TechEnum)0]->getPositionX() + triLength, mClickAreaMap[(TechEnum)0]->getPositionY() - halfLength);
	mClickAreaMap[(TechEnum)5]->setPosition(mClickAreaMap[(TechEnum)4]->getPositionX(), mClickAreaMap[(TechEnum)4]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)6]->setPosition(mClickAreaMap[(TechEnum)0]->getPositionX() - triLength, mClickAreaMap[(TechEnum)0]->getPositionY() + halfLength);
	mClickAreaMap[(TechEnum)7]->setPosition(mClickAreaMap[(TechEnum)6]->getPositionX(), mClickAreaMap[(TechEnum)6]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)9]->setPosition(mClickAreaMap[(TechEnum)0]->getPositionX(), mClickAreaMap[(TechEnum)0]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)10]->setPosition(mClickAreaMap[(TechEnum)9]->getPositionX(), mClickAreaMap[(TechEnum)9]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)11]->setPosition(mClickAreaMap[(TechEnum)10]->getPositionX(), mClickAreaMap[(TechEnum)10]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)12]->setPosition(mClickAreaMap[(TechEnum)0]->getPositionX() + triLength, mClickAreaMap[(TechEnum)0]->getPositionY() + halfLength);
	mClickAreaMap[(TechEnum)13]->setPosition(mClickAreaMap[(TechEnum)12]->getPositionX(), mClickAreaMap[(TechEnum)12]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)14]->setPosition(mClickAreaMap[(TechEnum)13]->getPositionX() + triLength, mClickAreaMap[(TechEnum)13]->getPositionY() + halfLength);
	mClickAreaMap[(TechEnum)15]->setPosition(mClickAreaMap[(TechEnum)0]->getPositionX() - triLength, mClickAreaMap[(TechEnum)0]->getPositionY() - halfLength);
	mClickAreaMap[(TechEnum)16]->setPosition(mClickAreaMap[(TechEnum)15]->getPositionX(), mClickAreaMap[(TechEnum)15]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)17]->setPosition(mClickAreaMap[(TechEnum)16]->getPositionX() - triLength, mClickAreaMap[(TechEnum)16]->getPositionY() - halfLength);
	mClickAreaMap[(TechEnum)18]->setPosition(mClickAreaMap[(TechEnum)4]->getPositionX() + triLength, mClickAreaMap[(TechEnum)4]->getPositionY() + halfLength);
	mClickAreaMap[(TechEnum)19]->setPosition(mClickAreaMap[(TechEnum)18]->getPositionX() + triLength, mClickAreaMap[(TechEnum)18]->getPositionY() + halfLength);
	mClickAreaMap[(TechEnum)20]->setPosition(mClickAreaMap[(TechEnum)18]->getPositionX(), mClickAreaMap[(TechEnum)18]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)8]->setPosition(mClickAreaMap[(TechEnum)19]->getPositionX(), mClickAreaMap[(TechEnum)19]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)21]->setPosition(mClickAreaMap[(TechEnum)8]->getPositionX(), mClickAreaMap[(TechEnum)8]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)22]->setPosition(mClickAreaMap[(TechEnum)20]->getPositionX(), mClickAreaMap[(TechEnum)20]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)23]->setPosition(mClickAreaMap[(TechEnum)5]->getPositionX(), mClickAreaMap[(TechEnum)5]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)24]->setPosition(mClickAreaMap[(TechEnum)7]->getPositionX() - triLength, mClickAreaMap[(TechEnum)7]->getPositionY() + halfLength);
	mClickAreaMap[(TechEnum)25]->setPosition(mClickAreaMap[(TechEnum)7]->getPositionX(), mClickAreaMap[(TechEnum)7]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)28]->setPosition(mClickAreaMap[(TechEnum)24]->getPositionX(), mClickAreaMap[(TechEnum)24]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)26]->setPosition(mClickAreaMap[(TechEnum)28]->getPositionX(), mClickAreaMap[(TechEnum)28]->getPositionY() - directLength);
	mClickAreaMap[(TechEnum)27]->setPosition(mClickAreaMap[(TechEnum)26]->getPositionX() - triLength, mClickAreaMap[(TechEnum)26]->getPositionY() - halfLength);
	mClickAreaMap[(TechEnum)30]->setPosition(mClickAreaMap[(TechEnum)27]->getPositionX(), mClickAreaMap[(TechEnum)27]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)29]->setPosition(mClickAreaMap[(TechEnum)30]->getPositionX(), mClickAreaMap[(TechEnum)30]->getPositionY() + directLength);
	mClickAreaMap[(TechEnum)31]->setPosition(mClickAreaMap[(TechEnum)27]->getPositionX() - triLength, mClickAreaMap[(TechEnum)27]->getPositionY() + halfLength);
	
	std::string imageName[32];
	//get json file
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("/dictionary/techtreedisplay.json");
	CCLOG("%s", jsonFile.c_str());
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string((const char *)loadStr, size);
	//json
	rapidjson::Document document;
	document.Parse<0>(jsonStr.c_str());
	if (document.HasParseError())
	{
		CCLOG("json parsing error!");
	}
	if (!document.IsObject())
	{
		CCLOG("not object");
	}
	rapidjson::Value & techs = document["techtree"];
	if (techs.IsArray())
	{
		for (int i = 0; i < techs.Size(); i++)
		{
			imageName[i] = "/techTreeIcon/";
			imageName[i] += techs[i]["imagename"].GetString();
			CCLOG("this:  %s", imageName[i]);
		}
	}
	for (int i = 0; i < 32; i++)
	{
		mTechSpriteMap[(TechEnum)i] = Sprite::create(imageName[i]);
		mTechSpriteMap[(TechEnum)i]->setScale(0.1, 0.1);
		mTechSpriteMap[(TechEnum)i]->setPosition(mClickAreaMap[(TechEnum)i]->getPosition());
		addChild(mTechSpriteMap[(TechEnum)i]);
		mTechSpriteMap[(TechEnum)i]->setVisible(!(bool)i);
		mClickAreaMap[(TechEnum)i]->setVisible(!(bool)i);
	}
	return true;
}
//method
//自己的区域包含了光标位置
bool TechTreeLayer::containPoint(Vec2 mousePoint)
{
	for (int i = 0; i < 32; i++)
	{
		Point wpoint = mClickAreaMap[(TechEnum)i]->getParent()->convertToWorldSpace(mClickAreaMap[(TechEnum)i]->getPosition());	
		sideLength = 100 * mClickAreaMap[(TechEnum)0]->getScaleX() * this->getScaleX();
		float deltaX = mousePoint.x - wpoint.x;
		float deltaY = mousePoint.y - wpoint.y;
		if ((abs(deltaY) <= sqrt(3) * sideLength / 2.0) && (abs(deltaY / sqrt(3) + deltaX) <= sideLength) && (abs(deltaY / sqrt(3) - deltaX) <= sideLength))
			return true;
	}
	return false;
}
//返回光标悬停的科技项。仅当包含光标时调用
TechEnum TechTreeLayer::getTechContainingPoint(Vec2 mousePoint)
{
	for (int i = 0; i < 32; i++)
	{
		Point wpoint = mClickAreaMap[(TechEnum)i]->getParent()->convertToWorldSpace(mClickAreaMap[(TechEnum)i]->getPosition());
		sideLength = 100 * mClickAreaMap[(TechEnum)0]->getScaleX() * this->getScaleX();
		float deltaX = mousePoint.x - wpoint.x;
		float deltaY = mousePoint.y - wpoint.y;
		if ((abs(deltaY) <= sqrt(3) * sideLength / 2.0)&&(abs(deltaY/sqrt(3)+deltaX)<=sideLength)&&(abs(deltaY/sqrt(3)-deltaX)<=sideLength))
			return (TechEnum)i;
	}
	return (TechEnum)0;
}
//切换科技显示状态
void TechTreeLayer::setTechState(TechEnum tech, TechStateEnum techState)
{
	mTechStateMap[tech] = techState;
	if (mTechStateMap[tech] == unavailable)
	{
		mTechSpriteMap[tech]->setVisible(false);
		mClickAreaMap[tech]->setVisible(false);
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("/uiComponent/techcell_transparent.png");
		mClickAreaMap[tech]->setTexture(texture);
	}
	else if (mTechStateMap[tech] == available)
	{
		mTechSpriteMap[tech]->setVisible(true);
		mClickAreaMap[tech]->setVisible(true);
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("/uiComponent/techcell_transparent_100px.png");
		mClickAreaMap[tech]->setTexture(texture);
	}
	else if (mTechStateMap[tech] == unlocking)
	{
		mTechSpriteMap[tech]->setVisible(true);
		mClickAreaMap[tech]->setVisible(true);
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("/uiComponent/techcell_halftransparent_100px.png");
		mClickAreaMap[tech]->setTexture(texture);
	}
	else if (mTechStateMap[tech] == unlocked)
	{
		mTechSpriteMap[tech]->setVisible(true);
		mClickAreaMap[tech]->setVisible(true);
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("/uiComponent/techcell_opaque_100px.png");
		mClickAreaMap[tech]->setTexture(texture);
	}
}


