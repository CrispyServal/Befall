#include "MiniMapLayer.h"

MiniMapLayer::MiniMapLayer()
{

}
MiniMapLayer::~MiniMapLayer()
{

}
bool MiniMapLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	// find location
	auto director = Director::getInstance();
	mWidth = director->getWinSize().width;
	mHeight = director->getWinSize().height;
	// initial
	pointSize = 10;
	mSizeX = 22;
	mSizeY = 22;
	mViewBoxSizeX = 10 * pointSize;
	mViewBoxSizeY = 5.625 * pointSize;
	//mBg
	mBg = DrawNode::create();
	mBg->drawSolidRect(Vec2( - mSizeX * pointSize / 2.0,  - mSizeY * pointSize / 2.0), Vec2(mSizeX * pointSize / 2.0, mSizeY * pointSize / 2.0), Color4F(0, 0, 0, 0.4));
	mBg->setPosition(mWidth / 2, mHeight / 2);
	addChild(mBg);
	//minmap
	mMiniMap = DrawNode::create();
	mMiniMap->setPosition(mWidth / 2, mHeight / 2);
	addChild(mMiniMap);
	//viewbox
	mViewBox = DrawNode::create();
	mViewBox->drawRect(Vec2(-mViewBoxSizeX / 2, -mViewBoxSizeY / 2), Vec2(mViewBoxSizeX / 2, mViewBoxSizeY / 2), Color4F(1, 1, 1, 1));
	mViewBox->setPosition(mWidth / 2, mHeight / 2);
	addChild(mViewBox);
	return true;
}
//method
void MiniMapLayer::setPointSize(float size)
{
	pointSize = size;
	mViewBoxSizeX = 10 * pointSize;
	mViewBoxSizeY = 5.625 * pointSize;
	//mBg
	mBg->clear();
	mBg->drawSolidRect(Vec2(-mSizeX * pointSize / 2.0, -mSizeY * pointSize / 2.0), Vec2(mSizeX * pointSize / 2.0, mSizeY * pointSize / 2.0), Color4F(0, 0, 0, 0.4));
	//
	//viewbox
	mViewBox->clear();
	mViewBox->drawRect(Vec2(-mViewBoxSizeX / 2, -mViewBoxSizeY / 2), Vec2(mViewBoxSizeX / 2, mViewBoxSizeY / 2), Color4F(1, 1, 1, 1));
}
void MiniMapLayer::setMapSize(int width, int height)
{
	mMiniMap->clear();
	mSizeX = width;
	mSizeY = height;
	mMiniMap->drawSolidRect(Vec2(-mSizeX * pointSize / 2.0, -mSizeY * pointSize / 2.0), Vec2(mSizeX * pointSize / 2.0, mSizeY * pointSize / 2.0), Color4F(0, 0, 0, 0.4));
}
void MiniMapLayer::setViewBoxSize(float ViewBoxScalarX, float ViewBoxScalarY)
{
	mViewBox->clear();
	mViewBoxSizeX = ViewBoxScalarX * pointSize;
	mViewBoxSizeY = ViewBoxScalarY * pointSize;	
	mViewBox->drawRect(Vec2(-mViewBoxSizeX / 2, -mViewBoxSizeY / 2), Vec2(mViewBoxSizeX / 2, mViewBoxSizeY / 2), Color4F(1, 1, 1, 1));
}
//传入左下角为（0,0）的鼠标坐标，如果本层中的有效区域包含此坐标，返回true
//小地图中，几乎所有区域都是有效区域
bool MiniMapLayer::containPoint(Vec2 mousePoint)// need  to update
{
	float distenceX = ( mSizeX * pointSize + mViewBoxSizeX ) / 2.0;
	float distenceY = ( mSizeY * pointSize + mViewBoxSizeY ) / 2.0;
	Point wpoint = mBg->getParent()->convertToWorldSpace(mBg->getPosition());
	if (abs(wpoint.x - mousePoint.x) <= distenceY && abs(wpoint.y-mousePoint.y) <= distenceY)
		return true;
	else
		return false;
}

