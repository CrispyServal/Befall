#include "UnitCampLayer.h"
#include <iostream>

using UnitCamp::ItemInCampStruct;
UnitCampLayer::UnitCampLayer()
{
	mUnitsList.push_back(UnitEnum::farmer);
	mUnitsList.push_back(UnitEnum::shortrangeunit1);
	mUnitsList.push_back(UnitEnum::shortrangeunit2);
	mUnitsList.push_back(UnitEnum::longrangeunit1);
	mUnitsList.push_back(UnitEnum::longrangeunit2);
	mUnitsList.push_back(UnitEnum::longrangeunit3);
}

UnitCampLayer::~UnitCampLayer()
{
}

std::string UnitCampLayer::getDicValue(char * str)
{
	 CCLOG("getdicValue: result: %s",mDictionary->valueForKey(str)->getCString());
	 return std::string{mDictionary->valueForKey(str)->getCString()};
}

bool UnitCampLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//read a json to get units' name and introduction
    mDictionary = Dictionary::createWithContentsOfFile(std::string{ "dictionary/" +UserDefault::getInstance()->getStringForKey("language") + ".xml" }.c_str());
	mDictionary->retain();
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("dictionary/unitdisplay.json");
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string( (const char *)loadStr, size );

	rapidjson::Document jDocument;
	jDocument.Parse<0>(jsonStr.c_str());
	if (jDocument.HasParseError())
	{
		CCLOG("unitDisplay.json parse error!");
	}
	if (!jDocument.IsObject())
	{
		CCLOG("error : not object!");
	}
	rapidjson::Value & units = jDocument["units"];
	if (!units.IsArray())
	{
		CCLOG("error : not array");
	}
	//bg
	mBg = Sprite::create("uiComponent/unit_factory_bg.png");
	mBg->setPosition(
		Director::getInstance()->getWinSize().width / 2,
		Director::getInstance()->getWinSize().height / 2
		);
	mBg->setOpacity(125);
	addChild(mBg, -2);
	//init items
	//!!: num of items < 10
	for (int i = 0; i < mUnitsList.size(); ++i)
	{
		CCLOG("sprite: %s", std::string{ "uiComponent/unit" + std::string{ (char)('0' + i) } +std::string{ ".png" } }.c_str());
		CCLOG("texture: %s", std::string{ "uiComponent/unit_image_" + std::string{ (char)('0' + i) } +std::string{ ".png" } }.c_str());
		auto tmpSprite = Sprite::create(std::string{ "uiComponent/unit" + std::string{ (char)('0' + i) } +std::string{ ".png" } });
		auto tmpTexture = Director::getInstance()->getTextureCache()->addImage(std::string{ "uiComponent/unit_image_" + std::string{ (char)('0' + i) } +std::string{ ".png" } });
		mItemsList.push_back(
			ItemInCampStruct{
			mUnitsList[i],
			tmpSprite,
			false,
			//true,
			tmpTexture,
			units[i]["name"].GetString(),
			units[i]["introduction"].GetString(),
			ResourcesStruct{ 0, 0, 0, 0 },
			UnitPropertyStruct{0,0,0,0,0,0}
		}
		);
		mUnitUnlockMap[mUnitsList[i]] = false;
	}
	//mCloseTexture = Director::getInstance()->getTextureCache()->addImage(std::string{ "uiComponent/unit_image_off.png" });
	CCLOG("%d", mItemsList.size());
	
	//big image
	mUnitImage = Sprite::createWithTexture(mItemsList[0].texture);
	mUnitImage->setScale(0.8);
	mUnitImage->setPosition(
		(mUnitImage->getBoundingBox().getMaxX() - mUnitImage->getBoundingBox().getMinX()) / 2 - 50,
		Director::getInstance()->getWinSize().height / 2
		);
	mUnitImage->setOpacity(0);
	addChild(mUnitImage, 0);
	//unitsNode
	auto unitsNode = Node::create();
	//units
	const float unitIn = 30;
	for (int i = 0; i < mItemsList.size(); ++i)
	{
		mItemsList[i].sprite->setColor(Color3B(100, 100, 100));
		mItemsList[i].sprite->setPosition(
			i * (mItemsList[i].sprite->getContentSize().width + unitIn),
			0
			);
		unitsNode->addChild(mItemsList[i].sprite);
	}
	unitsNode->setPosition(
		Director::getInstance()->getWinSize().width / 2 - (mItemsList.size() - 1) / 2.0 * (mItemsList[0].sprite->getContentSize().width + unitIn),
		//mUnitImage->getBoundingBox().getMinY() - mItemsList[0].sprite->getContentSize().height - unitIn 
		130
		);
	addChild(unitsNode, 0);
	//words
	mUnitName = Label::createWithTTF("no name", "fonts/STXIHEI.TTF", 40);
	mUnitName->setColor(Color3B( 255, 255, 255 ));
	mUnitName->setPosition
	(
		Vec2
		(
		Director::getInstance()->getWinSize().width * 3 / 4,
		mUnitImage->getBoundingBox().getMaxY() - mUnitName->getContentSize().height / 2
		)
	);
	mUnitName->setOpacity(0);
	addChild(mUnitName, 0);

	mUnitIntroduction = Label::createWithTTF("no name", "fonts/STXIHEI.TTF", 20);
	mUnitIntroduction->setColor(Color3B( 255, 255, 255 ));
	mUnitIntroduction->setMaxLineWidth(Director::getInstance()->getWinSize().width / 2 - 100);
	mUnitIntroduction->setPosition
	(
		Vec2
		(
		Director::getInstance()->getWinSize().width * 3 / 4,
		mUnitName->getBoundingBox().getMinY() - 3 * mUnitIntroduction->getContentSize().height
		)
	);
	mUnitIntroduction->setOpacity(0);
	addChild(mUnitIntroduction, 0);
	//re and pro
	mUnitResourcesAndProperty = Label::createWithTTF("no name", "fonts/STXIHEI.TTF", 20);
	mUnitResourcesAndProperty->setColor(Color3B( 255, 255, 255 ));
	mUnitResourcesAndProperty->setMaxLineWidth(Director::getInstance()->getWinSize().width / 2 - 100);
	mUnitResourcesAndProperty->setPosition
	(
		Vec2
		(
		Director::getInstance()->getWinSize().width * 3 / 4,
		240
		//mUnitIntroduction->getBoundingBox().getMinY() - 3 * mUnitResourcesAndProperty->getContentSize().height
		)
	);
	mUnitResourcesAndProperty->setOpacity(0);
	addChild(mUnitResourcesAndProperty, 0);
	return true;
}

