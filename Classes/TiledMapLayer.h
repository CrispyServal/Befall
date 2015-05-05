#ifndef TILEDMAPLAYER_H
#define TILEDMAPLAYER_H

#include "cocos2d.h"
#include "MyStructs.h"

using namespace cocos2d;

class TiledMapLayer : public Layer
{
public:
	TiledMapLayer();
	~TiledMapLayer();
	virtual bool init();
	CREATE_FUNC(TiledMapLayer);
	bool blockClick()
	{
		return true;
	}
	bool containPoint(const Vec2 & mousePoint);
	MyPointStruct getMyPointMouseOn(const Vec2 & mousePoint);
	//focus.x in (0,1), y too
	//place focus to the center of windows
	void setFocusPoint(Vec2 focus);
	Size getMapSize();
	//set all tile to gid1
	void cleanUp();
	void setTileColor(const MyPointStruct & point, int tileID);
	//const gid
	const int mGidNull = 1;
	const int mGidBlue = 2;
	const int mGidRed = 3;
private:
	TMXTiledMap * mTiledMap;
	//get coordinate in tiledmap from Vec2
	MyPointStruct tiledCoorForPostion(const Vec2 & position);
};

#endif // !TILEDMAPLAYER_H
