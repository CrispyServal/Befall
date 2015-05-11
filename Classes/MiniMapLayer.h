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
	//传入左下角为（0,0）的鼠标坐标，如果本层中的有效区域包含此坐标，返回true
	//小地图中，几乎所有区域都是有效区域
	void setPointSize(float size);
	void setMapSize(int width, int height);
	void setViewBoxSize(float ViewBoxScalarX, float ViewBoxScalarY);
	bool containPoint(Vec2 mousePoint);
	//这个不用动
	bool blockClick()
	{
		return true;
	}
	//改变小地图中的视野框的位置
	//如果视野框与边框相切，函数应当不再改变位置防止出界。
	void moveView(Vec2 mousePoint);
	//返回视野中心在小地图中的位置的比例，数值在[0+,1-]，用于外部地图把视野设置到对应位置
	//由于视野框在moveView函数中维护，且不会让视野框出界，因此返回的值范围不会到[0,1]
	Vec2 getViewPosition(Vec2 mousePoint);
	//传入4个集合，刷新小地图中的小方块，即重画一遍
	void refresh(
		std::set<MyPointStruct> unitSet0,
		std::set<MyPointStruct> unitSet1,
		std::set<MyPointStruct> fixedResourceSet,
		std::set<MyPointStruct> randomResourceSet
		);
private:
	//内部实现推荐使用drawNode
	float mWidth;
	float mHeight;
	int mSizeX;
	int mSizeY;
	float mViewBoxSizeX;
	float mViewBoxSizeY;
	float pointSize;
	DrawNode * mBg;
	DrawNode * mMiniMap;
	DrawNode * mViewBox;
};

#endif // !MINIMAPLAYER_H
/*
	综述：
	外部维护鼠标位置，点下鼠标时首先调用containPoint确认有没有点到小地图，
	如果点到了，会调用moveView来刷新视野框。
	并且调用getViewPosition得到位置，用来改变真实的地图
	另外，在外部单位移动时，会调用refresh刷新小地图。
*/