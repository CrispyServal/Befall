#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "cocos2d.h"
#include "MyEnums.h"
#include "MyStructs.h"
#include "yypnet.h"
#include "TechTree.h"
using namespace cocos2d;

/*
struct GameStateStruct
{
	//һ�ſƼ���
	TechTree techTree;
	//���굽��λ��map
	std::map<UnitEnum, ResourcesStruct> mInitialUnitResourceMap;
	std::map<UnitEnum, UnitPropertyStruct> mInitialUnitPropertyMap;
	std::map<MyPointStruct, Unit> unitMap;
};
*/


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
	const int MAXSIZE = 22;
	//m
	YypNet mNet;
	MenuItemLabel * backToMainSceneItem;
	//Layers
	//when connecting or listening, display juFlower or something else
	Layer * mWelcomeLayer;
	Layer * mTouchLayer;
	EventListenerMouse * mMouseListener;
	EventListenerTouch * mTouchListener;
	//methods----------------------
	//updates
	void startServer(float delta);
	void startConnecting(float delta);
	//1������ʱ���Ƿ����
	void update(float delta);

	//callback
	void backToMainScene(Ref * sender);
	void onMouseMoved(Event * event);
	//abstract things----------------------------------------
	//0�Ǽ�����1�ǶԷ�
	//GameStateStruct mGameState[2];
	//��ص����ݣ�ֻ��Ҫһ�ݣ����߻�ֱ����
	std::map<MyPointStruct, Unit> mResourceMap;
	//method
	//��ʼ��̬�����������档�˺�����ʼ��˫����GameState�����ܻ���ֵ䡣
	void initGameState();
	//std::vector<MyPointStruct> getNearPoint(const MyPointStruct & point);
	//std::vector<PathNodeStruct> getPathTree(MyPointStruct point, int range, const std::set<MyPointStruct> & barrier);
};

#endif // !GAMESCENE_H
