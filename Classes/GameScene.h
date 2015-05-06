#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "json/document.h"

#include <map>
#include <vector>

#include "MyEnums.h"
#include "MyStructs.h"
#include "YypNoBlockingNet.h"
#include "TechTree.h"
#include "TiledMapLayer.h"
#include "InfoMapLayer.h"
#include "TechFactory.h"
#include "UnitFactory.h"
#include "UnitCampLayer.h"
//using namespace cocos2d;
USING_NS_CC;

struct GameStateStruct
{
	//一颗科技树
	TechTree techTree;
	//坐标到单位的map
	std::map<MyPointStruct, Unit> unitMap;
	//extra property
	std::map<UnitEnum, UnitPropertyStruct> extraProperty;
};

struct UnitInitDataStruct
{
	UnitPropertyStruct property;
	ResourcesStruct consumption;
};

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene();
	virtual bool init();
	CREATE_FUNC(GameScene);
private:
	//global
	Director * mDirector;
	UserDefault * mUserDefault;
	GameModeEnum mGameMode;
	Dictionary * mDictionary;
	EventDispatcher * mDispatcher;
	float mWinHeight;
	float mWinWidth;
	Vec2 mMouseCoordinate;
	//need to change to tilemap size
	const int MAXSIZE = 22;
	//m
	YypNoBlockingNet mNet;
	MenuItemLabel * mBackToMainSceneItem;
	//Layers
	TiledMapLayer * mTiledMapLayer;
	//when connecting or listening, display juFlower or something else
	Layer * mWelcomeLayer;
	Layer * mTouchLayer;
	EventListenerMouse * mMouseListener;
	EventListenerTouch * mTouchListener;
	//methods----------------------------------------------------------
	void initWelcomeLayer();
	void initYypNet();
	//updates
	void acceptConnect(float delta);
	void startConnecting(float delta);
	void startGame();
	//1、检查计时器是否结束
	void update(float delta);

	//callback
	void backToMainScene(Ref * sender);
	void onMouseMoved(Event * event);
	void checkBackToMainSceneItem();
	//abstract things--------------------

	//共通

	//unit->initial property and consumption
	//init this map from json file
	std::map<UnitEnum, UnitInitDataStruct> mUnitInitDataMap;
	void initUnitData();
	//tech->initial comsumption
	//init this map from json file
	std::map<TechEnum, ResourcesStruct> mTechInitDataMap;
	void initTechData();
	//矿藏的数据，只需要一份，两边会分别减少
	std::map<MyPointStruct, Unit> mResourceMap;
	//init coordinate->fixedResource from json file
	void initResourceMap();
	//basePosition. 0 for server, 1 for client
	std::vector<MyPointStruct> mBasePosition;

	//双边

	//all game states,0 for this
	GameStateStruct mGameState[2];

	//单边

	//resourses
	ResourcesStruct mResources;
	//effeciency of collection
	ResourcesStruct mCollectionEffeciency;
	//sign whether it is my turn
	bool mMyTurn;
	//method
	//初始静态数据在这里面。此函数初始化双方的GameState。可能会读字典。
	void initGameState();
	std::vector<MyPointStruct> getNearPoint(const MyPointStruct & point);
	std::vector<PathNodeStruct> getPathTree(MyPointStruct point, int range, const std::set<MyPointStruct> & barrier);
};

#endif // !GAMESCENE_H
