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

//GameState 
//Default Property:
//mUnitInitDataMap for all elements

//Extra Property:
//extraProperty for all units
//mMaxHitPointOfBase = defaultProperty + effective tech influence

//Now Propert:
//GameState[tF].unitMap
//mResourceMap[mBasePosition[flag]].property

//unitMap include unit
struct GameStateStruct
{
	//һ�ſƼ���
	TechTree techTree;
	//unit lock state
	std::map<UnitEnum, bool> unitLockMap;
	//���굽��λ��map
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

struct ResourceCollectionStruct
{
	//-1:wild; 0:blue; 1:read
	int owner;
	int numOfFarmer;
};

struct AttackTextureStruct{
	Texture2D * LR1T;
	Texture2D * LR1E;
	Texture2D * LR2T;
	Texture2D * LR2E;
	Texture2D * LR3E;
	Texture2D * SR2T;
	Texture2D * SR2E;
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
	std::vector<Rect> mGrayBarRect;
	Node * resourcesIcons;
	Label * mFixedResourceLabel;
	Label * mRandomResourceLabel;
	Label * mProductivityLabel;
	Label * mResearchLabel;
	Label * mPopulationLabel;
	Sprite * fixedResourceIcon;
	Sprite * randomResourceIcon;
	Sprite * productivityIcon;
	Sprite * researchIcon;
	Sprite * populationIcon;
	//Chceck contains
	bool isGrayBarContains(Vec2 mMouseCoordinate);
	bool isResourcesIconsContains(Vec2 mMouseCoordinate, ResourcesIconEnum tag);

	//DisplayString
	map<string, string> mDisplayInfoMap;

	void initResourcesIcons();
	void refreshResourcesIcons(const int & turnFlag);
	void refreshPopulationIcons(const int & turnFlag);
	//a menu
	Node * mGameMenu;
	void initGameMenu();
	//Layers
	GameTimer * mTimer;
	void switchTurn();
	bool mUpdateTimerLock;
	Label * mTurnLabel;
	int mNumTurn;
	TechTreeLayer * mTechTreeLayer;
	UnitCampLayer * mUnitCampLayer;
	void checkTechTreeLayerOnTouchEnded();
	void checkUnitCampLayerOnTouchEnded();
	TiledMapLayer * mTiledMapLayer;
	void checkTiledMapOnTouchMoved();
	Sprite * mStarMap;
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
	const float starMovePerMove = 0.5;
	keyStruct mKeyStruct;

	void initWelcomeLayer();
	bool mWelcomeLayerDisplay;
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
	void readTwoPoint(const int & tF);

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

	//��ͨ

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
	//��ص����ݣ�ֻ��Ҫһ�ݣ����߻�ֱ����
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

	//˫��

	//all game states,0 for server
	GameStateStruct mGameState[2];
	//when switching turn, change unit state to fresh
	void refreshUnitState(const int & turnFlag);
	//spawn
	MyPointStruct mSpawn[2];
	bool spawnOccupied(int turnFlag);
	//population
	int mPopulation[2];

	const int mPopulationLimit = 100;

	//���ߣ���

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
	std::vector<MyPointStruct> spawnPoint;
	//��ʼ��̬�����������档�˺�����ʼ��˫����GameState�����ܻ���ֵ䡣
	void initGameState();
	std::vector<MyPointStruct> getNearPoint(const MyPointStruct & point);
	std::vector<PathNodeStruct> getPathTree(MyPointStruct point, int range, const std::set<MyPointStruct> & barrier);
	std::vector<MyPointStruct> getPath(const std::vector<PathNodeStruct> & pathTree, MyPointStruct pointTo);
	//spawn unit
	void spawnUnit(UnitEnum unit, int turnFlag);
	void spawnUnit(UnitEnum unit, int turnFlag, const MyPointStruct & pos);
	//unit moving
	void moveUnit(std::vector<MyPointStruct> path, int turnFlag, bool showAttachRange = false);
	//tF�ǹ�����Դ�����з�
	void attackUnit(const MyPointStruct & from, const MyPointStruct & attackedUnitPositionto, const int & tF);
	AttackTextureStruct mAttackTexture;
	void initAttackTexture();

	//Tech Influence
	void setTechInfluence(const int & flag, TechEnum tech);
	void unlockTechTree(const int & flag, TechEnum tech);
	void refreshTechTreeLayer(const int & flag);

	//
	void refreshUnitCamp(const int & flag);
	
	//calc turn left
	int calcInteger(int a, int b);

	UnitNowDisplayStruct existUnitOnTiledMap(const MyPointStruct & mPos);

	//yyp
	void showMoveRange(const MyPointStruct & unitPoint, const int & tF);
	void showAttackRange(const MyPointStruct & unitPoint, const int & tF);
	void deleteMoveRange();
	void deleteAttackRange();
	void unitAction(const MyPointStruct & nowPoint, int tF);
	std::vector <PathNodeStruct> mMoveRange;
	std::set <MyPointStruct> mAttackRange;
	MyPointStruct mOriginalPoint;
	int mUnitActionFSM[2];

	//for unit, resource, base
	void die(const MyPointStruct & point, const int & tF);

	//resources collection
	std::map<MyPointStruct, MyPointStruct> mFarmerResourceMap[2];
	//û�鵽������Ҳ����Ұ��ʡȥ��ʼ��
	std::map<MyPointStruct, ResourceCollectionStruct> mResourceCollectionMap;
	//ˢ�¿����ũ������0ʱ�Զ���ΪҰ��
	void refreshResourceCollectionState(const MyPointStruct & resourcePosition, bool increase, const int & tF);
	//��һ����ĳһ����ũ���ܲ��ܲ�
	bool collecable(const MyPointStruct & resourcePosition, const int & tF);
	//ũ���ƶ�ʱ�����ô˺��������ũ���뿪��ԭ���Ŀ󣬿������ͳ��
	//ֻ�����
	void collectionFarmerMove(const MyPointStruct & farmerFrom, const int & tF);
	//ũ�񹥻�ʱ�����ô˺������Զ�����ͳ��״̬
	void collectionFamerAttack(const MyPointStruct & farmerFrom, const MyPointStruct & farmerTo, const int & tF);
	//
	void refreshResource(const int & tF);

	//
	int whosUnit(MyPointStruct unitPoint);
	//animation items
	Sprite * ball;
	Sprite * explosives;
	DrawNode * whiteLine;
	Sprite * mWinImage[2];
	Sprite * mFailImage[2];
	bool mWinFlag;
	void win(const int & tF);
	void delayAndQuit(float delta);


	//Music
	void initMusic();
	void playUnitSound(UnitEnum mUnit);
	void playEffect(const char * filePath);
	void playBackgroundMusic(const char * filePath);
	int intensive;
	bool isPlayingIntensive;

	//caidan
	void surpriseEgg();

	//mist
	bool mist;
	int sightRange;
	std::vector<Unit> visibleUnit;
	void addMist(const int & tF, bool beginOfTurn = false);
	void addMist();

};

#endif // !GAMESCENE_H

/*
	��Ƽ�ʱ�򣬰�׷���������õ���Ӫ��ȥ
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
/*
	���ش���mResourceMap���档��Ϊ������Ҫ����ͨ�š�һ������ʱ����Ȼ��ͨ�š�
	���ǻ��صĳ�ʼ��Ϣ����uniinitmap���档
*/
//lq : working on attackUnit