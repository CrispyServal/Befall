#ifndef UNITCAMPLAYER_H
#define	UNITCAMPLAYER_H

#include "cocos2d.h"
#include <map>
#include "MyEnums.h"

using namespace cocos2d;

class UnitCampLayer : public Layer
{
public:
	UnitCampLayer();
	~UnitCampLayer();
	virtual bool init();
	CREATE_FUNC(UnitCampLayer);
	//在光标在按钮上面时候才返回true
	bool containPoint(Vec2 mousePoint);
	bool blockClick = true;
	//返回被点的按钮，用于造兵。仅当包含按钮时调用
	UnitEnum getClickedUnit(Vec2 mousePoint);
	//用于在悬停时候改变按钮图片
	void onMouseMoved(Vec2 mousePoint);
	//展示不同内容的接口
	void displayUnitInfo(const std::string & imageFileName, const std::string & unitName, const std::string & unitIntroduction);
private:
	//不同兵的按钮。按了就造兵的。
	std::map<UnitEnum, Sprite *> mUnitButtonMap;
	//展示单位的大图
	Sprite * mUnitImage;
	//展示单位名称和详细信息的标签
	Label * mUnitName;
	Label * mUnitIntroduction;
};


#endif // UNITCAMPLAYER_H

/**
	布局咨询丁力
	Label 请用 createTTF 生成，字体使用STXIHEI.TTF
*/