//改变小地图中的视野框的位置
//如果视野框与边框相切，函数应当不再改变位置防止出界。
void MiniMapLayer::moveView(Vec2 mousePoint)
{
	/*
	float maxX = (mSizeX * pointSize + (mViewBoxSizeY - mViewBoxSizeX)) / 2.0;
	CCLOG("vBSize: %f,%f", mViewBoxSizeX, mViewBoxSizeY);
	*/
	//float maxX = ( mSizeX * pointSize /*+ mViewBoxSizeY - mViewBoxSizeX*/) / 2.0;
	/*
	CCLOG("maxX: %f", maxX);
	float maxY = ( mSizeY * pointSize ) / 2.0;
	Point wBgpoint = mBg->getParent()->convertToWorldSpace(mBg->getPosition());	
	Point wVBpoint = mViewBox->getParent()->convertToWorldSpace(mViewBox->getPosition());
	float DirectX = mousePoint.x - wBgpoint.x;
	CCLOG("directX: %f", DirectX);
	float DirectY = mousePoint.y - wBgpoint.y;
	float distenceX = mousePoint.x - wVBpoint.x;
	float distenceY = mousePoint.y - wVBpoint.y;
	if (DirectX > maxX )
	{
		distenceX = distenceX - (DirectX - maxX);
	}
	else if (DirectX < -maxX)
	{
		CCLOG("left fix: distenceX: %f",distenceX);
		distenceX = distenceX - (DirectX + maxX);
		CCLOG("\tafter fixing: %f", distenceX);
	}
	if (DirectY > maxY)
	{
		distenceY = distenceY - (DirectY - maxY);
	}
	else if (DirectY < -maxY)
	{
		distenceY = distenceY - (DirectY + maxY);
	}
	//mViewBox->setPositionX(mViewBox->getPositionX() + distenceX);
	//mViewBox->setPositionY(mViewBox->getPositionY() + distenceY);
	*/
}
//返回视野中心在小地图中的位置的比例，数值在[0+,1-]，用于外部地图把视野设置到对应位置
//由于视野框在moveView函数中维护，且不会让视野框出界，因此返回的值范围不会到[0,1]
Vec2 MiniMapLayer::getViewPosition(Vec2 mousePoint)
{
	float maxX = (mSizeX * pointSize ) / 2.0;
	float maxY = (mSizeY * pointSize ) / 2.0;
	Point wBgpoint = mBg->getParent()->convertToWorldSpace(mBg->getPosition());
	float DirectX = mousePoint.x - wBgpoint.x;
	float DirectY = mousePoint.y - wBgpoint.y;
	if (DirectX > maxX)
	{
		DirectX = maxX;
	}
	else if (DirectX < -maxX)
	{
		DirectX = -maxX;
	}
	if (DirectY > maxY)
	{
		DirectY = maxY;
	}
	else if (DirectY < -maxY)
	{
		DirectY = -maxY;
	}
	Vec2 scalar;
	scalar.x = DirectX / ( mSizeX * pointSize ) + 0.5;
	scalar.y = DirectY / ( mSizeY * pointSize )+ 0.5;
	return scalar;
}

void MiniMapLayer::setViewPosition(Vec2 rate)
{
	Vec2 newPosition;
	newPosition.x = mBg->getPositionX() + (rate.x - 0.5) * (mSizeX * pointSize);
	newPosition.y = mBg->getPositionY() + (rate.y - 0.5) * (mSizeY * pointSize);
	mViewBox->setPosition(newPosition);
}

//传入4个集合，刷新小地图中的小方块，即重画一遍
void MiniMapLayer::refresh(
	std::set<MyPointStruct> unitSet0,//blue
	std::set<MyPointStruct> unitSet1,//red
	std::set<MyPointStruct> fixedResourceSet,//yellow
	std::set<MyPointStruct> randomResourceSet//green
	)
{
	mMiniMap->clear();
	float red, green, blue;
	int x, y;
	for (auto point : fixedResourceSet)
	{
		red = 1;
		green = 1;
		blue = 0;
		x = (point.x - mSizeX / 2.0 + 0.5) * pointSize;
		y = (mSizeY / 2.0 - point.y - 0.5) * pointSize;
		mMiniMap->drawSolidRect(Vec2(x - pointSize / 2, y - pointSize / 2), Vec2(x + pointSize / 2, y + pointSize / 2), Color4F(red, green, blue, 1));
	}
	for (auto point : randomResourceSet)
	{
		red = 0;
		green = 1;
		blue = 1;
		x = (point.x - mSizeX / 2.0 + 0.5) * pointSize;
		y = (mSizeY / 2.0 - point.y - 0.5) * pointSize;
		mMiniMap->drawSolidRect(Vec2(x - pointSize / 2, y - pointSize / 2), Vec2(x + pointSize / 2, y + pointSize / 2), Color4F(red, green, blue, 1));
	}
	for (auto point : unitSet0)
	{
		red = 0;
		green = 0;
		blue = 1;
		x = (point.x - mSizeX / 2.0 + 0.5) * pointSize;
		y = (mSizeY / 2.0 - point.y - 0.5) * pointSize;
		mMiniMap->drawSolidRect(Vec2(x - pointSize / 2, y - pointSize / 2), Vec2(x + pointSize / 2, y + pointSize / 2), Color4F(red, green, blue, 1));
	}
	for (auto point : unitSet1)
	{
		red = 1;
		green = 0;
		blue = 0;
		x = (point.x - mSizeX / 2.0 + 0.5) * pointSize;
		y = (mSizeY / 2.0 - point.y - 0.5) * pointSize;
		mMiniMap->drawSolidRect(Vec2(x - pointSize / 2, y - pointSize / 2), Vec2(x + pointSize / 2, y + pointSize / 2), Color4F(red, green, blue, 1));
	}
}