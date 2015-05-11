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
//using namespace cocos2d;
USING_NS_CC;

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
	void initResourcesIcons();
	//a menu
	Node * mGameMenu;
	void initGameMenu();
	//Layers
	GameTimer * mTimer;
	TechTreeLayer * mTechTreeLayer;
	UnitCampLayer * mUnitCampLayer;
	TiledMapLayer * mTiledMapLayer;
	Sprite * mTechTreeLayerButton;
	buttonTextureStruct mTechTreeLayerButtonTexture;
	Sprite * mUnitCampLayerButton;
	buttonTextureStruct mUnitCampLayerButtonTexture;
	void checkTechAndUnitButton();
	InfoMapLayer * mInfoMapLayer;
	//factory
	UnitFactory mUnitFactory;
	TechFactory mTechFactory;
	//when connecting or listening, display juFlower or something else
	Layer * mWelcomeLayer;
	Layer * mTouchLayer;
	EventListenerMouse * mMouseListener;
	EventListenerTouchOneByOne * mTouchListener;
	EventListenerKeyboard * mKeyboardListener;
	const float moveDis = 20;
	keyStruct mKeyStruct;
	//methods----------------------------------------------------------
	void initWelcomeLayer();
	void initYypNet();
	void initResourceTexture();
	void initUnitTexture();
	//updates
	void acceptConnect(float delta);
	void startConnecting(float delta);
	void startGame();
	//1������ʱ���Ƿ����
	void update(float delta);
	void NetUpdate(float delta);

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

	//unit->initial property and consumption
	//init this map from json file
	std::map<UnitEnum, UnitInitDataStruct> mUnitInitDataMap;
	void initUnitData();
	//tech->initial comsumption
	//init this map from json file
	std::map<TechEnum, ResourcesStruct> mTechInitDataMap;
	//init tech->comsumption map
	void initTechData();
	//��ص����ݣ�ֻ��Ҫһ�ݣ����߻�ֱ����
	std::map<MyPointStruct, Unit> mResourceMap;
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

	//all game states,0 for this
	GameStateStruct mGameState[2];

	//����

	//resourses
	ResourcesStruct mResources;
	//effeciency of collection
	ResourcesStruct mCollectionEffeciency;
	//sign whether it is my turn
	bool mMyTurn;
	//method
	//��ʼ��̬�����������档�˺�����ʼ��˫����GameState�����ܻ���ֵ䡣
	void initGameState();
	std::vector<MyPointStruct> getNearPoint(const MyPointStruct & point);
	std::vector<PathNodeStruct> getPathTree(MyPointStruct point, int range, const std::set<MyPointStruct> & barrier);
};

#endif // !GAMESCENE_H
