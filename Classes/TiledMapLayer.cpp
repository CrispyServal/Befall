#include "TiledMapLayer.h"

TiledMapLayer::TiledMapLayer()
{
}

TiledMapLayer::~TiledMapLayer()
{
}

bool TiledMapLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	mTiledMap = TMXTiledMap::create("uiComponent/tiled_map.tmx");
	addChild(mTiledMap);
	return true;
}

MyPointStruct TiledMapLayer::tiledCoorForPostion(const Vec2 & position)
{
	Size mapSize = mTiledMap->getMapSize();
	Size tileSize = mTiledMap->getTileSize();
	//CCLOG("tileSize: %f,%f", tileSize.width, tileSize.height);
	Vec2 positionL = mTiledMap->convertToNodeSpace(position);
	//CCLOG("in c: positionL: %f,%f", positionL.x, positionL.y);
	int x = positionL.x / tileSize.width;
	int y = (mapSize.height * tileSize.height - positionL.y) / tileSize.height;
	//CCLOG("in c: x,y: %d,%d", x, y);
	//int gid = mTiledMap->layerNamed("layer1")->getTileGIDAt(Vec2(x, y));
	//CCLOG("gid: %d", gid);
	return MyPointStruct{ x, y };
}

Vec2 TiledMapLayer::floatCoorForPosition(const MyPointStruct & position)
{
	Size mapSize = mTiledMap->getMapSize();
	Size tileSize = mTiledMap->getTileSize();
	float x = (position.x + 0.5) * tileSize.width;
	float y = (mapSize.height - position.y - 0.5) * tileSize.width;
	return mTiledMap->convertToWorldSpace(Vec2(x, y));
}

bool TiledMapLayer::containPoint(const Vec2 & mousePoint)
{
	/*
	Point point = Point(mTiledMap->getBoundingBox().getMinX(), mTiledMap->getBoundingBox().getMaxY());
	Point pointW = mTiledMap->getParent()->convertToWorldSpace(point);
	float width = mTiledMap->getBoundingBox().getMaxX() - mTiledMap->getBoundingBox().getMinX();
	float height = mTiledMap->getBoundingBox().getMaxY() - mTiledMap->getBoundingBox().getMinY();
	Rect rect = Rect(pointW.x, pointW.y, width, height);
	//debug
	auto dR = DrawNode::create();
	dR->drawRect(pointW, Vec2(pointW.x + width, pointW.y + height), Color4F(0, 1, 0, 1));
	//dR->drawRect(Vec2(rect.getMinX(),rect.getMinY()), Vec2(rect.getMaxX(),rect.getMaxY()) ,Color4F(1, 0, 0, 1));
	//dR->drawRect(point, Vec2(point.x + width, point.y + height), Color4F(1, 0, 0, 1));
	addChild(dR,2);
	if (rect.containsPoint(mousePoint))
	{
		return true;
	}
	else
	{
		return false;
	}
	*/
	if ((tiledCoorForPostion(mousePoint).x >= 0) && (tiledCoorForPostion(mousePoint).y >= 0) && (tiledCoorForPostion(mousePoint).x < mTiledMap->getMapSize().width) && (tiledCoorForPostion(mousePoint).y < mTiledMap->getMapSize().height))
	{
		return true;
	}
	else
	{
		return false;
	}
}

MyPointStruct TiledMapLayer::getMyPointMouseOn(const Vec2 & mousePoint)
{
	return tiledCoorForPostion(mousePoint);
}


void TiledMapLayer::setFocusPoint(Vec2 focus)
{
	float mapW = getMapSizeF().width;
	float mapH = getMapSizeF().height;
	float x = Director::getInstance()->getWinSize().width / 2 - focus.x * mapW;
	float y = Director::getInstance()->getWinSize().height / 2 - focus.y * mapH;
	mTiledMap->setPosition(x, y);
}

Size TiledMapLayer::getMapSizeF()
{
	return Size(mTiledMap->getMapSize().width * mTiledMap->getTileSize().width, mTiledMap->getMapSize().height * mTiledMap->getTileSize().height);
}

Size TiledMapLayer::getMapSize()
{
	return Size(mTiledMap->getMapSize().width, mTiledMap->getMapSize().height);
}

void TiledMapLayer::setTileColor(const MyPointStruct & point, int tileID)
{
	mTiledMap->layerNamed("layer1")->setTileGID(tileID, Vec2(point.x,point.y));
}

void TiledMapLayer::cleanUp()
{
	for (int x = 0; x < mTiledMap->getMapSize().width; ++x)
	{
		for (int y = 0; y < mTiledMap->getMapSize().height; ++y)
		{
			mTiledMap->layerNamed("layer1")->setTileGID(1, Vec2(x, y));
		}
	}
}