void UnitCampLayer::setUnitResourceAndProperty(UnitEnum unit, const ResourcesStruct & unitResources, const UnitPropertyStruct & unitProperty)
{
	for (auto & item : mItemsList)
	{
		if (item.unit == unit)
		{
			item.resources = unitResources;
			item.property = unitProperty;
		}
	}
}

void UnitCampLayer::onMouseMoved(Vec2 mousePoint)
{
	CCLOG("called!!");
	for (auto item : mItemsList)
	{
		Point point = Point(item.sprite->boundingBox().getMinX(), item.sprite->boundingBox().getMinY());
		Point wPoint = item.sprite->getParent()->convertToWorldSpace(point);
		float width = item.sprite->boundingBox().getMaxX() - item.sprite->boundingBox().getMinX();
		float height = item.sprite->boundingBox().getMaxY() - item.sprite->boundingBox().getMinY();
		auto rect = Rect(wPoint.x, wPoint.y, width, height);
		//auto rect = Rect(point.x, point.y, width, height);
		//debug
		//auto dR = DrawNode::create();
		//dR->drawRect(wPoint, Vec2(wPoint.x + width, wPoint.y + height), Color4F(0, 1, 0, 1));
		//dR->drawRect(Vec2(rect.getMinX(),rect.getMinY()), Vec2(rect.getMaxX(),rect.getMaxY()) ,Color4F(1, 0, 0, 1));
		//dR->drawRect(point, Vec2(point.x + width, point.y + height), Color4F(1, 0, 0, 1));
		//addChild(dR,2);
		//dR->cleanup();
		if (rect.containsPoint(mousePoint))
		{
			if (item.unlocked)
			//if (mUnitUnlockMap[item.unit])
			{
				mUnitImage->setOpacity(255);
				mUnitName->setOpacity(255);
				mUnitIntroduction->setOpacity(255);
				mUnitResourcesAndProperty->setOpacity(255);
				CCLOG("in onMouseMoved: unlock");
				//show big image
				mUnitImage->setTexture(item.texture);
				mUnitName->setString(item.name);
				mUnitIntroduction->setString(item.introduction);
				std::stringstream ss;
				ss << getDicValue("HP") << item.property.numHitPoint << "\n";
				ss << getDicValue("DEF") << item.property.numDefence << "\n";
				ss << getDicValue("ATK") << item.property.numAttack<< "\n";
				ss << getDicValue("RATK") << item.property.numRangeAttack<< "\n";
				ss << getDicValue("RMOV") << item.property.numRangeMove<< "\n";
				ss << getDicValue("CPU") << item.property.numPopulation<< "\n";
				ss << "\n";
				ss << getDicValue("FIXR") << item.resources.numFixedResource<< "\n";
				ss << getDicValue("RANR") << item.resources.numRandomResource<< "\n";
				ss << getDicValue("PROD") << item.resources.numProductivity<< "\n";
				mUnitResourcesAndProperty->setString(ss.str());
				//sprite effect(tmp)
				item.sprite->setScale(1.3);
			}
			else
			{
				CCLOG("in onMouseMoved: locked");
				//big close Image
				//mUnitImage->setTexture(mCloseTexture);
				mUnitImage->setOpacity(0);
			}
			break;
		}
		else
		{
			mUnitImage->setOpacity(0);
			item.sprite->setScale(1);
			mUnitName->setOpacity(0);
			mUnitIntroduction->setOpacity(0);
			mUnitResourcesAndProperty->setOpacity(0);
		}
	}
}

bool UnitCampLayer::containPoint(Vec2 mousePoint)
{
	//CCLOG("entrance");
	for (auto & item : mItemsList)
	//for (int i = 0; i < mItemsList.size(); ++ i)
	{
		//ItemInCampStruct& item = mItemsList[i];
		Point point = Point(item.sprite->boundingBox().getMinX(), item.sprite->boundingBox().getMinY());
		Point wPoint = item.sprite->getParent()->convertToWorldSpace(point);
		float width = item.sprite->boundingBox().getMaxX() - item.sprite->boundingBox().getMinX();
		float height = item.sprite->boundingBox().getMaxY() - item.sprite->boundingBox().getMinY();
		auto rect = Rect(wPoint.x, wPoint.y, width, height);
		if (rect.containsPoint(mousePoint))
		{
			mNowItem = item;
			return true;
		}
	}
	return false;
}

const UnitEnum UnitCampLayer::getunitMouseOn()
{
	return mNowItem.unit;
}

void UnitCampLayer::setUnlocked(UnitEnum unit, bool unlock)
{
	for (auto & item : mItemsList)
	{
		if (item.unit == unit)
		{
			item.unlocked = unlock;
			if (!unlock)
			{
				item.sprite->setColor(Color3B(100, 100, 100));
			}
			else
			{
				item.sprite->setColor(Color3B(255, 255, 255));
			}
			return;
		}
	}
	//error
}