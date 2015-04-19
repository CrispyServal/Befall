#ifndef MINIMAPLAYER_H
#define MINIMAPLAYER_H

#include "cocos2d.h"
#include "MyStructs.h"
#include <set>

using namespace cocos2d;

class MiniMapLayer : public Layer
{
public:
	MiniMapLayer();
	~MiniMapLayer();
	virtual bool init();
	CREATE_FUNC(MiniMapLayer);
	//method
	bool containPoint(Vec2 mousePoint);
	//吞没点击不往下传递
	bool blockClick = true;
	//传入光标位置，返回视野中心在地图大小中的比例数值。例如返回0.3，0.42。仅当包含鼠标时调用。
	Vec2 getViewPosition(Vec2 mousePoint);
	//刷新小地图。4种颜色的东西。方法：清空drawNode重画
	void refresh(
		std::set<MyPointStruct> unitSet0,
		std::set<MyPointStruct> unitSet1,
		std::set<MyPointStruct> fixedResourceSet,
		std::set<MyPointStruct> randomResourceSet
		);
	//实现:拖动视野框，并且自行限制范围。
	void onTouchBegan(Vec2 mousePoint);
	void onTouchMoved(Vec2 mousePoint);
	void onTouchEnded(Vec2 mousePoint);
private:
	//一堆正方形
	DrawNode * mDrawNode;
	//视野框
	DrawNode * mViewRect;
};

#endif // !MINIMAPLAYER_H
