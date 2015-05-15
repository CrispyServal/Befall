#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "json/document.h"

#include <map>
#include <vector>
#include <sstream>

#include "MyEnums.h"
#include "MyStructs.h"
#include "YypNoBlockingNet.h"
#include "TechTree.h"
#include "TiledMapLayer.h"
#include "InfoMapLayer.h"
#include "TechFactory.h"
#include "UnitFactory.h"
#include "UnitCampLayer.h"
#include "TechTreeLayer.h"
#include "GameTimer.h"
#include "MiniMapLayer.h"
//using namespace cocos2d;
USING_NS_CC;

//unitMap include unit
struct GameStateStruct
{
	//一颗科技树
	TechTree techTree;
	//unit lock state
	std::map<UnitEnum, bool> unitLockMap;
	//坐标到单位的map
	std::map<MyPointStruct, Unit> unitMap;
	//extra property
	std::map<UnitEnum, UnitPropertyStruct> extraProperty;
};

struct UnitNowDisplayStruct
{
	bool exist;
	UnitEnum mUnitEnum;
	UnitPropertyStruct property;
};

struct UnitInitDataStruct
{
	UnitPropertyStruct property;
	ResourcesStruct consumption;
};

struct ResourceTextureStruct
{
	Texture2D * abundant;
	Texture2D * middle;
	Texture2D * dried;
};

struct ResourceCriticalPointStruct
{
	float firstP;
	float secondP;
};

struct UnitTextureStruct
{
	Texture2D * front;
	Texture2D * back;
	Texture2D * side;
};

struct buttonTextureStruct
{
	Texture2D * off;
	Texture2D * on;
};

struct keyStruct
{
	bool w;
	bool s;
	bool a;
	bool d;
};

struct techIntroductionStruct
{
	std::string techName;
	std::string techIntroduction;
};

struct unitIntroductionStruct
{
	std::string unitName;
	std::string unitIntroduction;
};

const std::map<std::string, UnitEnum> mUnitStringEnumMap =
{
	{ "farmer", farmer},
	{ "shortRangeUnit1", shortrangeunit1 },
	{ "shortRangeUnit2", shortrangeunit2 },
	{ "longRangeUnit1", longrangeunit1 },
	{ "longRangeUnit2", longrangeunit2 },
	{ "longRangeUnit3", longrangeunit3 }
};

