#ifndef UNITCAMPLAYER_H
#define	UNITCAMPLAYER_H

#include "cocos2d.h"
#include <vector>
#include <map>
#include <sstream>
#include "MyEnums.h"
#include "MyStructs.h"
#include "json\document.h"


using namespace cocos2d;

namespace UnitCamp
{
	struct ItemInCampStruct
	{
		UnitEnum unit;
		Sprite * sprite;
		bool unlocked;
		Texture2D * texture;
		std::string name;
		std::string introduction;
		ResourcesStruct resources;
		UnitPropertyStruct property;
	};
}

class UnitCampLayer : public Layer
{
public:
	UnitCampLayer();
	~UnitCampLayer();
	virtual bool init();
	CREATE_FUNC(UnitCampLayer);
	//在光标在按钮上面时候才返回true
	bool containPoint(Vec2 mousePoint);
	bool blockClick()
	{
		return true;
	}
	//返回被点的按钮，用于造兵。仅当包含按钮时调用
	const UnitEnum getUnitMouseOn();
	//用于在悬停时候改变按钮图片
	void onMouseMoved(Vec2 mousePoint);
	//lock on or lock off
	void setUnlocked(UnitEnum unit,bool unlock);
	//set resource and property
	void setUnitResourceAndProperty(UnitEnum unit, const ResourcesStruct & unitResources, const UnitPropertyStruct & unitProperty);
	std::string getUnitName(UnitEnum mUnitEnum);
	std::string getUnitIntroDuction(UnitEnum mUnitEnum);
	UnitPropertyStruct getUnitProperty(UnitEnum mUnitEnum);
	int getUnitProductivity(UnitEnum mUnitEnum);

	Texture2D * getUnitTexture(UnitEnum unit);
	
	const ResourcesStruct getUnitResources(UnitEnum unit) const;
	void setFonts(const std::string & fontsFilePath);
private:
	//每个单位的数据存储
	std::vector<UnitCamp::ItemInCampStruct> mItemsList;
	std::string UnitCampLayer::getDicValue(char * str);
	Dictionary * mDictionary;
	std::vector<UnitEnum> mUnitsList;
	std::map<UnitEnum, bool> mUnitUnlockMap;
	//展示单位的大图
	Sprite * mUnitImage;
	//展示单位名称和详细信息的标签
	Label * mUnitName;
	Label * mUnitIntroduction;
	//resources and property
	Label * mUnitResourcesAndProperty;
	//半透明总背景
	Sprite * mBg;
	//当前鼠标位置上的按钮
	UnitCamp::ItemInCampStruct mNowItem;
	//特殊的一张texture, 未解锁的单位的大图
	//Texture2D * mCloseTexture;
	//textures
	std::map<UnitEnum,Texture2D *> mUnitTexture;
	std::string mFonts;
};


#endif // UNITCAMPLAYER_H

/**
	布局咨询丁力
	Label 请用 createTTF 生成，字体使用STXIHEI.TTF
*/