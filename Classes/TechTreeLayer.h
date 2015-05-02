#ifndef TECHTREELAYER_H
#define TECHTREELAYER_H

#include "cocos2d.h" 
#include <map>
#include "json/document.h"
#include "MyEnums.h"
#include <cmath>

using namespace cocos2d;

class TechTreeLayer : public Layer
{
public:
	TechTreeLayer();
	~TechTreeLayer();
	virtual bool init();
	CREATE_FUNC(TechTreeLayer);
	//method
	//自己的区域包含了光标位置
	bool containPoint(Vec2 mousePoint);
	//返回光标悬停的科技项。仅当包含光标时调用
	TechEnum getTechContainingPoint(Vec2 mousePoint);
	//切换科技显示状态
	void setTechState(TechEnum tech, TechStateEnum techState);
	//吞没，不再往下查找
	bool blockClick()
	{
		return true;
	}


private:
	float width;
	float height;
	float sideLength;
	float spaceLength;
	float directLength;
	float triLength;
	float halfLength;
	//图片本身
	std::map<TechEnum, Sprite *> mTechSpriteMap;
	//绘制在上层的一组，用来做点击区域
	std::map<TechEnum, Sprite *> mClickAreaMap;
	std::map<TechEnum, TechStateEnum> mTechStateMap;
	Dictionary * dictionary;
};

#endif // !TECHTREELAYER_H