const std::vector<TechEnum> mTechEnumList =
{
	techroot,
	techtree1,
	techtree2,
	techtree3,
	unlocklongrangeunit1,
	unlocklongrangeunit2,
	unlockshortrangeunit1,
	unlockshortrangeunit2,
	unlocklongrangeunit3,
	productivity1,
	productivity2,
	productivity3,
	resourcefixed1,
	resourcefixed2,
	resourcefixed3,
	resourcerandom1,
	resourcerandom2,
	resourcerandom3,
	hplongrange1,
	hplongrange2,
	arglongrange1,
	arglongrange2,
	atklongrange1,
	atklongrange2,
	atkshortrange1,
	atkshortrange2,
	defshortrange1,
	defshortrange2,
	mrgshortrange1,
	mrgshortrange2,
	defbase1,
	defbase2
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
	std::string getDicValue(char * str);
	EventDispatcher * mDispatcher;
	float mWinHeight;
	float mWinWidth;
	Vec2 mMouseCoordinate;
	//position for clicked
	Vec2 mMouseCoordinateTouch;
	//last position when mouse moving
	Vec2 mMouseCoordinateP;
	//need to change to tilemap size
	Size mMapSize;
	//m
	YypNoBlockingNet mNet;
	MenuItemLabel * mBackToMainSceneItem;
	DrawNode * mGrayBar;
	Label * mFixedResourceLabel;
	Label * mRandomResourceLabel;
	Label * mProductivityLabel;
	Label * mResearchLabel;
	Label * mPopulationLabel;

	std::string stringTurn;
	std::string stringPredict;
	std::string stringGoing;

	void initResourcesIcons();
	void refreshResourcesIcons(const int & turnFlag);
	void refreshPopulationIcons(const int & turnFlag);
	//a menu
	Node * mGameMenu;
	void initGameMenu();
	//Layers
	GameTimer * mTimer;
	void switchTurn();
	Label * mTurnLabel;
	int mNumTurn;
	TechTreeLayer * mTechTreeLayer;
	UnitCampLayer * mUnitCampLayer;
	void checkTechTreeLayerOnTouchEnded();
	void checkUnitCampLayerOnTouchEnded();
	TiledMapLayer * mTiledMapLayer;
	void checkTiledMapOnTouchMoved();
	Sprite * mTechTreeLayerButton;
	buttonTextureStruct mTechTreeLayerButtonTexture;
	Sprite * mUnitCampLayerButton;
	buttonTextureStruct mUnitCampLayerButtonTexture;
	void checkTechAndUnitButton();
	//waiting ding to finish resources
	Sprite * mTechMakingButton;
	Texture2D * mTechMakingButtonTexture;
	Sprite * mUnitMakingButton;
	Texture2D * mUnitMakingButtonTexture;
	void checkMakingButtonOnMouseMoved();
	void checkMakingButtonOnTouchEnded();
	void refreshMakingButton(int turnF);
	Texture2D * mMakingCancelTexture;

	InfoMapLayer * mInfoMapLayer;
	MiniMapLayer * mMiniMapLayer;
	void refreshMiniMap();
	void checkMiniMap();
	//factory
	UnitFactory mUnitFactory[2];
	void checkUnitFactory(int turnFlag);
	void checkTechFactory(int turnFlag);
	TechFactory mTechFactory[2];
	//when connecting or listening, display juFlower or something else
	//juFlower 2333333333
	Layer * mWelcomeLayer;
	Layer * mTouchLayer;
	EventListenerMouse * mMouseListener;
	EventListenerTouchOneByOne * mTouchListener;
	EventListenerKeyboard * mKeyboardListener;
	//distance per press for map
	const float moveDis = 40;
	keyStruct mKeyStruct;

	void initWelcomeLayer();
	void initYypNet();
	void initResourceTexture();
	void initUnitTexture();
	//net
	void acceptConnect(float delta);
	void startConnecting(float delta);
	//start
	void startGame();
	//update
	void update(float delta);
	void netUpdate(float delta);

	//callback
	void backToMainScene(Ref * sender);
	void onMouseMoved(Event * event);
	bool onTouchBegan(Touch * touch, Event * event);
	void onTouchMoved(Touch * touch, Event * event);
	void onTouchEnded(Touch * touch, Event * event);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event);
	void checkBackToMainSceneItemOnMouseMoved();
	void checkLayersOnMouseMoved();
	//abstract things--------------------

	//共通

	//unit & resources ->initial property and consumption
	//init this map from json file
	UnitPropertyStruct mMaxHitPointOfBase[2];
	std::map<UnitEnum, UnitInitDataStruct> mUnitInitDataMap;
	std::map<UnitEnum, unitIntroductionStruct> mUnitDisplayMap;
	void initUnitData();
	//tech->initial comsumption
	//init this map from json file
	std::map<TechEnum, ResourcesStruct> mTechInitDataMap;
	std::map<TechEnum, InfluenceStruct> mTechInitInfluenceMap;
	std::map<TechEnum, techIntroductionStruct> mTechDisplayMap;
	//init tech->comsumption map
	void initTechData();
	//矿藏的数据，只需要一份，两边会分别减少
	std::map<MyPointStruct, Unit> mResourceMap;

	//criticalPoint map
	const std::map<UnitEnum, ResourceCriticalPointStruct> mResourceCriticalMap = { {
		{ fixedResource, { 0.6, 0.3 } },
		{ randomResource, { 0.6, 0.3 } }
	} };
	void refreshResourcesTexture();
	//init coordinate->fixedResource from json file
	void initResourceMap();
	//num of random resource
	const int mNumOfRandomResource = 30;
	int mHitPointOfRandomResource = 0;
	//resource->texture. each resource -> 3 texture
	std::map<UnitEnum, ResourceTextureStruct> mResourceTextureMap;
	//unit->texture. each unit->3 texture
	std::map<UnitEnum, UnitTextureStruct> mUnitTextureMap[2];
	//basePosition. 0 for server, 1 for client
	std::vector<MyPointStruct> mBasePosition;

	//双边

	//all game states,0 for server
	GameStateStruct mGameState[2];
	//spawn
	MyPointStruct mSpawn[2];
	bool spawnOccupied(int turnFlag);
	//population
	int mPopulation[2];

	const int mPopulationLimit = 100;

	//单边（误

	//resourses
	ResourcesStruct mResources[2];
	//effective resoures
	//e.g. productivity += numFarmer * (Farmer.numAttack + mExtraResources);
	ResourcesStruct mExtraResources[2];
	//effeciency of collection
	ResourcesStruct mCollectionEffeciency;
	//sign whether it is my turn
	bool mBlueTurn;
	//if false, cannot do any thing about data
	bool mOperateEnable;
	//method
	//初始静态数据在这里面。此函数初始化双方的GameState。可能会读字典。
	void initGameState();
	std::vector<MyPointStruct> getNearPoint(const MyPointStruct & point);
	std::vector<PathNodeStruct> getPathTree(MyPointStruct point, int range, const std::set<MyPointStruct> & barrier);
	std::vector<MyPointStruct> getPath(const std::vector<PathNodeStruct> & pathTree, MyPointStruct pointTo);
	//spawn unit
	void spawnUnit(UnitEnum unit, int turnFlag);
	//unit moving
	void moveUnit(std::vector<MyPointStruct> path, int turnFlag);

	//Tech Influence
	void setTechInfluence(const int & flag, TechEnum tech);
	void unlockTechTree(const int & flag, TechEnum tech);
	void refreshTechTree(const int & flag);

	//
	void refreshUnitCamp(const int & flag);
	
	//calc turn left
	int calcInteger(int a, int b);

	UnitNowDisplayStruct existUnitOnTiledMap(const MyPointStruct & mPos);
};

#endif // !GAMESCENE_H

/*
	点科技时候，把追加属性设置到兵营层去
	net Example:
	while (!mNet.sendOnePoint(ranP))
	{
		auto err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
		{
			mDirector->popScene();
		}
	}
*/