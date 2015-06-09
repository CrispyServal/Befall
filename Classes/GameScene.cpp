#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

std::string GameScene::getDicValue(char * str)
{
	CCLOG("getdicValue: result: %s", mDictionary->valueForKey(str)->getCString());
	return std::string{ mDictionary->valueForKey(str)->getCString() };
}

std::vector<MyPointStruct> GameScene::getNearPoint(const MyPointStruct & point)
{
	std::vector<MyPointStruct> result;
	result.clear();
	if (point.x > 0)
	{
		result.push_back(MyPointStruct{ point.x - 1, point.y });
	}
	if (point.y > 0)
	{
		result.push_back(MyPointStruct{ point.x, point.y - 1 });
	}
	if (point.x < mMapSize.width - 1)
	{
		result.push_back(MyPointStruct{ point.x + 1, point.y });
	}
	if (point.y < mMapSize.height - 1)
	{
		result.push_back(MyPointStruct{ point.x, point.y + 1 });
	}
	/*debug
	for (auto i : result)
	{
	cout << "near: " << i.x << "," << i.y << endl;
	}
	*/
	return result;
}

std::vector<PathNodeStruct> GameScene::getPathTree(MyPointStruct point, int range, const std::set<MyPointStruct> & barrier)
{
	std::vector<PathNodeStruct> result;
	result.clear();

	int head = 0;
	int tail = 1;
	//init
	result.push_back(PathNodeStruct{ point, head, range });

	std::set<MyPointStruct> visitedSet;
	visitedSet.clear();

	while (head != tail)
	{
		if (result[head].stepLeft == 0)
		{
			break;
		}
		//cout << "head step:" << result[head].stepLeft << endl;
		for (auto n : getNearPoint(result[head].point))
		{
			//cout << "n: " << n.x << ", " << n.y << endl;
			if ((barrier.find(n) == barrier.end()) && (visitedSet.find(n) == visitedSet.end()))
			{
				/*debug
				cout << "new point: " << n.x << ", " << n.y << endl;
				cout << "head now: " << head << ", " << "tail now:" << tail << endl;
				*/
				visitedSet.insert(n);
				result.push_back(PathNodeStruct{ n, head, result[head].stepLeft - 1 });
				++tail;
			}
		}
		//++ first!!!!
		++head;
	}
	return result;
}

//need to be tested
std::vector<MyPointStruct> GameScene::getPath(const std::vector<PathNodeStruct> & pathTree, MyPointStruct pointTo)
{
	std::vector<MyPointStruct> result;
	//check
	bool findT = false;
	PathNodeStruct opNode;
	for (const auto & node : pathTree)
	{
		if (node.point == pointTo)
		{
			findT = true;
			opNode = node;
		}
	}
	if (!findT)
	{
		//error
		return result;
	}
	//
	while (opNode != pathTree[opNode.indexParent])
	{
		result.insert(result.begin(), opNode.point);
		opNode = pathTree[opNode.indexParent];
	}
	result.insert(result.begin(), opNode.point);
	return result;
}

bool GameScene::init()
{

	if (!Scene::init())
	{
		return false;
	}
	//global
	initTheme();
	mDirector = Director::getInstance();
	mUserDefault = UserDefault::getInstance();
	mapName = mUserDefault->getStringForKey("map");
	mDictionary = Dictionary::createWithContentsOfFile(
		std::string{ "dictionary/" + mCh + ".xml" }.c_str());
	mDictionary->retain();
	mKeyStruct = {
		false,
		false,
		false,
		false
	};
	initAttackTexture();
	initMusic();
	initMist();


	ball = Sprite::createWithTexture(mAttackTexture.LR2T);
	ball->setOpacity(0);
	explosives = Sprite::createWithTexture(mAttackTexture.LR2E);
	explosives->setOpacity(0);
	whiteLine = DrawNode::create();
	//mDictionary->retain();
	mDispatcher = mDirector->getEventDispatcher();
	mWinHeight = mDirector->getWinSize().height;
	mWinWidth = mDirector->getWinSize().width;
	//texture
	initResourceTexture();
	initUnitTexture();
	//making cancel texture
	mMakingCancelTexture = mDirector->getTextureCache()->addImage("number/icon_close_circle.png");
	//srand
	srand(static_cast<unsigned>(time(NULL)));
	//gamemode
	mGameMode = static_cast<GameModeEnum>(mUserDefault->getIntegerForKey("gamemode"));
	CCLOG("gameMode: %d", mGameMode);

	//tiledMapLayer
	mTiledMapLayer = TiledMapLayer::create();
	mTiledMapLayer->addChild(ball, 10);
	mTiledMapLayer->addChild(explosives, 10);
	mTiledMapLayer->addChild(whiteLine, 10);
	//init MapSize
	mMapSize = mTiledMapLayer->getMapSize();
	addChild(mTiledMapLayer, 1);
	//startMap
	mStarMap = Sprite::create("uiComponent/starmap.jpg");
	mStarMap->setPosition(mWinWidth / 2, mWinHeight / 2);
	addChild(mStarMap, -1);
	//menu
	initGameMenu();
	auto startMenu = Menu::create(MenuItemLabel::create(
		[&]()->Label*{
		auto label = Label::create(getDicValue("menu"), mFonts, 24);
		label->setColor(Color3B(0, 0, 0));
		return label;
	}(),
		[&](Ref * sender)->void{
		if (!mGameMenu->isVisible())
		{
			mGameMenu->setVisible(true);
		}
		else
		{
			mGameMenu->setVisible(false);
		}
	}), NULL);
	startMenu->setPosition(startMenu->getPosition().x - mWinWidth / 2 + 40, startMenu->getPosition().y + mWinHeight / 2 - 20);
	addChild(startMenu, 5);
	//timer
	mTimer = GameTimer::create();
	mTimer->setPosition(0, -mWinHeight / 2);
	mTimer->setMaxTime(60);
	mTimer->setEndName(getDicValue("endTurn"));
	mTimer->setFonts(mFonts);
	addChild(mTimer, 5);
	//techtreelayer
	mTechTreeLayer = TechTreeLayer::create();
	mTechTreeLayer->setVisible(false);
	addChild(mTechTreeLayer, 2);
	//InfoMapLayer
	mInfoMapLayer = InfoMapLayer::create();
	mInfoMapLayer->setFonts(mFonts);
	mInfoMapLayer->setBgColor(mBgColor);
	float miniPS = 8;
	mInfoMapLayer->setPointSize(miniPS);
	mInfoMapLayer->setMapSize(mTiledMapLayer->getMapSize().width, mTiledMapLayer->getMapSize().height);
	mInfoMapLayer->setPosition(Vec2((-mWinWidth
		+ mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS) / 2,
		(-mWinHeight + mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS) / 2));
	addChild(mInfoMapLayer, 5);
	//miniMapLayer
	mMiniMapLayer = MiniMapLayer::create();
	mMiniMapLayer->setShape(mShape);
	mMiniMapLayer->setBgColor(mBgColor);
	mMiniMapLayer->setPointSize(miniPS);
	mMiniMapLayer->setMapSize(mTiledMapLayer->getMapSize().width, mTiledMapLayer->getMapSize().height);
	mMiniMapLayer->setPosition(Vec2((mWinWidth
		- mTiledMapLayer->getMapSize().width * miniPS
		- mWinHeight / mTiledMapLayer->getTileSize().width * miniPS) / 2,
		(-mWinHeight + mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS) / 2));
	addChild(mMiniMapLayer, 5);

	initGreyBar(miniPS, mBarColor);

	//turn label
	mTurnLabel = Label::createWithTTF(getDicValue("Going") + " 0 " + getDicValue("Turn"), mFonts, 24);
	mTurnLabel->setColor(Color3B(0, 0, 0));
	mTurnLabel->setPosition(mWinWidth - 120, mWinHeight - mTurnLabel->getContentSize().height / 2 - 5);
	addChild(mTurnLabel, 8);
	//resources icon
	initResourcesIcons();
	//unitcamplayer
	mUnitCampLayer = UnitCampLayer::create();
	//mUnitCampLayer->setPosition(0, 50);
	mUnitCampLayer->setFonts(mFonts);
	mUnitCampLayer->setUnlocked(farmer, true);
	/*
	mUnitCampLayer->setUnlocked(shortrangeunit1,true);
	mUnitCampLayer->setUnlocked(shortrangeunit2,true);
	mUnitCampLayer->setUnlocked(longrangeunit1,true);
	mUnitCampLayer->setUnlocked(longrangeunit2,true);
	mUnitCampLayer->setUnlocked(longrangeunit3,true);
	*/
	mUnitCampLayer->setVisible(false);
	addChild(mUnitCampLayer, 2);
	//2 botton
	mTechTreeLayerButtonTexture = {
		mDirector->getTextureCache()->addImage("uiComponent/icon_researchtab_black.png"),
		mDirector->getTextureCache()->addImage("uiComponent/icon_researchtab_white.png")
	};
	mTechTreeLayerButton = Sprite::createWithTexture(mTechTreeLayerButtonTexture.off);
	mTechTreeLayerButton->setPosition(mWinWidth / 2 - 150,
		(mTechTreeLayerButton->getBoundingBox().getMaxY()
		- mTechTreeLayerButton->getBoundingBox().getMinY()) / 2 - 3);
	mTechTreeLayerButton->setScale(0.9);
	addChild(mTechTreeLayerButton, 8);

	mUnitCampLayerButtonTexture = {
		mDirector->getTextureCache()->addImage("uiComponent/icon_militarycamp_black.png"),
		mDirector->getTextureCache()->addImage("uiComponent/icon_militarycamp_white.png")
	};
	mUnitCampLayerButton = Sprite::createWithTexture(mUnitCampLayerButtonTexture.off);
	mUnitCampLayerButton->setPosition(mWinWidth / 2 + 150,
		(mUnitCampLayerButton->getBoundingBox().getMaxY()
		- mUnitCampLayerButton->getBoundingBox().getMinY()) / 2 - 3);
	mUnitCampLayerButton->setScale(0.9);
	addChild(mUnitCampLayerButton, 8);
	//2 working button
	mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(techroot);
	mTechMakingButton = Sprite::createWithTexture(mTechMakingButtonTexture);
	mTechMakingButton->setScale(0.8);
	mTechMakingButton->setPosition(mTechTreeLayerButton->getPosition().x
		- 2 * (mTechMakingButton->boundingBox().getMaxX()
		- mTechMakingButton->boundingBox().getMinX()),
		mTechTreeLayerButton->getPosition().y);
	mTechMakingButton->setVisible(false);
	addChild(mTechMakingButton, 8);
	mUnitMakingButtonTexture = mUnitCampLayer->getUnitTexture(farmer);
	mUnitMakingButton = Sprite::createWithTexture(mUnitMakingButtonTexture);
	mUnitMakingButton->setScale(0.8);
	mUnitMakingButton->setPosition(mUnitCampLayerButton->getPosition().x
		+ 2 * (mUnitMakingButton->boundingBox().getMaxX()
		- mUnitMakingButton->boundingBox().getMinX()),
		mUnitCampLayerButton->getPosition().y);
	mUnitMakingButton->setVisible(false);
	addChild(mUnitMakingButton, 8);
	//win and fail image
	mWinImage[0] = Sprite::createWithTexture(mDirector->getTextureCache()->addImage("uiComponent/victory_blue.jpg"));
	mWinImage[1] = Sprite::createWithTexture(mDirector->getTextureCache()->addImage("uiComponent/victory_red.jpg"));
	mFailImage[0] = Sprite::createWithTexture(mDirector->getTextureCache()->addImage("uiComponent/fail_blue.jpg"));
	mFailImage[1] = Sprite::createWithTexture(mDirector->getTextureCache()->addImage("uiComponent/fail_red.jpg"));
	mWinImage[0]->setPosition(mWinWidth / 2, mWinHeight / 2);
	mWinImage[1]->setPosition(mWinWidth / 2, mWinHeight / 2);
	mFailImage[0]->setPosition(mWinWidth / 2, mWinHeight / 2);
	mFailImage[1]->setPosition(mWinWidth / 2, mWinHeight / 2);
	mWinImage[0]->setVisible(false);
	mWinImage[1]->setVisible(false);
	mFailImage[0]->setVisible(false);
	mFailImage[1]->setVisible(false);
	addChild(mWinImage[0], 12);
	addChild(mWinImage[1], 12);
	addChild(mFailImage[0], 12);
	addChild(mFailImage[1], 12);
	mWinFlag = false;

	if (mGameMode == vsPlayer)
	{
		mWelcomeLayerDisplay = false;
		startGame();
	}
	else if (mGameMode == server || mGameMode == client)
	{
		mWelcomeLayerDisplay = true;
		initWelcomeLayer();
	}
	initYypNet();
	//scheduleUpdate();
	//listeners
	mMouseListener = EventListenerMouse::create();
	mMouseListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMoved, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(GameScene::mMouseListener, this);
	mTouchListener = EventListenerTouchOneByOne::create();
	mTouchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	mTouchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
	mTouchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(GameScene::mTouchListener, this);
	mKeyboardListener = EventListenerKeyboard::create();
	mKeyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	mKeyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(mKeyboardListener, this);

	return true;
}

void GameScene::initGreyBar(float miniPS, Color4F color)
{
	//gray bar
	//Bottom bar
	mGrayBar = DrawNode::create();
	mGrayBar->drawSolidRect(Vec2(0, 0), Vec2(mWinWidth, 50), color);
	mGrayBarRect.push_back(Rect(0, 0, mWinWidth, 50));

	//TopBar
	mGrayBar->drawSolidRect(Vec2(0, mWinHeight - 45), Vec2(mWinWidth, mWinHeight), color);
	mGrayBarRect.push_back(Rect(0, mWinHeight - 45, mWinWidth, 45));

	//InfoMap
	mGrayBar->drawSolidRect(Vec2(-mWinWidth
		+ mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS, 50),
		Vec2(mTiledMapLayer->getMapSize().width * miniPS + 45,
		50 + mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS - 50),
		color);
	mGrayBarRect.push_back(Rect(-mWinWidth
		+ mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS
		, 50, mTiledMapLayer->getMapSize().width * miniPS + 50,
		mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS - 50));

	//MiniMap
	mGrayBar->drawSolidRect(Vec2(mWinWidth
		- mTiledMapLayer->getMapSize().width * miniPS
		- mWinHeight / mTiledMapLayer->getTileSize().width * miniPS, 50),
		Vec2(mWinWidth,
		50 + mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS - 50),
		color);
	mGrayBarRect.push_back(Rect(mWinWidth
		- mTiledMapLayer->getMapSize().width * miniPS
		- mWinHeight / mTiledMapLayer->getTileSize().width * miniPS, 50,
		mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS, mTiledMapLayer->getMapSize().width * miniPS
		+ mWinHeight / mTiledMapLayer->getTileSize().width * miniPS - 50));

	addChild(mGrayBar, 3);
}

//for server, waiting client
void GameScene::acceptConnect(float delta)
{
	//listen
	CCLOG("lisening...");
	if (!mNet.acceptConnect())
	{
		int r = WSAGetLastError();
		if (r == WSAEWOULDBLOCK)
		{
			CCLOG("no client connected");
		}
		else
		{
			CCLOG("error!");
		}
	}
	else
	{
		CCLOG("accept succeed");
		startGame();
		unschedule(schedule_selector(GameScene::acceptConnect));
	}
}

//for client, make connecting
void GameScene::startConnecting(float delta)
{
	CCLOG("connecting...");
	//CCLOG("ip: %s,  port:  %s", (char *)mUserDefault->getStringForKey("ip").c_str(), mUserDefault->getIntegerForKey("port"));
	if (mNet.makeConnect())
	{
		CCLOG("connecting successed");
		startGame();
		unschedule(schedule_selector(GameScene::startConnecting));
		CCLOG("unscheduled connect");
	}
	else
	{
		int r = WSAGetLastError();
		if (r == WSAEISCONN)
		{
			startGame();
			unschedule(schedule_selector(GameScene::startConnecting));
			CCLOG("unscheduled connect");
		}
	}
}

void GameScene::netUpdate(float delta)
{
	//CCLOG("net update");
	//test
	int tF = mGameMode == server ? 0 : 1;
	if (mNet.read())
	{
		CCLOG("read!");
		//read something
		auto which = mNet.getWhich();
		CCLOG("which : %d", which);
		if (which == newTech)
		{
			CCLOG("new tech");
			while (!mNet.sendTech(mNet.getTech()));
			CCLOG("sended back");
			unlockTechTree(1 - tF, mNet.getTech());
		}
		else if (which == newSoldier)
		{
			CCLOG("new unit");
			//place soldier to enemy's spawn
			while (!mNet.sendNewSoldier(mNet.getNewSoldier()));
			CCLOG("sended back");
			spawnUnit(mNet.getNewSoldier().unit, 1 - tF);
		}
		else if (which == twoPoints)
		{
			CCLOG("2 p");
			while (!mNet.sendTwoPoint(mNet.getPoints()));
			CCLOG("sended back");
			readTwoPoint(tF);
		}
		else if (which == end)
		{
			CCLOG("end");
			//while (mNet.sendEnd());
			switchTurn();
		}
		else if (which == youwin)
		{
			while (!mNet.sendYouWin());
			CCLOG("sended back");
			CCLOG("i win");
			win(tF);
		}

	}
	else
	{
		auto err = WSAGetLastError();
		//CCLOG("net err: %d", err);
		if (err != WSAEWOULDBLOCK)
		{
			CCLOG("he GGed!!!");
		}
	}
}

//tF reading
void GameScene::readTwoPoint(const int & tF)
{
	auto twoPoint = mNet.getPoints();
	//
	bool attack = false;
	for (auto & i : mGameState[tF].unitMap)
	{
		if (i.first == twoPoint.second)
		{
			//attack my unit
			CCLOG("read 2 p: attack my unit");
			attack = true;
			break;
		}
	}
	//
	for (auto & i : mResourceMap)
	{
		if (i.first == twoPoint.second)
		{
			//attack resource
			CCLOG("read 2 p: attack resources or base");
			attack = true;
			break;
		}
	}
	if (attack)
	{
		attackUnit(twoPoint.first, twoPoint.second, 1 - tF);
	}
	else
	{
		//move
		std::set <MyPointStruct> barrier;// need to repair
		for (auto state : mGameState)
		{
			for (auto ob : state.unitMap)
			{
				barrier.insert(ob.first);
			}
		}
		for (auto ob : mResourceMap)
		{
			if (ob.second.type == base)
			{
				auto nearPoint = getNearPoint(ob.first);
				for (auto point : nearPoint)
				{
					barrier.insert(point);
				}
			}
			barrier.insert(ob.first);
		}
		auto unit = mGameState[1 - tF].unitMap[twoPoint.first];
		auto pathTree = getPathTree(twoPoint.first, unit.property.numRangeMove, barrier);
		auto path = getPath(pathTree, twoPoint.second);
		//debug
		CCLOG("path:  ");
		for (const auto & pN : path)
		{
			CCLOG("node: %d,%d", pN.x, pN.y);
		}
		moveUnit(path, 1 - tF);
	}
}

//--update
void GameScene::update(float delta)
{
	//move
	Vec2 mapP = mTiledMapLayer->getPosition();
	//CCLOG("mapP %f,%f", mapP.x, mapP.y);
	Size mapS = mTiledMapLayer->getMapSizeF();
	//CCLOG("mapS %f,%f", mapS.width, mapS.height);
	if (mKeyStruct.w && (mapP.y >= mWinHeight - mapS.height - mWinHeight / 2 + moveDis /* - mapS.height + moveDis - 50*/))
	{
		mTiledMapLayer->setPosition(mapP.x, mapP.y - moveDis);
		mapP = mTiledMapLayer->getPosition();
	}
	if (mKeyStruct.s && (mapP.y <= 0 + mWinHeight / 2 - moveDis /* - moveDis + 280*/))
	{
		mTiledMapLayer->setPosition(mapP.x, mapP.y + moveDis);
		mapP = mTiledMapLayer->getPosition();
	}
	if (mKeyStruct.a && (mapP.x <= 0 + mWinHeight / 2 - moveDis/* - moveDis*/))
	{
		mTiledMapLayer->setPosition(mapP.x + moveDis, mapP.y);
		mapP = mTiledMapLayer->getPosition();
	}
	if (mKeyStruct.d && (mapP.x >= mWinWidth - mapS.width - mWinHeight / 2 + moveDis/* - mapS.width + moveDis*/))
	{
		mTiledMapLayer->setPosition(mapP.x - moveDis, mapP.y);
		mapP = mTiledMapLayer->getPosition();
	}
	Vec2 position = Vec2((mWinWidth / 2 - mTiledMapLayer->getPosition().x) / mTiledMapLayer->getMapSizeF().width, (mWinHeight / 2 - mTiledMapLayer->getPosition().y) / mTiledMapLayer->getMapSizeF().height);
	//setViewPosition
	mMiniMapLayer->setViewPosition(position);

	//timer
	if (!mTimer->isEnded())
	{
		mTimer->refresh(delta);
	}
	else
	{
		//ended
		if (!mUpdateTimerLock)
		{
			//first time to end
			mUpdateTimerLock = true;
			playEffect(ELECTROSWITCH);
			switchTurn();
		}
	}
	auto p0 = Vec2(
		mTiledMapLayer->getPosition().x/* * starMovePerMove*/ + (mTiledMapLayer->getMapSizeF().width + mWinWidth) / 2,
		mTiledMapLayer->getPosition().y/* * starMovePerMove*/ + (mTiledMapLayer->getMapSizeF().height + mWinHeight) / 2
		);
	mStarMap->setPosition(p0.x * starMovePerMove, p0.y * starMovePerMove);
	if (mPopulation[0] > 60 || mPopulation[1] > 60)
	{
		if (isPlayingIntensive == false)
		{
			SimpleAudioEngine::getInstance()->stopBackgroundMusic();
			playBackgroundMusic(BG_MUSIC2);
			isPlayingIntensive = true;
		}
	}
	else if (isPlayingIntensive == true)
	{
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		playBackgroundMusic(BG_MUSIC);
		isPlayingIntensive = false;
	}
}

//--switchTurn
void GameScene::switchTurn()
{
	surpriseEgg();
	//end turn
	mBlueTurn = !mBlueTurn;
	addMist();
	CCLOG("now turn: %d", mBlueTurn);
	mUnitCampLayer->setVisible(false);
	mTechTreeLayer->setVisible(false);
	//refresh NumTurn
	if (mBlueTurn)
	{
		++mNumTurn;
		//CCLOG("going : %s", getDicValue("Going").c_str());
		//CCLOG("turn : %s", getDicValue("Turn").c_str());
		mTurnLabel->setString(getDicValue("Going") + " "
			+ std::to_string(mNumTurn) + " " + getDicValue("Turn"));
	}
	//CCLOG("changed turn now %s", mBlueTurn ? "blue" : "red");
	//start turn
	if (mGameMode == vsPlayer)
	{
		mOperateEnable = true;
		mUpdateTimerLock = false;
		int tF = mBlueTurn ? 0 : 1;
		//CCLOG("vsPlayer,tF: %d", tF);
		checkTechFactory(tF);
		checkUnitFactory(tF);
		refreshTechTreeLayer(tF);
		refreshUnitCamp(tF);
		refreshResource(tF);
		refreshResourcesIcons(tF);
		refreshMakingButton(tF);
		refreshUnitState(tF);
		refreshPopulationIcons(tF);
		refreshMiniMap();
		mTimer->start();
		mTimer->setTimerColor(tF);
		//refresh 2 layer display from gamestate
		//start a new turn
	}
	else if (mGameMode == server || mGameMode == client)
	{
		//set OE
		int tF = -1;
		if (mGameMode == server)
		{
			mOperateEnable = mBlueTurn;
			tF = 0;
		}
		else if (mGameMode == client)
		{
			mOperateEnable = !mBlueTurn;
			tF = 1;
		}
		CCLOG("in switch Turn tF = %d", tF);
		if (mBlueTurn)
		{
			refreshResource(0);
		}
		else
		{
			refreshResource(1);
		}

		//timer
		if (mOperateEnable)
		{
			CCLOG("OE true");
			refreshResourcesIcons(tF);
			refreshMakingButton(tF);
			refreshUnitState(tF);
			refreshMiniMap();
			checkTechFactory(tF);
			checkUnitFactory(tF);
			refreshTechTreeLayer(tF);
			refreshUnitCamp(tF);
			refreshPopulationIcons(tF);
			mTimer->start();
			mTimer->setTimerColor(tF);
			mUpdateTimerLock = false;
		}
		else
		{
			CCLOG("OE false");
			mTimer->shutDown();
			//send end
			while (!mNet.sendEnd())
			{
				auto err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					//mDirector->popScene();
				}
			}
		}
	}
	mTechTreeLayerButton->setTexture(mTechTreeLayerButtonTexture.off);
	mUnitCampLayerButton->setTexture(mUnitCampLayerButtonTexture.off);
	refreshResourcesTexture();
	checkLayersOnMouseMoved();
	deleteAttackRange();
	deleteMoveRange();
	mUnitActionFSM[0] = mUnitActionFSM[1] = 0;


}

void GameScene::refreshUnitState(const int & turnFlag)
{
	for (auto & i : mGameState[turnFlag].unitMap)
	{
		i.second.state = fresh;
	}
}

void GameScene::checkTechFactory(int turnFlag)
{
	mTechFactory[turnFlag].refresh(mResources[turnFlag].numResearchLevel);
	if (mTechFactory[turnFlag].finished())
	{
		//setInfluence
		auto newTech = mTechFactory[turnFlag].getFinishedTech();
		unlockTechTree(turnFlag, newTech);
		mTechFactory[turnFlag].setExistence(false);
		//display
		refreshMakingButton(turnFlag);
		//send
		if (mGameMode == server || mGameMode == client)
		{
			while (!mNet.sendTech(newTech))
			{
				auto err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					//mDirector->popScene();
				}
			}
			while (!mNet.read()){}
			CCLOG("read send back!");
		}
	}
}

void GameScene::checkUnitFactory(int turnFlag)
{
	if (!mUnitFactory[turnFlag].finished())
	{
		mUnitFactory[turnFlag].refresh(mResources[turnFlag].numProductivity);
	}
	if (mUnitFactory[turnFlag].finished() && (!spawnOccupied(turnFlag)))
	{
		//getUnit
		auto newUnit = mUnitFactory[turnFlag].getFinishedUnit();
		//spawn it 
		spawnUnit(newUnit, turnFlag);
		if (newUnit == longrangeunit3)
		{
			playEffect(FOLLOWME);
		}
		else
		{
			playEffect(IMREADY);
		}
		refreshMiniMap();
		//add population
		mPopulation[turnFlag] += (mUnitInitDataMap[newUnit].property.numPopulation
			+ mGameState[turnFlag].extraProperty[newUnit].numPopulation);
		mUnitFactory[turnFlag].setExistence(false);
		//display
		refreshPopulationIcons(turnFlag);
		refreshMakingButton(turnFlag);
		//set Existence
		mUnitFactory[turnFlag].setExistence(false);
		CCLOG("finished! newUnit: %d", newUnit);
		//CCLOG("finished! newUnit: %d", newUnit);
		//send
		if (mGameMode == server || mGameMode == client)
		{
			while (!mNet.sendNewSoldier(newSoldierStruct{ newUnit, mSpawn[turnFlag] }))
			{
				auto err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					//mDirector->popScene();
				}
			}
			while (!mNet.read()){}
			CCLOG("read send back!");
		}
	}
}

void GameScene::moveUnit(std::vector<MyPointStruct> path, int turnFlag, bool showAttachRange)
{
	if (path.size() < 2)
	{
		CCLOG("invalid path");
		system("pause");
		return;
	}
	bool oeBak = mOperateEnable;
	mOperateEnable = false;
	const float duration = 0.2;
	//get unit sprite;
	Unit unit;
	UnitEnum type;
	bool found = false;
	for (auto & i : mGameState[turnFlag].unitMap)
	{
		if (i.first == path[0])
		{
			unit = i.second;
			type = unit.type;
			found = true;
			break;
		}
	}
	//error
	if (!found)
	{
		CCLOG("error, unit not found!");
		return;
	}
	//sequence vector
	Vector<FiniteTimeAction*> actionVector;
	auto funcUp = CallFunc::create([unit, type, turnFlag, this]()->void{ unit.sprite->setTexture(mUnitTextureMap[turnFlag][type].back); });
	auto funcDown = CallFunc::create([unit, type, turnFlag, this]()->void{unit.sprite->setTexture(mUnitTextureMap[turnFlag][type].front); });
	auto funcLeft = CallFunc::create([unit, type, turnFlag, this]()->void{unit.sprite->setTexture(mUnitTextureMap[turnFlag][type].side); unit.sprite->setFlippedX(true); });
	auto funcRight = CallFunc::create([unit, type, turnFlag, this]()->void{unit.sprite->setTexture(mUnitTextureMap[turnFlag][type].side); unit.sprite->setFlippedX(false); });

	//last action
	//0: none, 1: up, 2: down, 3: left, 4: right
	short lastAction = 0;
	for (int i = 0; i < path.size() - 1; ++i)
	{
		auto & thisP = path[i];
		auto & nextP = path[i + 1];
		auto nextPF = mTiledMapLayer->floatNodeCoorForPosition(nextP);
		//check direction
		if ((thisP.x == nextP.x) && (thisP.y > nextP.y))
		{
			//move up
			if (lastAction != 1)
			{
				actionVector.pushBack(funcUp);
				lastAction = 1;
			}
		}
		else if ((thisP.x == nextP.x) && (thisP.y < nextP.y))
		{
			//move down
			if (lastAction != 2)
			{
				actionVector.pushBack(funcDown);
				lastAction = 2;
			}
		}
		else if ((thisP.y == nextP.y) && (thisP.x > nextP.x))
		{
			//move left
			if (lastAction != 3)
			{
				actionVector.pushBack(funcLeft);
				lastAction = 3;
			}
			//unit->setFlipX(true);
		}
		else if ((thisP.y == nextP.y) && (thisP.x < nextP.x))
		{
			//move right
			if (lastAction != 4)
			{
				actionVector.pushBack(funcRight);
				lastAction = 4;
			}
			///unit->setFlippedX(false);
		}
		actionVector.pushBack(MoveTo::create(duration, nextPF));
	}
	//sequence
	auto unLockOEC = CallFunc::create([this, oeBak]()->void{mOperateEnable = oeBak; });
	auto refreshUnitMapC = CallFunc::create(
		[this, turnFlag, path, unit]()->void{
		//CCLOG("path test: %d,%d", path[0].x, path[0].y);
		mGameState[turnFlag].unitMap.erase(path[0]);
		mGameState[turnFlag].unitMap[path[path.size() - 1]] = unit;
		addMist();
	}
	);
	auto showAttackRangeC = CallFunc::create(
		[this, path, turnFlag]()->void{
		showAttackRange(path[path.size() - 1], turnFlag);
		if (mAttackRange.empty())
		{
			deleteAttackRange();
			mUnitActionFSM[turnFlag] = 0;
		}
		else
		{
			mUnitActionFSM[turnFlag] = 2;
		}
	}
	);
	auto refreshMiniMapC = CallFunc::create(CC_CALLBACK_0(GameScene::refreshMiniMap, this));
	actionVector.pushBack(refreshUnitMapC);
	actionVector.pushBack(unLockOEC);
	actionVector.pushBack(refreshMiniMapC);
	if (showAttachRange)
	{
		actionVector.pushBack(showAttackRangeC);
	}
	auto sequence = Sequence::create(actionVector);
	//sequence->retain();
	unit.sprite->runAction(sequence);
}

//--attackUnit
//from->tF
void GameScene::attackUnit(const MyPointStruct & from, const MyPointStruct & attackedUnitPosition, const int & tF)
{
	auto typeFrom = mGameState[tF].unitMap[from].type;
	//check direction
	int rightDis = attackedUnitPosition.x - from.x;
	bool right = rightDis > 0;
	int upDis = attackedUnitPosition.y - from.y;
	bool up = upDis < 0;
	int hDis = abs(rightDis);
	int vDis = abs(upDis);
	//change direction
	if (right && (hDis >= vDis))
	{
		mGameState[tF].unitMap[from].sprite->setTexture(mUnitTextureMap[tF][typeFrom].side);
		mGameState[tF].unitMap[from].sprite->setFlippedX(false);
	}
	else if ((!right) && (hDis >= vDis))
	{
		mGameState[tF].unitMap[from].sprite->setTexture(mUnitTextureMap[tF][typeFrom].side);
		mGameState[tF].unitMap[from].sprite->setFlippedX(true);
	}
	else if (up && (hDis <= vDis))
	{
		mGameState[tF].unitMap[from].sprite->setTexture(mUnitTextureMap[tF][typeFrom].back);
		mGameState[tF].unitMap[from].sprite->setFlippedX(false);
	}
	else if ((!up) && (hDis <= vDis))
	{
		mGameState[tF].unitMap[from].sprite->setTexture(mUnitTextureMap[tF][typeFrom].front);
		mGameState[tF].unitMap[from].sprite->setFlippedX(false);
	}
	//check attackedUnitPosition type
	for (auto & reP : mResourceMap)
	{
		if ((reP.first == attackedUnitPosition) && (reP.second.type != base))
		{
			//attack resource
			collectionFamerAttack(from, attackedUnitPosition, tF);
			return;
		}
	}
	//animate
	bool oeBak = mOperateEnable;
	mOperateEnable = false;

	Vec2 fromP0 = Vec2(mTiledMapLayer->floatNodeCoorForPosition(from).x, mTiledMapLayer->floatNodeCoorForPosition(from).y);
	Vec2 toP = mTiledMapLayer->floatNodeCoorForPosition(attackedUnitPosition);
	//ball->setOpacity(0);
	ball->setPosition(fromP0);
	explosives->setPosition(toP);

	//Actions
	//moveball and blink
	auto moveBall = MoveTo::create(0.3, mTiledMapLayer->floatNodeCoorForPosition(attackedUnitPosition));

	//explore
	auto Explode = Sequence::create(
		Spawn::create(
		Sequence::create(
		DelayTime::create(0.2),
		FadeOut::create(0.3),
		NULL
		),
		ScaleTo::create(0.5, 2.5),
		NULL
		),
		NULL
		);
	Explode->retain();
	//move
	float moveDisX = mTiledMapLayer->getTileSize().width;
	CCLOG("moveDisX: %f", moveDisX);
	float moveDisY = mTiledMapLayer->getTileSize().height;
	CCLOG("moveDisY: %f", moveDisY);
	auto attackUp = Sequence::create(
		MoveBy::create(0.3, Vec2(0, -moveDisY)),
		MoveBy::create(0.1, Vec2(0, 1.5 * moveDisY)),
		MoveBy::create(0.1, Vec2(0, -0.5 * moveDisY)),
		NULL
		);
	auto attackDown = Sequence::create(
		MoveBy::create(0.3, Vec2(0, moveDisY)),
		MoveBy::create(0.1, Vec2(0, -1.5*moveDisY)),
		MoveBy::create(0.1, Vec2(0, 0.5*moveDisY)),
		NULL
		);
	auto attackLeft = Sequence::create(
		MoveBy::create(0.3, Vec2(moveDisX, 0)),
		MoveBy::create(0.1, Vec2(-1.5*moveDisX, 0)),
		MoveBy::create(0.1, Vec2(0.5*moveDisX, 0)),
		NULL
		);
	auto attackRight = Sequence::create(
		MoveBy::create(0.3, Vec2(-moveDisX, 0)),
		MoveBy::create(0.1, Vec2(1.5*moveDisX, 0)),
		MoveBy::create(0.1, Vec2(-0.5*moveDisX, 0)),
		NULL
		);
	//callFuncs
	//callthis after
	auto afterFunc = CallFunc::create([this, tF, from, typeFrom, attackedUnitPosition]()->void
	{
		CCLOG("after Func called");
		bool attackedBase = false;
		auto baseNearing = getNearPoint(mBasePosition[1 - tF]);
		for (const auto & i : baseNearing)
		{
			if (attackedUnitPosition == i)
			{
				attackedBase = true;
			}
		}
		//base
		if (attackedUnitPosition == mBasePosition[1 - tF])
		{
			attackedBase = true;
		}
		if (attackedBase)
		{
			auto & HP = mResourceMap[mBasePosition[1 - tF]].property.numHitPoint;
			auto deltaHP = abs(mGameState[tF].unitMap[from].property.numAttack + mGameState[tF].extraProperty[typeFrom].numAttack - mResourceMap[mBasePosition[1 - tF]].property.numDefence);
			if (typeFrom == longrangeunit3)
			{
				HP -= 10 * deltaHP;
			}
			else
			{
				HP -= deltaHP;
			}
			//die?
			if (HP <= 0)
			{
				die(mBasePosition[1 - tF], 1 - tF);
			}
		}
		else
		{
			auto typeTo = mGameState[1 - tF].unitMap[attackedUnitPosition].type;
			//unit
			auto & HP = mGameState[1 - tF].unitMap[attackedUnitPosition].property.numHitPoint;
			CCLOG("unit a unit: HP: %d", HP);
			auto atk = mGameState[tF].unitMap[from].property.numAttack;
			auto atkE = mGameState[tF].extraProperty[typeFrom].numAttack;
			auto def = mGameState[1 - tF].unitMap[attackedUnitPosition].property.numDefence;
			auto defE = mGameState[1 - tF].extraProperty[typeTo].numDefence;
			CCLOG("atk = %d + %d;  def = %d + %d", atk, atkE, def, defE);
			HP -= abs(atk + atkE - def - defE);
			CCLOG("aftar a, HP: %d", HP);
			if (HP <= 0)
			{
				die(attackedUnitPosition, 1 - tF);
			}
		}
	});
	afterFunc->retain();
	auto unlockOE = CallFunc::create(
		[this, oeBak]()->void{
		CCLOG("unlockOE called");
		mOperateEnable = oeBak;
	}
	);
	unlockOE->retain();


	auto runExplode = CallFunc::create(
		[this, Explode, afterFunc, unlockOE]()->void
	{
		CCLOG("runing explode!");
		explosives->setOpacity(255);
		explosives->setScale(0.1);
		explosives->runAction(
			Sequence::create(
			Explode,
			afterFunc,
			unlockOE,
			NULL)
			);
	}
	);
	//switch
	if (typeFrom == shortrangeunit2 || typeFrom == longrangeunit1 || typeFrom == longrangeunit2)
	{
		if (typeFrom == shortrangeunit2)
		{
			playEffect(BONG);
			ball->setTexture(mAttackTexture.SR2T);
			explosives->setTexture(mAttackTexture.SR2E);
		}
		else if (typeFrom == longrangeunit1)
		{
			playEffect(BZZZ);
			ball->setTexture(mAttackTexture.LR1T);
			explosives->setTexture(mAttackTexture.LR1E);
		}
		else if (typeFrom == longrangeunit2)
		{
			playEffect(BWUBWU);
			ball->setTexture(mAttackTexture.LR2T);
			explosives->setTexture(mAttackTexture.LR2E);
		}
		ball->setOpacity(255);
		ball->runAction(
			Sequence::create(
			moveBall,
			CallFunc::create([this]()->void{ball->setOpacity(0); }),
			runExplode,
			NULL
			)
			);
	}
	else if (typeFrom == shortrangeunit1 || typeFrom == farmer)
	{
		playEffect(BWANG);
		Vector<FiniteTimeAction*> hitAnimation;
		//sr1, move
		if (right && (hDis > vDis))
		{
			//right
			hitAnimation.pushBack(attackRight);
		}
		else if ((!right) && (hDis > vDis))
		{
			hitAnimation.pushBack(attackLeft);
		}
		else if (up && (hDis < vDis))
		{
			hitAnimation.pushBack(attackUp);
		}
		else if ((!up) && (hDis < vDis))
		{
			hitAnimation.pushBack(attackDown);
		}
		hitAnimation.pushBack(afterFunc);
		hitAnimation.pushBack(unlockOE);
		mGameState[tF].unitMap[from].sprite->runAction(
			Sequence::create(hitAnimation)
			);

	}
	else if (typeFrom == longrangeunit3)
	{
		//whiteLine->clear();
		playEffect(BZIIU);
		Vec2 fromP = Vec2(fromP0.x + 64 * (toP.x - fromP0.x) / abs(toP.x - fromP0.x + 0.1), fromP0.y + 64 * (toP.y - fromP0.y) / abs(toP.y - fromP0.y + 0.1));
		float drawCount = 200;
		float drawDeltaX = (toP.x - fromP.x) / drawCount;
		float drawDeltaY = (toP.y - fromP.y) / drawCount;
		float x = fromP.x;
		float y = fromP.y;
		while (abs(x - toP.x) > 1 || abs(y - toP.y) > 1)
		{
			whiteLine->drawSolidCircle(Vec2(x, y), 10, 10, 10, Color4F(1, 1, 1, 1));
			if ((int)x % 2 < 1)
			{
				whiteLine->drawSolidCircle(Vec2(x + (CCRANDOM_0_1() - 0.5) * 12, y + (CCRANDOM_0_1() - 0.5) * 12), 8, 10, 10, Color4F(1, 1, 1, 0.9));
			}
			if ((int)x % 4 < 2)
			{
				whiteLine->drawSolidCircle(Vec2(x + (CCRANDOM_0_1() - 0.5) * 64, y + (CCRANDOM_0_1() - 0.5) * 64), 4, 10, 10, Color4F(1, 1, 1, 0.7));
			}
			if ((int)x % 12 < 2)
			{
				whiteLine->drawSolidCircle(Vec2(x + (CCRANDOM_0_1() - 0.5) * 128, y + (CCRANDOM_0_1() - 0.5) * 128), 4, 10, 10, Color4F(1, 1, 1, 0.4));
			}
			x += drawDeltaX;
			y += drawDeltaY;
		}
		explosives->setTexture(mAttackTexture.LR3E);
		CCLOG("before action!");
		whiteLine->runAction(
			Sequence::create(
			DelayTime::create(0.3),
			CallFunc::create([this]()->void{whiteLine->clear(); }),
			runExplode,
			NULL
			)
			);
	}
}

void GameScene::die(const MyPointStruct & point, const int & tF)
{
	CCLOG("die point: %d,%d", point.x, point.y);
	//base
	if (point == mBasePosition[tF])
	{
		//win(tF);
		win(1 - tF);
		return;
	}
	//unit
	bool foundU = false;
	CCLOG("unitmap size: %d", mGameState[tF].unitMap.size());
	for (auto & i : mGameState[tF].unitMap)
	{
		CCLOG("searching unit map.%d,%d", i.first.x, i.first.y);
		if (i.first == point)
		{
			//found
			foundU = true;
			break;
		}
	}
	if (foundU)
	{
		playEffect(FALLAPART);
		mPopulation[tF] -= mGameState[tF].unitMap[point].property.numPopulation;
		if (mGameMode == server || mGameMode == client)
		{
			int tFT = mGameMode == server ? 0 : 1;
			refreshResourcesIcons(tFT);
		}
		mGameState[tF].unitMap[point].sprite->removeFromParentAndCleanup(true);
		mGameState[tF].unitMap.erase(point);
		return;
	}
	//resources
	bool foundR = false;
	for (auto & i : mResourceMap)
	{
		if (i.first == point)
		{
			foundR = true;
			break;
		}
	}
	if (foundR)
	{
		playEffect(WEAKEXPLOSIVE);
		mResourceMap[point].sprite->removeFromParentAndCleanup(true);
		mResourceMap.erase(point);
		return;
	}
	refreshMiniMap();
}

void GameScene::win(const int & tF)
{
	auto show = Sequence::create(
		FadeIn::create(0.5),
		NULL
		);
	mWinFlag = true;
	mOperateEnable = false;
	if (mGameMode == vsPlayer)
	{
		playEffect(VICTORY);
		mWinImage[tF]->setOpacity(0);
		mWinImage[tF]->setVisible(true);
		mWinImage[tF]->runAction(show);
	}
	else if (mGameMode == client || mGameMode == server)
	{
		int tFN = mGameMode == server ? 0 : 1;
		if (tF == tFN)
		{
			playEffect(VICTORY);
			mWinImage[tF]->setOpacity(0);
			mWinImage[tF]->setVisible(true);
			mWinImage[tF]->runAction(show);
		}
		else
		{
			playEffect(FAIL);
			mFailImage[tF]->setOpacity(0);
			mFailImage[tF]->setVisible(true);
			mFailImage[tF]->runAction(show);
		}
	}
	scheduleOnce(schedule_selector(GameScene::delayAndQuit), 3);
}

void GameScene::delayAndQuit(float delta)
{
	backToMainScene(NULL);
}

void GameScene::spawnUnit(UnitEnum unit, int turnFlag)
{
	auto pro = UnitPropertyStruct(mUnitInitDataMap[unit].property);
	CCLOG("spawning property: {hp:%d,atk:%d,def:%d,movR:%d,atR:%d,po:%d,}", pro.numHitPoint, pro.numAttack, pro.numDefence, pro.numRangeMove, pro.numRangeAttack, pro.numPopulation);
	Unit newUnit = {
		unit,
		UnitPropertyStruct(mUnitInitDataMap[unit].property),
		UnitStateEnum::fresh,
		[&]()->Sprite*
		{
			auto sprite = Sprite::createWithTexture(mUnitTextureMap[turnFlag][unit].front);
			sprite->setPosition(mTiledMapLayer->floatNodeCoorForPosition(mSpawn[turnFlag]));
			mTiledMapLayer->addChild(sprite, 2);
			return sprite;
		}()
	};
	//add to unitMap
	mGameState[turnFlag].unitMap[mSpawn[turnFlag]] = newUnit;
}

void GameScene::spawnUnit(UnitEnum unit, int turnFlag, const MyPointStruct & pos)
{
	auto pro = UnitPropertyStruct(mUnitInitDataMap[unit].property);
	CCLOG("spawning property: {hp:%d,atk:%d,def:%d,movR:%d,atR:%d,po:%d,}", pro.numHitPoint, pro.numAttack, pro.numDefence, pro.numRangeMove, pro.numRangeAttack, pro.numPopulation);
	Unit newUnit = {
		unit,
		UnitPropertyStruct(mUnitInitDataMap[unit].property),
		UnitStateEnum::fresh,
		[&]()->Sprite*
		{
			auto sprite = Sprite::createWithTexture(mUnitTextureMap[turnFlag][unit].front);
			sprite->setPosition(mTiledMapLayer->floatNodeCoorForPosition(pos));
			mTiledMapLayer->addChild(sprite, 2);
			return sprite;
		}()
	};
	//add to unitMap
	mGameState[turnFlag].unitMap[pos] = newUnit;
}

//testing
bool GameScene::spawnOccupied(int turnFlag)
{
	auto & spawn = mSpawn[turnFlag];
	for (const auto & i : mGameState[turnFlag].unitMap)
	{
		if (i.first == spawn)
		{
			return true;
		}
	}
	return false;
}

void GameScene::backToMainScene(Ref * sender)
{
	/*
	if (mGameMode == server)
	{
	mNet.endServer();
	}
	else if (mGameMode == client)
	{
	mNet.deleteConnect();
	}
	*/
	mDirector->popScene();
	//mDirector->popToRootScene();
}

bool GameScene::onTouchBegan(Touch * touch, Event * event)
{
	mMouseCoordinateTouch = mMouseCoordinate;
	mMouseCoordinateP = mMouseCoordinate;
	return true;
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event * event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_W || keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		mKeyStruct.w = true;
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_S || keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
	{
		mKeyStruct.s = true;
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_A || keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		mKeyStruct.a = true;
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_D || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		mKeyStruct.d = true;
	}
	//refresh miniMap

}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_W || keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
	{
		mKeyStruct.w = false;
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_S || keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
	{
		mKeyStruct.s = false;
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_A || keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
	{
		mKeyStruct.a = false;
	}
	if (keyCode == EventKeyboard::KeyCode::KEY_D || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
	{
		mKeyStruct.d = false;
	}
	//open/close gameMenu
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
	{
		if (!mGameMenu->isVisible())
		{
			mGameMenu->setVisible(true);
		}
		else
		{
			mGameMenu->setVisible(false);
		}
	}
}

void GameScene::onTouchMoved(Touch * touch, Event * event)
{
	//do something here
	do
	{
		//minimap
		if (mMiniMapLayer->containPoint(mMouseCoordinate))
		{
			checkMiniMap();
			break;
		}
		else if (mMiniMapLayer->blockClick())
		{
			break;
		}
		//tiledmap
		if (mTiledMapLayer->containPoint(mMouseCoordinate))
		{
			checkTiledMapOnTouchMoved();
		}
		else if (mTiledMapLayer->blockClick())
		{
			break;
		}
	} while (0);

	mMouseCoordinateP = mMouseCoordinate;
}

void GameScene::onTouchEnded(Touch * touch, Event * event)
{
	const int offset = 5;
	if ((abs(mMouseCoordinate.x - mMouseCoordinateTouch.x) <= offset) && (abs(mMouseCoordinate.y - mMouseCoordinateTouch.y) <= offset))
	{
		//win
		if (mWinFlag)
		{
			backToMainScene(NULL);
		}
		checkTechAndUnitButton();
		checkMakingButtonOnTouchEnded();
		do
		{
			//minimap
			if (mMiniMapLayer->containPoint(mMouseCoordinate))
			{
				checkMiniMap();
				break;
			}
			else if (mMiniMapLayer->blockClick())
			{
				break;
			}
			//timer
			if (mTimer->containPoint(mMouseCoordinateTouch))
			{
				CCLOG("timer contain");
				//timer contain bug
				if (mOperateEnable)
				{
					playEffect(ELECTROSWITCH);
					mTimer->shutDown();
					mOperateEnable = false;
				}
				break;
			}
			else if (mTimer->blockClick())
			{
				break;
			}
			//techLayer
			if (mTechTreeLayer->isVisible())
			{
				if (mTechTreeLayer->containPoint(mMouseCoordinateTouch))
				{
					checkTechTreeLayerOnTouchEnded();
					break;
				}
				else if (mTechTreeLayer->blockClick())
				{
					break;
				}
			}
			//unitCamp
			if (mUnitCampLayer->isVisible())
			{
				if (mUnitCampLayer->containPoint(mMouseCoordinateTouch))
				{
					checkUnitCampLayerOnTouchEnded();
					break;
				}
				else if (mUnitCampLayer->blockClick())
				{
					break;
				}
			}
			//yyp mark
			if (mTiledMapLayer->containPoint(mMouseCoordinateTouch))
			{
				int tF = mBlueTurn ? 0 : 1;
				auto mapPoint = mTiledMapLayer->tiledCoorForPostion(mMouseCoordinateTouch);
				CCLOG("tiled coor touched: %d,%d, tF = %d", mapPoint.x, mapPoint.y, tF);
				if (mOperateEnable)
				{
					unitAction(mapPoint, tF);
				}
				break;
			}
			else if (mTiledMapLayer->blockClick())
			{
				break;
			}
		} while (0);
	}
}

void GameScene::unlockTechTree(const int & flag, TechEnum tech)
{
	mGameState[flag].techTree.unlock(tech);
	//CCLOG("ssss: %d", mGameState[flag].techTree.isUnlocked(tech));
	setTechInfluence(flag, tech);
	if (mOperateEnable)
	{
		refreshTechTreeLayer(flag);
	}
}

void GameScene::setTechInfluence(const int & flag, TechEnum tech)
{
	string mType = mTechInitInfluenceMap[tech].type;
	string mTarget = mTechInitInfluenceMap[tech].target;
	int mValue = mTechInitInfluenceMap[tech].value;
	for (const auto & unit : mUnitStringEnumMap)
	{
		if (unit.first == mType)
		{
			mGameState[flag].unitLockMap[mUnitStringEnumMap.at(mType)] = false;
			return;
		}
	}
	if (mTarget != "")
	{
		if (mTarget == "longRange")
		{
			CCLOG("longRangeAttributeAdded");
			if (mType == "numHitPoint")
			{
				mGameState[flag].extraProperty[longrangeunit1].numHitPoint += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numHitPoint += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numHitPoint += mValue;
			}
			if (mType == "numDefence")
			{
				mGameState[flag].extraProperty[longrangeunit1].numDefence += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numDefence += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numDefence += mValue;
			}
			if (mType == "numAttack")
			{
				mGameState[flag].extraProperty[longrangeunit1].numAttack += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numAttack += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numAttack += mValue;
			}
			if (mType == "numAttackRange")
			{
				mGameState[flag].extraProperty[longrangeunit1].numRangeAttack += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numRangeAttack += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numRangeAttack += mValue;
			}
			if (mType == "numMoveRange")
			{
				mGameState[flag].extraProperty[longrangeunit1].numRangeMove += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numRangeMove += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numRangeMove += mValue;
			}
			if (mType == "numPopulation")
			{
				mGameState[flag].extraProperty[longrangeunit1].numPopulation += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numPopulation += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numPopulation += mValue;
			}
		}
		if (mTarget == "shortRange")
		{
			if (mType == "numHitPoint")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numHitPoint += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numHitPoint += mValue;
			}
			if (mType == "numDefence")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numDefence += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numDefence += mValue;
			}
			if (mType == "numAttack")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numAttack += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numAttack += mValue;
			}
			if (mType == "numAttackRange")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numRangeAttack += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numRangeAttack += mValue;
			}
			if (mType == "numMoveRange")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numRangeMove += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numRangeMove += mValue;
			}
			if (mType == "numPopulation")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numPopulation += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numPopulation += mValue;
			}
		}
		if (mTarget == "base")
		{
			if (mType == "numHitPoint")
			{
				mResourceMap[mBasePosition[flag]].property.numHitPoint += mValue;
				mMaxHitPointOfBase[flag].numHitPoint += mValue;
			}
		}
		return;
	}
	else
	{
		if (mType == "numResearchLevel")
		{
			CCLOG("numResearchLevelAdded");
			mResources[flag].numResearchLevel += mValue;
		}
		if (mType == "numProductivity")
		{
			CCLOG("numProductivityAdded");
			mResources[flag].numProductivity += mValue;
		}
		if (mType == "numFixedResource")
		{
			mExtraResources[flag].numFixedResource += mValue;
		}
		if (mType == "numRandomResource")
		{
			mExtraResources[flag].numRandomResource += mValue;
		}
	}
	return;
}

void GameScene::refreshTechTreeLayer(const int & flag)
{

	for (const auto & i : mTechEnumList)
	{
		if (mGameState[flag].techTree.unlockable(i))
		{
			//CCLOG("tech a: %d", i);
			mTechTreeLayer->setTechState(i, available);
		}
		else if (mGameState[flag].techTree.isUnlocked(i))
		{
			//CCLOG("tech un: %d", i);
			mTechTreeLayer->setTechState(i, unlocked);
		}
		else
		{
			mTechTreeLayer->setTechState(i, unavailable);
		}
	}
	//waiting for factory coming soon~
	if (mTechFactory[flag].techExistence())
	{
		auto makingTech = mTechFactory[flag].getMakingTech();
		mTechTreeLayer->setTechState(makingTech, unlocking);
	}
}

void GameScene::refreshUnitCamp(const int & flag)
{
	//refresh resources
	for (const auto & unit : mUnitStringEnumMap)
	{
		auto property = UnitPropertyStruct{
			mUnitInitDataMap[unit.second].property.numHitPoint + mGameState[flag].extraProperty[unit.second].numHitPoint,
				mUnitInitDataMap[unit.second].property.numDefence + mGameState[flag].extraProperty[unit.second].numDefence,
				mUnitInitDataMap[unit.second].property.numAttack + mGameState[flag].extraProperty[unit.second].numAttack,
				mUnitInitDataMap[unit.second].property.numRangeAttack + mGameState[flag].extraProperty[unit.second].numRangeAttack,
				mUnitInitDataMap[unit.second].property.numRangeMove + mGameState[flag].extraProperty[unit.second].numRangeMove,
				mUnitInitDataMap[unit.second].property.numPopulation + mGameState[flag].extraProperty[unit.second].numPopulation
		};
		auto resource = ResourcesStruct{
			mUnitInitDataMap[unit.second].consumption.numFixedResource,
			mUnitInitDataMap[unit.second].consumption.numRandomResource,
			mUnitInitDataMap[unit.second].consumption.numProductivity,
			mUnitInitDataMap[unit.second].consumption.numResearchLevel
		};
		mUnitCampLayer->setUnitResourceAndProperty(unit.second, resource, property);
	}
	//refresh lock
	for (const auto & i : mGameState[flag].unitLockMap)
	{
		mUnitCampLayer->setUnlocked(i.first, !i.second);
	}
}

void GameScene::refreshResourcesTexture()
{
	for (auto & i : mResourceMap)
	{
		//CCLOG("position: %d,%d", i.first.x, i.first.y);
		if ((i.second.type == fixedResource) || (i.second.type == randomResource))
		{
			float hp = (float)i.second.property.numHitPoint;
			float hpL = (float)(mUnitInitDataMap[i.second.type].property.numHitPoint);
			auto firstHp = hpL * mResourceCriticalMap.at(i.second.type).firstP;
			auto secondHp = hpL * mResourceCriticalMap.at(i.second.type).secondP;
			//CCLOG("type: %d, hp: %f, first hp: %f, second hp: %f", i.second.type, hp, firstHp, secondHp);
			if (hp > firstHp)
			{
				i.second.sprite->setTexture(mResourceTextureMap[i.second.type].abundant);
			}
			else if (hp > secondHp)
			{
				i.second.sprite->setTexture(mResourceTextureMap[i.second.type].middle);
			}
			else
			{
				i.second.sprite->setTexture(mResourceTextureMap[i.second.type].dried);
			}
		}
	}
}

void GameScene::refreshResourcesIcons(const int & turnFlag)
//void GameScene::refreshResourcesIcons(const ResourcesStruct & resources)
{
	auto resources = mResources[turnFlag];
	std::stringstream ss;
	ss << resources.numFixedResource;
	CCLOG("F: %s", ss.str().c_str());
	mFixedResourceLabel->setString(ss.str());
	ss.clear();
	ss.str("");
	ss << resources.numRandomResource;
	CCLOG("R: %s", ss.str().c_str());
	mRandomResourceLabel->setString(ss.str());
	ss.clear();
	ss.str("");
	ss << resources.numProductivity;
	mProductivityLabel->setString(ss.str());
	ss.clear();
	ss.str("");
	ss << resources.numResearchLevel;
	mResearchLabel->setString(ss.str());
}

void GameScene::refreshPopulationIcons(const int & turnFlag)
{
	std::stringstream ss;
	ss << mPopulation[turnFlag] << "/" << mPopulationLimit;
	mPopulationLabel->setString(ss.str());
}

void GameScene::refreshMakingButton(int turnF)
{
	if (mUnitFactory[turnF].unitExistence())
	{
		mUnitMakingButtonTexture = mUnitCampLayer->getUnitTexture(mUnitFactory[turnF].getMakingUnit());
		mUnitMakingButton->setTexture(mUnitMakingButtonTexture);
		mUnitMakingButton->setVisible(true);
	}
	else
	{
		mUnitMakingButton->setVisible(false);
	}
	if (mTechFactory[turnF].techExistence())
	{
		mTechMakingButton->setVisible(true);
		mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(mTechFactory[turnF].getMakingTech());
		mTechMakingButton->setTexture(mTechMakingButtonTexture);
	}
	else
	{
		mTechMakingButton->setVisible(false);
	}
}

void GameScene::onMouseMoved(Event * event)
{
	auto e = static_cast<EventMouse *>(event);
	//get mouse coordinate
	mMouseCoordinate.x = e->getLocation().x;
	mMouseCoordinate.y = 2 * mDirector->getWinSize().height - e->getLocation().y;
	//CCLOG("%f,%f", mMouseCoordinate.x, mMouseCoordinate.y);
	if ((mGameMode == GameModeEnum::client) || (mGameMode == GameModeEnum::server))
	{
		if (mWelcomeLayerDisplay)
		{
			checkBackToMainSceneItemOnMouseMoved();
		}
	}
	checkLayersOnMouseMoved();
}

//--startGame
void GameScene::startGame()
{
	//
	if (mGameMode == server || mGameMode == client)
	{
		//mWelcomeLayer->setVisible(false);
		mWelcomeLayerDisplay = false;
		mWelcomeLayer->removeFromParentAndCleanup(true);
	}
	initGameState();
	//turn
	mBlueTurn = true;
	if (mGameMode != client)
	{
		mOperateEnable = true;
		//update timer lock
		mUpdateTimerLock = false;
		mTimer->start();
	}
	else
	{
		CCLOG("in startGame(): client");
		//client
		mOperateEnable = false;
		mTimer->shutDown();
		mUpdateTimerLock = true;
	}
	//init farmer
	//spawnUnit(farmer, 0);

	for (int i = 0; i < mBasePosition.size(); i++)
	{
		spawnUnit(farmer, i, MyPointStruct{ mBasePosition[i].x + 1, mBasePosition[i].y + 1 });
		spawnUnit(farmer, i, MyPointStruct{ mBasePosition[i].x + 1, mBasePosition[i].y - 1 });
		spawnUnit(farmer, i, MyPointStruct{ mBasePosition[i].x - 1, mBasePosition[i].y + 1 });
		spawnUnit(farmer, i, MyPointStruct{ mBasePosition[i].x - 1, mBasePosition[i].y - 1 });
	}
	//
	addMist();
	//change Population
	mPopulation[0] += 4 * (mUnitInitDataMap[farmer].property.numPopulation
		+ mGameState[0].extraProperty[farmer].numPopulation);
	mPopulation[1] += 4 * (mUnitInitDataMap[farmer].property.numPopulation
		+ mGameState[1].extraProperty[farmer].numPopulation);
	//refresh minimap
	refreshMiniMap();
	//refresh resources
	refreshResourcesTexture();
	refreshPopulationIcons(0);



	//Test for info map
	//mInfoMapLayer->displayText("TECH", "FUCK YOU\nLIU QI!!\nAND FUCK YOUR MOTHER AND FATHER AND SISTER AND BROTHER", stringPredict + std::to_string(100) + stringTurn);
	//update
	scheduleUpdate();

	//TechTreeLayerRefreshing & add Mist
	if (mGameMode == server || mGameMode == vsPlayer)
	{
		refreshTechTreeLayer(0);
	}
	else if (mGameMode == client)
	{
		refreshTechTreeLayer(1);
	}

	//net update, 0.5s
	if (mGameMode == server || mGameMode == client)
	{
		schedule(schedule_selector(GameScene::netUpdate), 0.1, CC_REPEAT_FOREVER, 0);
	}
}

void GameScene::refreshMiniMap()
{
	std::set<MyPointStruct> unitSet[2];
	std::set<MyPointStruct> unitSetN[2];
	int tF = -1;
	if (!mist)
	{
		for (int k = 0; k < 2; ++k)
		{
			for (const auto & i : mGameState[k].unitMap)
			{
				if (i.second.state == fresh)
				{
					unitSet[k].insert(i.first);
				}
				else
				{
					unitSetN[k].insert(i.first);
				}
			}
			//base
			unitSet[k].insert(mBasePosition[k]);
			for (auto i : getNearPoint(mBasePosition[k]))
			{
				unitSet[k].insert(i);
			}
		}
	}
	else
	{
		if (mGameMode == server || mGameMode == client)
		{
			if (mGameMode == server)
			{
				tF = 0;
			}
			else if (mGameMode == client)
			{
				tF = 1;
			}
		}
		else if (mGameMode == vsPlayer)
		{
			tF = mBlueTurn ? 0 : 1;
		}
		for (const auto & i : mGameState[tF].unitMap)
		{
			if (i.second.state == fresh)
			{
				unitSet[tF].insert(i.first);
			}
			else
			{
				unitSetN[tF].insert(i.first);
			}
		}
		//base
		unitSet[tF].insert(mBasePosition[tF]);
		unitSet[1 - tF].insert(mBasePosition[1 - tF]);
		for (auto i : getNearPoint(mBasePosition[tF]))
		{
			unitSet[tF].insert(i);
		}
		for (auto i : getNearPoint(mBasePosition[1 - tF]))
		{
			unitSet[1 - tF].insert(i);
		}

		for (auto & pUnit : mGameState[1 - tF].unitMap)
		{
			//CCLOG("pathtree");
			for (auto p : visibleUnit)
			{
				//CCLOG("{%d,%d}",p.point.x, p.point.y);
				if (p == pUnit.first)
				{
					unitSet[1 - tF].insert(p);
				}
			}
			//CCLOG("\n");
		}

	}
	std::set<MyPointStruct> fixedRSet;
	std::set<MyPointStruct> randomRSet;
	for (const auto & i : mResourceMap)
	{
		if (i.second.type == fixedResource)
		{
			fixedRSet.insert(i.first);
			continue;
		}
		if (i.second.type == randomResource)
		{
			randomRSet.insert(i.first);
			continue;
		}
	}
	mMiniMapLayer->refresh(unitSet[0], unitSetN[0], unitSet[1], unitSetN[1], fixedRSet, randomRSet);
}

void GameScene::checkMiniMap()
{
	mMiniMapLayer->moveView(mMouseCoordinate);
	Vec2 pF = mMiniMapLayer->getViewPosition(mMouseCoordinate);
	//CCLOG("pF: %f,%f", pF.x, pF.y);
	Vec2 pOfM = Vec2(mTiledMapLayer->getMapSizeF().width * pF.x, mTiledMapLayer->getMapSizeF().height * pF.y);
	mTiledMapLayer->setPosition(mWinWidth / 2 - pOfM.x, mWinHeight / 2 - pOfM.y);
}

void GameScene::checkTiledMapOnTouchMoved()
{
	Vec2 mapP = mTiledMapLayer->getPosition();
	//CCLOG("mapP %f,%f", mapP.x, mapP.y);
	Size mapS = mTiledMapLayer->getMapSizeF();
	Vec2 newP = Vec2(mapP.x + mMouseCoordinate.x - mMouseCoordinateP.x, mapP.y + mMouseCoordinate.y - mMouseCoordinateP.y);
	Vec2 dis = Vec2(newP.x - mapP.x, newP.y - mapP.y);
	if (mapP.y <= mWinHeight - mapS.height - mWinHeight / 2 - dis.y/* - mapS.height + moveDis - 50*/)
	{
		newP.y = mapP.y;
	}
	else if (mapP.y >= 0 + mWinHeight / 2 - dis.y/* - moveDis + 280*/)
	{
		newP.y = mapP.y;
	}
	if (mapP.x >= 0 + mWinHeight / 2 - dis.x/* - moveDis*/)
	{
		newP.x = mapP.x;
	}
	else if (mapP.x <= mWinWidth - mapS.width - mWinHeight / 2 - dis.x/* - mapS.width + moveDis*/)
	{
		newP.x = mapP.x;
	}
	mTiledMapLayer->setPosition(newP);
}

void GameScene::checkTechTreeLayerOnTouchEnded()
{
	auto tech = mTechTreeLayer->getTechContainingPoint(mMouseCoordinateTouch);
	if (mGameMode == server || mGameMode == client)
	{
		int tF = (mGameMode == server) ? 0 : 1;
		if (mOperateEnable)
		{
			if (mGameState[tF].techTree.unlockable(tech))
			{
				if (!mTechFactory[tF].techExistence())
				{
					//add to factory
					if (mResources[tF] >= mTechInitDataMap[tech])
					{
						mResources[tF] -= mTechInitDataMap[tech];
						mTechFactory[tF].addNewTech(tech);
						mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(tech);
						refreshTechTreeLayer(tF);
						refreshMakingButton(tF);
						refreshResourcesIcons(tF);
					}
					else
					{
						playEffect(WARNING);
					}
				}
			}
		}
	}
	else if (mGameMode == vsPlayer)
	{
		int tF = mBlueTurn ? 0 : 1;
		if (mGameState[tF].techTree.unlockable(tech))
		{
			if (!mTechFactory[tF].techExistence())
			{
				if (mResources[tF] >= mTechInitDataMap[tech])
				{
					mResources[tF] -= mTechInitDataMap[tech];
					//mPopulation[tF] += mUnitInitDataMap[unit].property.numPopulation;
					mTechFactory[tF].addNewTech(tech);
					mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(tech);
					refreshTechTreeLayer(tF);
					refreshMakingButton(tF);
					CCLOG("vsPlayer; added new Tech!");
					refreshResourcesIcons(tF);
				}
				else
				{
					playEffect(WARNING);
				}
			}
		}
	}
}

void GameScene::checkUnitCampLayerOnTouchEnded()
{
	auto unit = mUnitCampLayer->getUnitMouseOn();
	if (mGameMode == server || mGameMode == client)
	{
		int tF = (mGameMode == server) ? 0 : 1;
		if (mOperateEnable)
		{
			if (!mUnitFactory[tF].unitExistence())
			{
				//add to factory
				if ((mResources[tF] >= mUnitInitDataMap[unit].consumption) && (mPopulation[tF] + mUnitInitDataMap[unit].property.numPopulation <= mPopulationLimit))
				{
					mResources[tF] -= mUnitInitDataMap[unit].consumption;
					//mPopulation[tF] += mUnitInitDataMap[unit].property.numPopulation;
					mUnitFactory[tF].addNewUnit(unit);
					refreshMakingButton(tF);
					refreshResourcesIcons(tF);
					mUnitMakingButtonTexture = mUnitCampLayer->getUnitTexture(unit);
				}
				else
				{
					playEffect(WARNING);
				}
			}
		}
	}
	else if (mGameMode == vsPlayer)
	{
		int tF = mBlueTurn ? 0 : 1;
		if (!mUnitFactory[tF].unitExistence())
		{
			if ((mResources[tF] >= mUnitInitDataMap[unit].consumption) && (mPopulation[tF] + mUnitInitDataMap[unit].property.numPopulation <= mPopulationLimit))
			{
				mResources[tF] -= mUnitInitDataMap[unit].consumption;
				//mPopulation[tF] += mUnitInitDataMap[unit].property.numPopulation;
				mUnitFactory[tF].addNewUnit(unit);
				refreshMakingButton(tF);
				CCLOG("vsPlayer; added new unit!");
				refreshResourcesIcons(tF);
				mUnitMakingButtonTexture = mUnitCampLayer->getUnitTexture(unit);
			}
			else
			{
				playEffect(WARNING);
			}
		}
	}
}

void GameScene::checkMakingButtonOnMouseMoved()
{
	if (mUnitMakingButton->isVisible())
	{
		//unit
		//BUG!! mUnitMakingButtonTexture should be bilateral
		if (mUnitMakingButton->boundingBox().containsPoint(mMouseCoordinate))
		{
			mUnitMakingButton->setTexture(mMakingCancelTexture);
		}
		else
		{
			mUnitMakingButton->setTexture(mUnitMakingButtonTexture);
		}
	}
	if (mTechMakingButton->isVisible())
	{
		if (mTechMakingButton->boundingBox().containsPoint(mMouseCoordinate))
		{
			mTechMakingButton->setTexture(mMakingCancelTexture);
		}
		else
		{
			mTechMakingButton->setTexture(mTechMakingButtonTexture);
		}
	}

}

void GameScene::checkMakingButtonOnTouchEnded()
{
	int tF = mBlueTurn ? 0 : 1;
	if (mUnitMakingButton->isVisible())
	{
		//unit
		if (mUnitMakingButton->boundingBox().containsPoint(mMouseCoordinate))
		{
			mUnitMakingButton->setVisible(false);
			mUnitFactory[tF].cancelNowUnit();
		}
	}
	if (mTechMakingButton->isVisible())
	{
		if (mTechMakingButton->boundingBox().containsPoint(mMouseCoordinate))
		{
			mTechMakingButton->setVisible(false);
			mTechFactory[tF].cancelNowTech();
			refreshTechTreeLayer(tF);
		}
	}
}

void GameScene::checkBackToMainSceneItemOnMouseMoved()
{
	//cancel's scale effect
	auto box = mBackToMainSceneItem->boundingBox();
	auto size = mBackToMainSceneItem->getContentSize();
	auto pos = mBackToMainSceneItem->getPosition();
	float x = pos.x + mDirector->getWinSize().width / 2 - size.width / 2;
	float y = pos.y + mDirector->getWinSize().height / 2 - size.height / 2;
	float width = size.width;
	float height = size.height;
	Rect rect(x, y, width, height);
	if (rect.containsPoint(mMouseCoordinate))
	{
		/*debug
		auto drawN = DrawNode::create();
		drawN->drawRect(Vec2(x, y), Vec2(x + width, y + height), Color4F(0, 0, 0, 1));
		drawN->drawRect(Vec2(box.getMinX(), box.getMinY()), Vec2(box.getMaxX(), box.getMaxY()), Color4F(1, 0, 0, 1));
		GameScene::addChild(drawN, 10);
		*/
		mBackToMainSceneItem->setScale(1.2);
	}
	else
	{
		mBackToMainSceneItem->setScale(1);
	}
}

void GameScene::checkLayersOnMouseMoved()
{
	do
	{
		//UI(timer,buttons,minimap)
		checkMakingButtonOnMouseMoved();
		//Tech/Unit layer
		if (mUnitCampLayer->isVisible())
		{
			//CCLOG("to call!!");
			mUnitCampLayer->onMouseMoved(mMouseCoordinate);
		}

		bool clearFlag = true;
		int tF = -1;
		if (mGameMode == vsPlayer)
		{
			tF = mBlueTurn ? 0 : 1;
		}
		else if (mGameMode == server || mGameMode == client)
		{
			tF = (mGameMode == server) ? 0 : 1;
		}

		//tiledMap
		if (mTiledMapLayer->containPoint(mMouseCoordinate)
			&& !mTechTreeLayer->isVisible() && !mUnitCampLayer->isVisible()
			&& !isGrayBarContains(mMouseCoordinate))
		{
			int whosbase = -1;
			auto mPos = mTiledMapLayer->tiledCoorForPostion(mMouseCoordinate);

			auto unitInfo = existUnitOnTiledMap(mPos);
			if (unitInfo.exist)
			{
				if (unitInfo.mUnitEnum == base)
				{
					if (mPos == mBasePosition[0])
					{
						whosbase = 0;
					}
					else
					{
						whosbase = 1;
					}
					mInfoMapLayer->displayUnitInfo(
						mUnitDisplayMap[unitInfo.mUnitEnum].unitName,
						unitInfo.property.numHitPoint,
						mMaxHitPointOfBase[whosbase].numHitPoint);
					clearFlag = false;
				}
				else if (unitInfo.mUnitEnum == fixedResource || unitInfo.mUnitEnum == randomResource)
				{
					mInfoMapLayer->displayUnitInfo(
						mUnitDisplayMap[unitInfo.mUnitEnum].unitName,
						unitInfo.property.numHitPoint,
						mUnitInitDataMap[unitInfo.mUnitEnum].property.numHitPoint
						);
					clearFlag = false;
				}
				else
				{
					bool visibleFlag = false;
					if (mist)
					{
						for (auto i : visibleUnit)
						{
							if (i == mPos)
							{
								visibleFlag = true;
							}
						}
					}
					else
					{
						visibleFlag = true;
					}
					if (visibleFlag){
						mInfoMapLayer->displayUnitProperty(
							mUnitDisplayMap[unitInfo.mUnitEnum].unitName,
							unitInfo.property.numHitPoint
							+ mGameState[whosUnit(mPos)].extraProperty[unitInfo.mUnitEnum].numHitPoint,
							mUnitInitDataMap[unitInfo.mUnitEnum].property.numHitPoint
							+ mGameState[whosUnit(mPos)].extraProperty[unitInfo.mUnitEnum].numHitPoint,
							mDisplayInfoMap["ATK"] + std::to_string(unitInfo.property.numAttack
							+ mGameState[whosUnit(mPos)].extraProperty[unitInfo.mUnitEnum].numAttack) + "\n"
							+ mDisplayInfoMap["DEF"] + std::to_string(unitInfo.property.numDefence
							+ mGameState[whosUnit(mPos)].extraProperty[unitInfo.mUnitEnum].numDefence) + "\n"
							+ mDisplayInfoMap["RATK"] + std::to_string(unitInfo.property.numRangeAttack
							+ mGameState[whosUnit(mPos)].extraProperty[unitInfo.mUnitEnum].numRangeAttack));
						clearFlag = false;
					}
				}
			}
		}

		//UnitMaking Button
		if (mUnitMakingButton->isVisible()
			&& mUnitMakingButton->boundingBox().containsPoint(mMouseCoordinate))
		{
			//refresh InfoMap
			auto unit = mUnitFactory[tF].getMakingUnit();
			mInfoMapLayer->displayText(mUnitCampLayer->getUnitName(unit),
				mUnitCampLayer->getUnitIntroDuction(unit),
				mDisplayInfoMap["Predict"] + std::to_string(calcInteger(
				mUnitFactory[tF].getLeftTime(),
				mResources[tF].numProductivity)) + mDisplayInfoMap["Turn"]);
			clearFlag = false;
		}

		//TechMakingButton
		if (mTechMakingButton->isVisible() &&
			mTechMakingButton->boundingBox().containsPoint(mMouseCoordinate))
		{
			//refresh InfoMap
			auto tech = mTechFactory[tF].getMakingTech();
			mInfoMapLayer->displayText(mTechDisplayMap[tech].techName,
				mTechDisplayMap[tech].techIntroduction,
				mDisplayInfoMap["Predict"] + std::to_string(calcInteger(
				mTechFactory[tF].getLeftTime(),
				mResources[tF].numResearchLevel)) + mDisplayInfoMap["Turn"]);
			clearFlag = false;
		}

		//mUnitCampLayer
		if (mUnitCampLayer->isVisible() &&
			mUnitCampLayer->containPoint(mMouseCoordinate))
		{
			auto unit = mUnitCampLayer->getUnitMouseOn();
			mInfoMapLayer->displayText(mUnitCampLayer->getUnitName(unit),
				mUnitCampLayer->getUnitIntroDuction(unit),
				mDisplayInfoMap["Predict"] + std::to_string(calcInteger(
				mUnitCampLayer->getUnitProductivity(unit),
				mResources[tF].numProductivity)) + mDisplayInfoMap["Turn"]);
			clearFlag = false;
		}

		//techTreeLayer
		if (mTechTreeLayer->isVisible()
			&& mTechTreeLayer->containPoint(mMouseCoordinate))
		{
			auto tech = mTechTreeLayer->getTechContainingPoint(mMouseCoordinate);
			if (mGameState[tF].techTree.unlockable(tech))
			{
				mInfoMapLayer->displayText(mTechDisplayMap[tech].techName,
					mTechDisplayMap[tech].techIntroduction,
					mDisplayInfoMap["Predict"] + std::to_string(calcInteger(
					mTechInitDataMap[tech].numResearchLevel,
					mResources[tF].numResearchLevel)) + mDisplayInfoMap["Turn"]);
				clearFlag = false;
			}
			if (mGameState[tF].techTree.isUnlocked(tech))
			{
				mInfoMapLayer->displayText(mTechDisplayMap[tech].techName,
					mTechDisplayMap[tech].techIntroduction,
					"");
				clearFlag = false;
			}
		}

		if (isGrayBarContains(mMouseCoordinate))
		{
			if (isResourcesIconsContains(mMouseCoordinate, fixedResourceI)
				|| isResourcesIconsContains(mMouseCoordinate, fixedResourceL))
			{
				mInfoMapLayer->displayText(mUnitDisplayMap[fixedResource].unitName,
					mUnitDisplayMap[fixedResource].unitIntroduction,
					std::to_string(mResources[tF].numFixedResource));
				clearFlag = false;
			}
			if (isResourcesIconsContains(mMouseCoordinate, randomResourceI)
				|| isResourcesIconsContains(mMouseCoordinate, randomResourceL))
			{
				mInfoMapLayer->displayText(mUnitDisplayMap[randomResource].unitName,
					mUnitDisplayMap[randomResource].unitIntroduction,
					std::to_string(mResources[tF].numRandomResource));
				clearFlag = false;
			}
			if (isResourcesIconsContains(mMouseCoordinate, productivityI)
				|| isResourcesIconsContains(mMouseCoordinate, productivityL))
			{
				mInfoMapLayer->displayText(mDisplayInfoMap["productivity"],
					mDisplayInfoMap["productivityIntro"],
					std::to_string(mResources[tF].numProductivity));
				clearFlag = false;
			}
			if (isResourcesIconsContains(mMouseCoordinate, researchI)
				|| isResourcesIconsContains(mMouseCoordinate, researchL))
			{
				mInfoMapLayer->displayText(mDisplayInfoMap["research"],
					mDisplayInfoMap["researchIntro"],
					std::to_string(mResources[tF].numResearchLevel));
				clearFlag = false;
			}
			if (isResourcesIconsContains(mMouseCoordinate, populationI)
				|| isResourcesIconsContains(mMouseCoordinate, populationL))
			{
				mInfoMapLayer->displayText(mDisplayInfoMap["population"],
					mDisplayInfoMap["populationIntro"],
					std::to_string(mPopulation[tF]) + "/" + std::to_string(mPopulationLimit));
				clearFlag = false;
			}
			if (mTechTreeLayerButton->boundingBox().containsPoint(mMouseCoordinate))
			{
				mInfoMapLayer->displayText(mDisplayInfoMap["TechTree"],
					mDisplayInfoMap["TechTreeIntro"], "");
				clearFlag = false;
			}
			if (mUnitCampLayerButton->boundingBox().containsPoint(mMouseCoordinate))
			{
				mInfoMapLayer->displayText(mDisplayInfoMap["UnitCamp"],
					mDisplayInfoMap["UnitCampIntro"], "");
				clearFlag = false;
			}
		}
		if (mTimer->containPoint(mMouseCoordinate))
		{
			mInfoMapLayer->displayText(mDisplayInfoMap["Timer"],
				mDisplayInfoMap["TimerIntro"], "");
			clearFlag = false;
		}

		//waiting method to get label  contain info;
		if (clearFlag)
		{
			mInfoMapLayer->clearAllInfo();
		}
	} while (0);
}

bool GameScene::isGrayBarContains(Vec2 mMouseCoordinate)
{
	for (auto i : mGrayBarRect)
	{
		if (i.containsPoint(mMouseCoordinate))
			return true;
	}
	return false;
}

bool GameScene::isResourcesIconsContains(Vec2 mMouseCoordinate, ResourcesIconEnum tag)
{
	float mBoxScale = 0.6;
	auto mParentBox = resourcesIcons->getBoundingBox();
	auto mChildBox = resourcesIcons->getChildByTag(tag)->getBoundingBox();
	auto mRect = Rect(mParentBox.getMidX() + mChildBox.getMinX() * mBoxScale,
		mParentBox.getMidY() + mChildBox.getMinY() * mBoxScale,
		(mChildBox.getMaxX() - mChildBox.getMinX()) * mBoxScale,
		(mChildBox.getMaxY() - mChildBox.getMinY()) * mBoxScale);
	if (mRect.containsPoint(mMouseCoordinate))
		return true;
	return false;
}

UnitNowDisplayStruct GameScene::existUnitOnTiledMap(const MyPointStruct & mPos)
{
	//unitMap, mResourceMap
	for (auto i : mResourceMap)
	{
		if (i.first == mPos)
		{
			return UnitNowDisplayStruct{ true, i.second.type, i.second.property };
		}
	}
	for (int j = 0; j < 2; j++)
	{
		for (auto i : mGameState[j].unitMap)
		{
			if (i.first == mPos)
			{
				return UnitNowDisplayStruct{ true, i.second.type, i.second.property };
			}
		}
	}
	return  UnitNowDisplayStruct{ false, base, UnitPropertyStruct{ 0, 0, 0, 0, 0, 0 } };
}

int GameScene::calcInteger(int a, int b)
{
	if (a == 0)
		return 0;
	if (a%b)
	{
		return a / b + 1;
	}
	else
	{
		return a / b;
	}
}

int GameScene::whosUnit(MyPointStruct unitPoint)
{
	for (int i = 0; i < 2; ++i)
	{
		for (auto & hehe : mGameState[i].unitMap)
		{
			if (hehe.first == unitPoint)
			{
				return i;
			}
		}
	}
	return -1;
}
void GameScene::checkTechAndUnitButton()
{
	//open/close techTreeLayer/UnitCampLayer
	if (mTechTreeLayerButton->boundingBox().containsPoint(mMouseCoordinate))
	{
		if (!mTechTreeLayer->isVisible())
		{
			playEffect(VISTA);
			mTechTreeLayer->setVisible(true);
			mUnitCampLayer->setVisible(false);
			mUnitCampLayerButton->setTexture(mUnitCampLayerButtonTexture.off);
			mTechTreeLayerButton->setTexture(mTechTreeLayerButtonTexture.on);
		}
		else
		{
			mTechTreeLayer->setVisible(false);
			mTechTreeLayerButton->setTexture(mTechTreeLayerButtonTexture.off);
		}
	}
	if (mUnitCampLayerButton->boundingBox().containsPoint(mMouseCoordinate))
	{
		if (!mUnitCampLayer->isVisible())
		{
			playEffect(BILIBILI);
			mUnitCampLayer->setVisible(true);
			mTechTreeLayer->setVisible(false);
			mTechTreeLayerButton->setTexture(mTechTreeLayerButtonTexture.off);
			mUnitCampLayerButton->setTexture(mUnitCampLayerButtonTexture.on);
		}
		else
		{
			mUnitCampLayer->setVisible(false);
			mUnitCampLayerButton->setTexture(mUnitCampLayerButtonTexture.off);
		}
	}
}

void GameScene::initAttackTexture()
{
	mAttackTexture =
	{
		mDirector->getTextureCache()->addImage("animation/LR1ThrowOut.png"),
		mDirector->getTextureCache()->addImage("animation/LR1Explosive.png"),
		mDirector->getTextureCache()->addImage("animation/LR2ThrowOut.png"),
		mDirector->getTextureCache()->addImage("animation/LR2Explosive.png"),
		mDirector->getTextureCache()->addImage("animation/LR3Explosive.png"),
		mDirector->getTextureCache()->addImage("animation/SR2ThrowOut.png"),
		mDirector->getTextureCache()->addImage("animation/SR2Explosive.png")
	};
}

void GameScene::initGameState()
{
	//num turn
	mNumTurn = 0;
	//spawn

	//population
	mPopulation[0] = mPopulation[1] = 0;
	//共通
	initUnitData();
	initTechData();



	initResourceMap();

	mSpawn[0] = MyPointStruct{ mBasePosition[0].x, mBasePosition[0].y };
	mSpawn[1] = MyPointStruct{ mBasePosition[1].x, mBasePosition[1].y };

	for (int i = 0; i < mBasePosition.size(); i++)
	{
		spawnPoint.push_back(MyPointStruct{ mBasePosition[i].x + 1, mBasePosition[i].y + 1 });
		spawnPoint.push_back(MyPointStruct{ mBasePosition[i].x + 1, mBasePosition[i].y - 1 });
		spawnPoint.push_back(MyPointStruct{ mBasePosition[i].x - 1, mBasePosition[i].y + 1 });
		spawnPoint.push_back(MyPointStruct{ mBasePosition[i].x - 1, mBasePosition[i].y - 1 });
	}
	//syncronize mist
	//send Mist
	if (mGameMode == server)
	{
		int tempMist = mist ? 1 : 0;
		while (!mNet.sendMist(tempMist))
		{
			auto err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				//mDirector->popScene();
			}
		}
		while (!mNet.read()){}
		CCLOG("mist sended!");
	}
	if (mGameMode == client)
	{
		while (!mNet.read())
		{
		}
		while (!mNet.sendMist(mNet.getMist()))
		{
		}
		if (mNet.getWhich() == mistnet)
		{
			bool tempMist = mNet.getMist() ? true : false;
			mist = tempMist;
		}
	}

	//set unit resources and property in campLayer
	for (const auto & i : mUnitInitDataMap)
	{
		mUnitCampLayer->setUnitResourceAndProperty(i.first, i.second.consumption, i.second.property);
	}
	//mResources
	mResources[0] = mResources[1] = ResourcesStruct{ 30, 30, 10, 10 };
	//extra resources
	mExtraResources[0] = mExtraResources[1] = ResourcesStruct{ 0, 0, 0, 0 };
	//set Label value
	std::stringstream ssFixed;
	ssFixed << mResources[0].numFixedResource;
	mFixedResourceLabel->setString(ssFixed.str());
	std::stringstream ssRandom;
	ssRandom << mResources[0].numRandomResource;
	mRandomResourceLabel->setString(ssRandom.str());
	std::stringstream ssProductivity;
	ssProductivity << mResources[0].numProductivity;
	mProductivityLabel->setString(ssProductivity.str());
	std::stringstream ssResearch;
	ssResearch << mResources[0].numResearchLevel;
	mResearchLabel->setString(ssResearch.str());

	mCollectionEffeciency = ResourcesStruct{ 0, 0, 0, 0 };
	//双边
	TechTree techTree;
	//CCLOG("techroot? %d",techTree.isUnlocked(techroot));
	std::map<MyPointStruct, Unit> unitMap;
	std::map<UnitEnum, UnitPropertyStruct> extraProperty;
	std::map<UnitEnum, bool> unitLockMap = {
		{ farmer, false },
		{ shortrangeunit1, true },
		{ shortrangeunit2, true },
		{ longrangeunit1, true },
		{ longrangeunit2, true },
		{ longrangeunit3, true },
	};
	GameStateStruct  gameState = {
		techTree,
		unitLockMap,
		unitMap,
		extraProperty
	};
	gameState.techTree.unlock(techroot);
	mGameState[0] = mGameState[1] = gameState;
	//initUnitFactory
	for (int i = 0; i < 2; ++i)
	{
		mUnitFactory[i].setUnitTime(farmer, mUnitCampLayer->getUnitResources(farmer).numProductivity);
		mUnitFactory[i].setUnitTime(shortrangeunit1, mUnitCampLayer->getUnitResources(shortrangeunit1).numProductivity);
		mUnitFactory[i].setUnitTime(shortrangeunit2, mUnitCampLayer->getUnitResources(shortrangeunit2).numProductivity);
		mUnitFactory[i].setUnitTime(longrangeunit1, mUnitCampLayer->getUnitResources(longrangeunit1).numProductivity);
		mUnitFactory[i].setUnitTime(longrangeunit2, mUnitCampLayer->getUnitResources(longrangeunit2).numProductivity);
		mUnitFactory[i].setUnitTime(longrangeunit3, mUnitCampLayer->getUnitResources(longrangeunit3).numProductivity);
	}
	for (int i = 0; i < 2; i++)
	{
		for (auto refreshTech : mTechEnumList)
		{
			mTechFactory[i].setTechTime(refreshTech, mTechInitDataMap[refreshTech].numResearchLevel);
		}
	}
	//init string
	mDisplayInfoMap["Turn"] = getDicValue("Turn");
	mDisplayInfoMap["Predict"] = getDicValue("Predict");
	mDisplayInfoMap["Going"] = getDicValue("Going");
	mDisplayInfoMap["ATK"] = getDicValue("ATK");
	mDisplayInfoMap["DEF"] = getDicValue("DEF");
	mDisplayInfoMap["RATK"] = getDicValue("RATK");
	mDisplayInfoMap["Left"] = getDicValue("Left");
	mDisplayInfoMap["Second"] = getDicValue("Second");
	mDisplayInfoMap["Timer"] = getDicValue("Timer");
	mDisplayInfoMap["TimerIntro"] = getDicValue("TimerIntro");
	mDisplayInfoMap["productivity"] = getDicValue("productivity");
	mDisplayInfoMap["productivityIntro"] = getDicValue("productivityIntro");
	mDisplayInfoMap["research"] = getDicValue("research");
	mDisplayInfoMap["researchIntro"] = getDicValue("researchIntro");
	mDisplayInfoMap["population"] = getDicValue("population");
	mDisplayInfoMap["populationIntro"] = getDicValue("populationIntro");
	mDisplayInfoMap["TechTree"] = getDicValue("TechTree");
	mDisplayInfoMap["TechTreeIntro"] = getDicValue("TechTreeIntro");
	mDisplayInfoMap["UnitCamp"] = getDicValue("UnitCamp");
	mDisplayInfoMap["UnitCampIntro"] = getDicValue("UnitCampIntro");

	//yyp
	mUnitActionFSM[0] = 0;
	mUnitActionFSM[1] = 0;

}

//--initResourceMap
void GameScene::initResourceMap()
{
	//const float ranScale = 0.5;
	//const float fixScale = 0.5;
	//fixed: json
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("map/" + mapName);
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string((const char *)loadStr, size);

	rapidjson::Document jDocument;
	jDocument.Parse<0>(jsonStr.c_str());
	if (jDocument.HasParseError())
	{
		CCLOG("unitDisplay.json parse error!");
	}
	if (!jDocument.IsObject())
	{
		CCLOG("error : not object!");
	}
	rapidjson::Value & mapElements = jDocument["mapelement"];
	if (mapElements.IsArray())
	{
		for (int i = 0; i < mapElements.Size(); ++i)
		{
			rapidjson::Value & item = mapElements[i];
			MyPointStruct point = { item["position"]["X"].GetInt(), item["position"]["Y"].GetInt() };
			//CCLOG("point: %d,%d; num: %d", point.x, point.y, item["numHitPoint"].GetInt());
			std::string element{ item["element"].GetString() };
			//CCLOG("element: %s", element.c_str());
			if (element == "base0")
			{
				//CCLOG("a base!");
				mBasePosition.push_back(point);
				Unit unit = {
					UnitEnum::base,
					mUnitInitDataMap[base].property,
					UnitStateEnum::attacked,
					[&]()->Sprite*{
						auto sprite = Sprite::create("uiComponent/base_blue.png");
						sprite->setScale(0.75);
						return sprite;
					}()
						//Sprite::create("item1.png")
						//sprite
				};
				//
				//CCLOG("reM,add base:%d,%d,%d,%d,%d,%d", unit.property.numHitPoint, unit.property.numDefence, unit.property.numAttack, unit.property.numRangeAttack, unit.property.numRangeMove, unit.property.numPopulation);
				mResourceMap[point] = unit;
				continue;
			}
			if (element == "base1")
			{
				//CCLOG("a base!");
				mBasePosition.push_back(point);
				Unit unit = {
					UnitEnum::base,
					mUnitInitDataMap[base].property,
					UnitStateEnum::attacked,
					[&]()->Sprite*{
						auto sprite = Sprite::create("uiComponent/base_red.png");
						sprite->setScale(0.75);
						return sprite;
					}()
						//Sprite::create("item1.png")
						//sprite
				};
				//
				//CCLOG("reM,add base:%d,%d,%d,%d,%d,%d", unit.property.numHitPoint, unit.property.numDefence, unit.property.numAttack, unit.property.numRangeAttack, unit.property.numRangeMove, unit.property.numPopulation);
				mResourceMap[point] = unit;
				continue;
			}
			if (element == "resourceFixed")
			{
				//CCLOG("a resourceF!");
				Unit unit = {
					UnitEnum::fixedResource,
					UnitPropertyStruct{ item["numHitPoint"].GetInt(), 0, 0, 0, 0, 0 },
					UnitStateEnum::attacked,
					[&]()->Sprite*{
						auto sprite = Sprite::createWithTexture(mResourceTextureMap[fixedResource].abundant);
						//sprite->setScale(fixScale);
						return sprite;
					}()
						//Sprite::createWithTexture(mResourceTextureMap[fixedResource].abundant)
				};
				mResourceMap[point] = unit;
				continue;
			}
			if (element == "resourceRandom")
			{
				//CCLOG("a resourceR!");
				Unit unit = {
					UnitEnum::randomResource,
					UnitPropertyStruct{ item["numHitPoint"].GetInt(), 0, 0, 0, 0, 0 },
					UnitStateEnum::attacked,
					[&]()->Sprite*{
						auto sprite = Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant);
						//sprite->setScale(ranScale);
						return sprite;
					}()
						//Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant)
				};
				//save HP of randomR
				mHitPointOfRandomResource = item["numHitPoint"].GetInt();
				//CCLOG("hp of ranP: %d", mHitPointOfRandomResource);
				mResourceMap[point] = unit;
				continue;
			}
		}
	}
	//random: random
	if (mGameMode != client)
	{
		int i = 0;
		while (i < mNumOfRandomResource)
		{
			MyPointStruct ranP = {
				CCRANDOM_0_1() * mMapSize.width,
				CCRANDOM_0_1() * mMapSize.height
			};
			//CCLOG("i = %d, ranP: %d,%d",i, ranP.x, ranP.y);
			bool occupied = false;
			//near spawn
			CCLOG("random point {%d,%d}", ranP.x, ranP.y);
			for (const auto & spawn : mSpawn)
			{
				CCLOG("spawn point {%d,%d}", spawn.x, spawn.y);
				if ((spawn.x == ranP.x) && (spawn.y == ranP.y))
				{
					occupied = true;
					break;
				}
				for (auto p : getNearPoint(spawn))
				{
					CCLOG("spawn near point {%d,%d}", p.x, p.y);
					if ((p.x == ranP.x) && (p.y == ranP.y))
					{
						occupied = true;
						break;
					}
				}
			}

			for (const auto & spawn : spawnPoint)
			{
				CCLOG("spawnPoint point {%d,%d}", spawn.x, spawn.y);
				if ((spawn.x == ranP.x) && (spawn.y == ranP.y))
				{
					occupied = true;
					break;
				}
				for (auto p : getNearPoint(spawn))
				{
					CCLOG("near point {%d,%d}", p.x, p.y);
					if ((p.x == ranP.x) && (p.y == ranP.y))
					{
						occupied = true;
						break;
					}
				}
			}

			for (const auto & i : mResourceMap)
			{
				if ((i.first.x == ranP.x) && (i.first.y == ranP.y))
				{
					occupied = true;
					break;
				}
				//near base
				if (i.second.type == base)
				{
					for (auto p : getNearPoint(i.first))
					{
						if ((p.x == ranP.x) && (p.y == ranP.y))
						{
							occupied = true;
							break;
						}
					}
				}
			}
			if (!occupied)
			{
				//send ranP to client
				if (mGameMode == server)
				{
					while (!mNet.sendOnePoint(ranP))
					{
						/*
						auto err = WSAGetLastError();
						if (err != WSAEWOULDBLOCK)
						{
						CCLOG("he GGed so fast!!!");
						mDirector->popScene();
						}
						*/
					}
					//check send back
					while (!mNet.read())
					{
					}
					CCLOG("read send back!");
					if (mNet.getWhich() != onePoint)
					{
						//error
						CCLOG("read back error");
					}
					CCLOG("read back :%d,%d", mNet.getOnePoint().x, mNet.getOnePoint().y);
				}
				mResourceMap[ranP] = Unit{
					UnitEnum::randomResource,
					UnitPropertyStruct{ mHitPointOfRandomResource, 0, 0, 0, 0, 0 },
					UnitStateEnum::attacked,
					[&]()->Sprite*{
						auto sprite = Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant);
						//sprite->setScale(ranScale);
						return sprite;
					}()
						//Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant)
				};
				++i;
			}
		}
		if (mGameMode == server)
		{
			while (!mNet.sendEnd())
			{
				auto err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					CCLOG("he GGed so fast!!!");
					mDirector->popScene();
				}
			}
			CCLOG("sended end");
		}
	}
	else
	{
		//client: read ranP
		while (true)
		{
			//read a ranP
			while (!mNet.read())
			{
				//auto err = WSAGetLastError();
				/*
				if (err != WSAEWOULDBLOCK )
				{
				CCLOG("he GGed so fast!!!");
				mDirector->popScene();
				}
				*/
			}
			if (mNet.getWhich() == end)
			{
				CCLOG("read end");
				//以防万一
				mNet.lockOn();
				break;
			}
			//send back
			while (!mNet.sendOnePoint(mNet.getOnePoint()))
			{
			}
			if (mNet.getWhich() == onePoint)
			{
				mResourceMap[mNet.getOnePoint()] = Unit{
				UnitEnum::randomResource,
				UnitPropertyStruct{ mHitPointOfRandomResource, 0, 0, 0, 0, 0 },
				UnitStateEnum::attacked,
				[&]()->Sprite*{
					auto sprite = Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant);
					//sprite->setScale(ranScale);
					return sprite;
				}()
					//Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant)
			};
				CCLOG("read: %d,%d", mNet.getOnePoint().x, mNet.getOnePoint().y);
			}
		}
	}
	//place them
	for (const auto & i : mResourceMap)
	{
		i.second.sprite->setPosition(mTiledMapLayer->floatNodeCoorForPosition(i.first));
		mTiledMapLayer->addChild(i.second.sprite, 2);
	}
}

void GameScene::initTechData()
{
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("dictionary/techtreedisplay.json");
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string((const char *)loadStr, size);

	rapidjson::Document jDocument;
	jDocument.Parse<0>(jsonStr.c_str());
	if (jDocument.HasParseError())
	{
		CCLOG("unitDisplay.json parse error!");
	}
	if (!jDocument.IsObject())
	{
		CCLOG("error : not object!");
	}
	rapidjson::Value & techTree = jDocument["techtree"];
	if (techTree.IsArray())
	{
		for (int i = 0; i < techTree.Size(); ++i)
		{
			rapidjson::Value & tech = techTree[i];
			TechEnum techE = techTree::mTechStringEnumMap.at(tech["tech"].GetString());
			//CCLOG("tech: %s", tech["tech"].GetString());
			ResourcesStruct consumption = {
				tech["consumption"]["numFixedResource"].GetInt(),
				tech["consumption"]["numRandomResource"].GetInt(),
				0,
				tech["consumption"]["absResearchLevel"].GetInt()
			};
			mTechInitDataMap[techE] = consumption;

			//Get Influence
			InfluenceStruct influence = {
				tech["influence"]["type"].GetString(),
				tech["influence"]["target"].GetString(),
				tech["influence"]["value"].GetInt()
			};
			mTechInitInfluenceMap[techE] = influence;

			//Get DisplayInfo
			techIntroductionStruct introduction = {
				tech["title"].GetString(),
				tech["explain"].GetString()
				+ std::string("\n") +
				tech["dialogue"].GetString()
			};
			mTechDisplayMap[techE] = introduction;
		}
	}
}

void GameScene::initUnitData()
{
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("dictionary/unitdata.json");
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string((const char *)loadStr, size);

	rapidjson::Document jDocument;
	jDocument.Parse<0>(jsonStr.c_str());
	if (jDocument.HasParseError())
	{
		CCLOG("unitdata.json parse error!");
	}
	if (!jDocument.IsObject())
	{
		CCLOG("error : not object!");
	}
	rapidjson::Value & unitData = jDocument["unitData"];
	if (unitData.IsArray())
	{
		for (int i = 0; i < unitData.Size(); ++i)
		{
			rapidjson::Value & unit = unitData[i];
			UnitInitDataStruct data = {
				UnitPropertyStruct{
				unit["attributes"]["numHitPoint"].GetInt(),
				unit["attributes"]["numDefence"].GetInt(),
				unit["attributes"]["numAttack"].GetInt(),
				unit["attributes"]["numAttackRange"].GetInt(),
				unit["attributes"]["numMoveRange"].GetInt(),
				unit["attributes"]["numPopulation"].GetInt()
			},
			ResourcesStruct{
					unit["consumption"]["numFixedResource"].GetInt(),
					unit["consumption"]["numRandomResource"].GetInt(),
					unit["consumption"]["absProductivity"].GetInt(),
					0
				}
			};
			unitIntroductionStruct introduction = {
				unit["name"].GetString(),
				unit["introduction"].GetString()
			};
			std::string type = unit["unit"].GetString();
			//CCLOG("type: %s", type.c_str());
			if (type == "base")
			{
				mUnitInitDataMap[base] = data;
				mUnitDisplayMap[base] = introduction;
				mMaxHitPointOfBase[0] = data.property;
				mMaxHitPointOfBase[1] = data.property;
				continue;
			}
			if (type == "farmer")
			{
				mUnitInitDataMap[farmer] = data;
				mUnitDisplayMap[farmer] = introduction;
				continue;
			}
			if (type == "shortrangeunit1")
			{
				mUnitInitDataMap[shortrangeunit1] = data;
				mUnitDisplayMap[shortrangeunit1] = introduction;
				continue;
			}
			if (type == "shortrangeunit2")
			{
				mUnitInitDataMap[shortrangeunit2] = data;
				mUnitDisplayMap[shortrangeunit2] = introduction;
				continue;
			}
			if (type == "longrangeunit1")
			{
				mUnitInitDataMap[longrangeunit1] = data;
				mUnitDisplayMap[longrangeunit1] = introduction;
				continue;
			}
			if (type == "longrangeunit2")
			{
				mUnitInitDataMap[longrangeunit2] = data;
				mUnitDisplayMap[longrangeunit2] = introduction;
				continue;
			}
			if (type == "longrangeunit3")
			{
				mUnitInitDataMap[longrangeunit3] = data;
				mUnitDisplayMap[longrangeunit3] = introduction;
				continue;
			}
			if (type == "fixedResource")
			{
				mUnitInitDataMap[fixedResource] = data;
				mUnitDisplayMap[fixedResource] = introduction;
				continue;
			}
			if (type == "randomResource")
			{
				mUnitInitDataMap[randomResource] = data;
				mUnitDisplayMap[randomResource] = introduction;
				continue;
			}
		}
	}
}

void GameScene::initResourcesIcons()
{
	const float iconsFontSize = 30;
	const float offset = 200;
	const float offset2 = 30;
	resourcesIcons = Node::create();
	fixedResourceIcon = Sprite::create("uiComponent/icon_hydrogen.png");
	fixedResourceIcon->setPosition(offset, 0);
	resourcesIcons->addChild(fixedResourceIcon, 1, fixedResourceI);
	mFixedResourceLabel = Label::createWithTTF("", mFonts, iconsFontSize);
	mFixedResourceLabel->setColor(Color3B(0, 0, 0));
	mFixedResourceLabel->setPosition(fixedResourceIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mFixedResourceLabel, 1, fixedResourceL);

	randomResourceIcon = Sprite::create("uiComponent/icon_gravity.png");
	randomResourceIcon->setPosition(0, 0);
	resourcesIcons->addChild(randomResourceIcon, 1, randomResourceI);
	mRandomResourceLabel = Label::createWithTTF("", mFonts, iconsFontSize);
	mRandomResourceLabel->setColor(Color3B(0, 0, 0));
	mRandomResourceLabel->setPosition(randomResourceIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mRandomResourceLabel, 1, randomResourceL);

	productivityIcon = Sprite::create("uiComponent/icon_productivity.png");
	productivityIcon->setPosition(offset * 2, 0);
	resourcesIcons->addChild(productivityIcon, 1, productivityI);
	mProductivityLabel = Label::createWithTTF("", mFonts, iconsFontSize);
	mProductivityLabel->setColor(Color3B(0, 0, 0));
	mProductivityLabel->setPosition(productivityIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mProductivityLabel, 1, productivityL);

	researchIcon = Sprite::create("uiComponent/icon_researchlevel.png");
	researchIcon->setPosition(offset * 3, 0);
	resourcesIcons->addChild(researchIcon, 1, researchI);
	mResearchLabel = Label::createWithTTF("", mFonts, iconsFontSize);
	mResearchLabel->setColor(Color3B(0, 0, 0));
	mResearchLabel->setPosition(researchIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mResearchLabel, 1, researchL);

	populationIcon = Sprite::create("uiComponent/icon_cpu.png");
	populationIcon->setPosition(offset * 4, 0);
	resourcesIcons->addChild(populationIcon, 1, populationI);
	std::stringstream ssP;
	ssP << "0/" << mPopulationLimit;
	mPopulationLabel = Label::createWithTTF(ssP.str(), mFonts, iconsFontSize);
	mPopulationLabel->setColor(Color3B(0, 0, 0));
	mPopulationLabel->setPosition(populationIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mPopulationLabel, 1, populationL);

	//You shall not change scale arbitrarily, if you change this, then change the scale in isReourcesIconsCotains()
	resourcesIcons->setScale(0.6);
	resourcesIcons->setPosition(mWinWidth / 2 - 2.5 * offset * resourcesIcons->getScale(), mWinHeight - (fixedResourceIcon->boundingBox().getMaxY() - fixedResourceIcon->boundingBox().getMinY()) / 2);
	addChild(resourcesIcons, 3);

	CCLOG("BOX(%f,%f,%f,%f)", resourcesIcons->getChildByTag(randomResourceI)->getBoundingBox().getMaxX(),
		resourcesIcons->getChildByTag(randomResourceI)->getBoundingBox().getMaxY(),
		resourcesIcons->getChildByTag(randomResourceI)->getBoundingBox().getMinX(),
		resourcesIcons->getChildByTag(randomResourceI)->getBoundingBox().getMinY());
	CCLOG("BOX(%f,%f,%f,%f)", resourcesIcons->getChildByTag(randomResourceL)->getBoundingBox().getMaxX(),
		resourcesIcons->getChildByTag(randomResourceL)->getBoundingBox().getMaxY(),
		resourcesIcons->getChildByTag(randomResourceL)->getBoundingBox().getMinX(),
		resourcesIcons->getChildByTag(randomResourceL)->getBoundingBox().getMinY());
	//CCLOG("%f", fixedResourceIcon->boundingBox().getMaxY() - fixedResourceIcon->boundingBox().getMinY());
}

void GameScene::initGameMenu()
{
	auto youWinLabel = Label::createWithTTF(getDicValue("youWin"), mFonts, 30);
	auto GGLabel = Label::createWithTTF(getDicValue("GG"), mFonts, 30);
	auto youWinItem = MenuItemLabel::create(youWinLabel, [&](Ref * sender)->void{
		if (mOperateEnable)
		{
			if (mGameMode == server || mGameMode == client)
			{
				while (!mNet.sendYouWin())
				{
					auto err = WSAGetLastError();
					if (err != WSAEWOULDBLOCK)
					{
						CCLOG("he GG!!");
						//mDirector->popScene();
					}
				}
				while (!mNet.read()){}
				CCLOG("read send back!");
				int tF = mGameMode == server ? 0 : 1;
				win(1 - tF);
			}
			else if (mGameMode == vsPlayer)
			{
				//lose
				CCLOG("i lose!");
				int tF = mBlueTurn ? 0 : 1;
				win(1 - tF);
			}
		}
	});
	auto GGItem = MenuItemLabel::create(GGLabel, [&](Ref * sender)->void{
		mDirector->popScene();
		//autorelease();
	});
	auto menuBg = DrawNode::create();
	menuBg->drawSolidRect(Vec2(mWinWidth / 2 - 100, mWinHeight / 2 - 150),
		Vec2(mWinWidth / 2 + 100, mWinHeight / 2 + 150),
		mBarColor);
	auto menu = Menu::create(/*bgItem,*/ youWinItem, GGItem, NULL);
	menu->alignItemsVerticallyWithPadding(20);
	mGameMenu = Node::create();
	mGameMenu->addChild(menuBg);
	mGameMenu->addChild(menu);
	mGameMenu->setVisible(false);
	addChild(mGameMenu, 8);
}

void GameScene::initWelcomeLayer()
{
	//welcome layer : waiting and connecting
	//init juFlower
	mWelcomeLayer = Layer::create();
	Sprite * juFlower[2];
	for (int i = 0; i < 2; ++i)
	{
		juFlower[i] = Sprite::create("uiComponent/icon_waiting" + std::string{ (char)('0' + i) } +".png");
		juFlower[i]->setScale(0.8);
		juFlower[i]->setPosition(
			mWinWidth / 2,
			mWinHeight / 2);
	}
	juFlower[0]->runAction(RepeatForever::create(RotateBy::create(1, -30)));
	juFlower[1]->runAction(RepeatForever::create(RotateBy::create(1, 30)));
	//wating label
	auto watingLabel = Label::createWithTTF("no word", mFonts, 30);
	//CCLOG("%s",watingLabel->getString().c_str());
	if (mGameMode == GameModeEnum::server)
	{
		CCLOG("waiting:  %s", mDictionary->valueForKey("wating")->getCString());
		watingLabel->setString(mDictionary->valueForKey("waiting")->getCString());
	}
	if (mGameMode == GameModeEnum::client)
	{
		watingLabel->setString(mDictionary->valueForKey("connecting")->getCString());
	}
	watingLabel->setPosition(
		mWinWidth / 2,
		juFlower[0]->getBoundingBox().getMaxY() + 0.3 * (juFlower[0]->getBoundingBox().getMaxY() - juFlower[0]->getBoundingBox().getMinY())
		);
	auto backToMainSceneLabel = Label::createWithTTF(mDictionary->valueForKey("cancel")->getCString(), "/fonts/STXIHEI.TTF", 30);
	mBackToMainSceneItem = MenuItemLabel::create(backToMainSceneLabel, CC_CALLBACK_1(GameScene::backToMainScene, this));
	mBackToMainSceneItem->setPosition(
		mBackToMainSceneItem->getPosition().x,
		mBackToMainSceneItem->getPosition().y - juFlower[0]->getBoundingBox().getMaxY() + juFlower[0]->getBoundingBox().getMinY() + 10
		);
	auto welcomeMenu = Menu::create(mBackToMainSceneItem, NULL);

	auto welcomeBg = DrawNode::create();
	welcomeBg->drawSolidRect(Vec2(0, 0), Vec2(mWinWidth, mWinHeight), Color4F(0, 0, 0, 1));
	/*
	welcomeMenu->setPosition(
	welcomeMenu->getPosition().x,
	juFlower->getBoundingBox().getMinY() * 2- juFlower->getBoundingBox().getMaxY()
	);
	*/
	mWelcomeLayer->addChild(welcomeBg);
	mWelcomeLayer->addChild(watingLabel);
	mWelcomeLayer->addChild(welcomeMenu);
	mWelcomeLayer->addChild(juFlower[0]);
	mWelcomeLayer->addChild(juFlower[1]);
	addChild(mWelcomeLayer, 10);
}

void GameScene::initYypNet()
{
	//yypNet
	switch (mGameMode)
	{
	case server:
		mNet.startServer(mUserDefault->getIntegerForKey("port"));
		schedule(schedule_selector(GameScene::acceptConnect), 0.1, CC_REPEAT_FOREVER, 0);
		break;
	case client:
		//display juFlower
		//mNet.makeConnect((char *)(mUserDefault->getStringForKey("ip").c_str()), mUserDefault->getIntegerForKey("port"));
		mNet.startConnect((char *)(mUserDefault->getStringForKey("ip").c_str()), mUserDefault->getIntegerForKey("port"));
		CCLOG("ip: %s", (char *)(mUserDefault->getStringForKey("ip").c_str()));
		CCLOG("port %d", mUserDefault->getIntegerForKey("port"));
		schedule(schedule_selector(GameScene::startConnecting), 0.1, CC_REPEAT_FOREVER, 0);
		//startConnecting(0.1);
		break;
	case vsPlayer:
		break;
	default:
		CCLOG("default?");
		break;
	}
}


void GameScene::initResourceTexture()
{
	mResourceTextureMap[fixedResource] = {
		mDirector->getTextureCache()->addImage("uiComponent/resource_fixed_abundant.png"),
		mDirector->getTextureCache()->addImage("uiComponent/resource_fixed_middle.png"),
		mDirector->getTextureCache()->addImage("uiComponent/resource_fixed_dried.png")
	};
	mResourceTextureMap[randomResource] = {
		mDirector->getTextureCache()->addImage("uiComponent/resource_random_abundant.png"),
		mDirector->getTextureCache()->addImage("uiComponent/resource_random_middle.png"),
		mDirector->getTextureCache()->addImage("uiComponent/resource_random_dried.png")
	};
}

void GameScene::initUnitTexture()
{
	for (int i = 0; i < 2; ++i)
	{
		mUnitTextureMap[i][farmer] = {
			mDirector->getTextureCache()->addImage(mIconFile + "/farmer_front_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/farmer_back_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/farmer_side_" + std::to_string(i) + ".png")
		};
		mUnitTextureMap[i][longrangeunit1] = {
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit1_front_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit1_back_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit1_side_" + std::to_string(i) + ".png")
		};
		mUnitTextureMap[i][longrangeunit2] = {
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit2_front_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit2_back_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit2_side_" + std::to_string(i) + ".png")
		};
		mUnitTextureMap[i][longrangeunit3] = {
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit3_front_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit3_back_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/longrangeunit3_side_" + std::to_string(i) + ".png")
		};
		mUnitTextureMap[i][shortrangeunit1] = {
			mDirector->getTextureCache()->addImage(mIconFile + "/shortrangeunit1_front_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/shortrangeunit1_back_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/shortrangeunit1_side_" + std::to_string(i) + ".png")
		};
		mUnitTextureMap[i][shortrangeunit2] = {
			mDirector->getTextureCache()->addImage(mIconFile + "/shortrangeunit2_front_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/shortrangeunit2_back_" + std::to_string(i) + ".png"),
			mDirector->getTextureCache()->addImage(mIconFile + "/shortrangeunit2_side_" + std::to_string(i) + ".png")
		};
	}
	CCLOG("munittexturemap size: %d", mUnitTextureMap[0].size());
}

void GameScene::showMoveRange(const MyPointStruct & unitPoint, const int & tF)//yypmark
{
	CCLOG("showMoveRange: unitPoint: %d,%d", unitPoint.x, unitPoint.y);
	std::set <MyPointStruct> barrier;// need to repair
	for (auto state : mGameState)
	{
		for (auto ob : state.unitMap)
		{
			barrier.insert(ob.first);
		}
	}
	for (auto ob : mResourceMap)
	{
		if (ob.second.type == base)
		{
			auto nearPoint = getNearPoint(ob.first);
			for (auto point : nearPoint)
			{
				barrier.insert(point);
			}
		}
		barrier.insert(ob.first);
	}
	auto unit = mGameState[tF].unitMap[unitPoint];
	mMoveRange = getPathTree(unitPoint, unit.property.numRangeMove + mGameState[tF].extraProperty[unit.type].numRangeMove, barrier);
	for (auto unitPath : mMoveRange)
	{
		mTiledMapLayer->setTileColor(unitPath.point, 2);
	}
	playUnitSound(unit.type);
}

void GameScene::showAttackRange(const MyPointStruct & unitPoint, const int & tF)
{
	CCLOG("showAttackRange: unitPoint: %d,%d", unitPoint.x, unitPoint.y);
	CCLOG("he base P : %d,%d", mBasePosition[1 - tF].x, mBasePosition[1 - tF].y);
	std::set <MyPointStruct> barrier;
	auto unit = mGameState[tF].unitMap[unitPoint];
	auto attackTree = getPathTree(unitPoint, unit.property.numRangeAttack + mGameState[tF].extraProperty[unit.type].numRangeAttack, barrier);
	CCLOG("my atk range: %d", unit.property.numRangeAttack);
	for (auto attacking : attackTree)
	{
		if (attacking.point == mBasePosition[1 - tF])
		{
			mAttackRange.insert(attacking.point);
			mTiledMapLayer->setTileColor(attacking.point, 3);
		}
		//near base
		for (const auto & i : getNearPoint(mBasePosition[1 - tF]))
		{
			if (attacking.point == i)
			{
				mAttackRange.insert(attacking.point);
				mTiledMapLayer->setTileColor(attacking.point, 3);
			}
		}
		for (auto enemy : mGameState[1 - tF].unitMap)
		{
			if (attacking.point == enemy.first)
			{
				mAttackRange.insert(attacking.point);
				mTiledMapLayer->setTileColor(attacking.point, 3);
			}
		}
		if (unit.type == farmer)
		{
			for (auto resource : mResourceMap)
			{
				if (attacking.point == resource.first)
				{
					mAttackRange.insert(attacking.point);
					mTiledMapLayer->setTileColor(attacking.point, 3);
				}
			}
		}
	}
}

void GameScene::deleteMoveRange()
{
	for (auto unitPath : mMoveRange)
	{
		//CCLOG("deleteMoveRange: %d,%d", unitPath.point.x, unitPath.point.y);
		mTiledMapLayer->setTileColor(unitPath.point, 1);
	}
	mMoveRange.clear();
}

void GameScene::deleteAttackRange()
{
	CCLOG("attack range size %d", mAttackRange.size());
	for (auto attacking : mAttackRange)
	{
		//CCLOG("deleteAttackRange: %d,%d", attacking.x, attacking.y);
		mTiledMapLayer->setTileColor(attacking, 1);
	}
	mAttackRange.clear();
}

//--unitAction
void GameScene::unitAction(const MyPointStruct & nowPoint, int tF)
{
	CCLOG("in unitAction: tF = %d", tF);
	//mUnitActionFSM[tF] = 0;
	CCLOG("%d", mUnitActionFSM[tF]);
	switch (mUnitActionFSM[tF])
	{
		//intial
	case 0:
		for (auto i : mGameState[tF].unitMap)
		{
			if (i.first == nowPoint)
			{
				if (i.second.state == fresh)
				{
					mOriginalPoint = nowPoint;
					showMoveRange(mOriginalPoint, tF);
					showAttackRange(mOriginalPoint, tF);
					mUnitActionFSM[tF] = 1;
				}
				if (i.second.state == moved)
				{
					mOriginalPoint = nowPoint;
					//showMoveRange(mOriginalPoint, tF);
					showAttackRange(mOriginalPoint, tF);
					mUnitActionFSM[tF] = 2;
				}
				if (i.second.state == attacked)
				{
					playEffect(WARNING);
				}
				return;
			}
		}
		break;
		//move
	case 1:
		if (nowPoint == mOriginalPoint)
		{
			deleteMoveRange();
			deleteAttackRange();
			mUnitActionFSM[tF] = 0;
			break;
		}
		for (auto attackingNode : mAttackRange)
		{
			if (nowPoint == attackingNode)
			{
				deleteMoveRange();
				deleteAttackRange();
				//send
				if (mGameMode == server || mGameMode == client)
				{
					while (!mNet.sendTwoPoint(twoPointStruct{ mOriginalPoint, nowPoint }))
					{
						auto err = WSAGetLastError();
						if (err != WSAEWOULDBLOCK)
						{
							//mDirector->popScene();
						}
					}
					while (!mNet.read()){}
					CCLOG("read send back!");
				}
				//attack
				mGameState[tF].unitMap[mOriginalPoint].state = attacked;
				attackUnit(mOriginalPoint, nowPoint, tF);
				CCLOG("in Action: origin: %d,%d", mOriginalPoint.x, mOriginalPoint.y);
				mUnitActionFSM[tF] = 0;
				return;
			}
		}
		for (auto pathNode : mMoveRange)
		{
			if (nowPoint == pathNode.point)
			{
				CCLOG("correctRoot");
				auto movePath = getPath(mMoveRange, nowPoint);
				//testing getPath
				deleteAttackRange();
				deleteMoveRange();
				mGameState[tF].unitMap[mOriginalPoint].state = moved;
				//send
				if (mGameMode == server || mGameMode == client)
				{
					while (!mNet.sendTwoPoint(twoPointStruct{ mOriginalPoint, nowPoint }))
					{
						auto err = WSAGetLastError();
						if (err != WSAEWOULDBLOCK)
						{
							//mDirector->popScene();
						}
					}
					while (!mNet.read()){}
					CCLOG("read send back!");
				}
				//move
				moveUnit(movePath, tF, true);
				//farmer
				if (mGameState[tF].unitMap[mOriginalPoint].type == farmer)
				{
					collectionFarmerMove(mOriginalPoint, tF);
				}
				mOriginalPoint = nowPoint;
				/*
				showAttackRange(nowPoint, tF);
				if (mAttackRange.empty())
				{
				deleteAttackRange();
				mUnitActionFSM[tF] = 0;
				}
				else
				{
				mUnitActionFSM[tF] = 2;
				}
				*/
				return;
			}
		}
		for (auto i : mGameState[tF].unitMap)
		{
			if (i.first == nowPoint && nowPoint != mOriginalPoint)
			{
				if (i.second.state == fresh)
				{
					deleteMoveRange();
					deleteAttackRange();
					mOriginalPoint = nowPoint;
					showMoveRange(mOriginalPoint, tF);
					showAttackRange(mOriginalPoint, tF);
					mUnitActionFSM[tF] = 1;
					return;
				}
				if (i.second.state == moved)
				{
					deleteMoveRange();
					deleteAttackRange();
					mOriginalPoint = nowPoint;
					//showMoveRange(mOriginalPoint, tF);
					showAttackRange(mOriginalPoint, tF);
					mUnitActionFSM[tF] = 2;
					return;
				}
				if (i.second.state == attacked)
				{
					playEffect(WARNING);
					return;
				}
			}
		}
		//else yyp
		deleteMoveRange();
		deleteAttackRange();
		mUnitActionFSM[tF] = 0;
		break;
		//after move
	case 2:
		if (nowPoint == mOriginalPoint)
		{
			deleteMoveRange();
			deleteAttackRange();
			mUnitActionFSM[tF] = 0;
			break;
		}
		for (auto attackingNode : mAttackRange)
		{
			if (nowPoint == attackingNode)
			{
				deleteAttackRange();
				//send
				if (mGameMode == server || mGameMode == client)
				{
					while (!mNet.sendTwoPoint(twoPointStruct{ mOriginalPoint, nowPoint }))
					{
						auto err = WSAGetLastError();
						if (err != WSAEWOULDBLOCK)
						{
							//mDirector->popScene();
						}
					}
					while (!mNet.read()){}
					CCLOG("read send back!");
				}
				//attack
				mGameState[tF].unitMap[mOriginalPoint].state = attacked;
				attackUnit(mOriginalPoint, nowPoint, tF);
				CCLOG("in Action: origin: %d,%d", mOriginalPoint.x, mOriginalPoint.y);
				mUnitActionFSM[tF] = 0;
				return;
			}
		}
		for (auto i : mGameState[tF].unitMap)
		{
			if (i.first == nowPoint && nowPoint != mOriginalPoint)
			{
				if (i.second.state == fresh)
				{
					deleteMoveRange();
					deleteAttackRange();
					mOriginalPoint = nowPoint;
					showMoveRange(mOriginalPoint, tF);
					showAttackRange(mOriginalPoint, tF);
					mUnitActionFSM[tF] = 1;
					return;
				}
				if (i.second.state == moved)
				{
					deleteMoveRange();
					deleteAttackRange();
					mOriginalPoint = nowPoint;
					//showMoveRange(mOriginalPoint, tF);
					showAttackRange(mOriginalPoint, tF);
					mUnitActionFSM[tF] = 2;
					return;
				}
				if (i.second.state == attacked)
				{
					playEffect(WARNING);
					return;
				}
			}
		}
		break;
	}
}

//need test
bool GameScene::collecable(const MyPointStruct & resourcePosition, const int & tF)
{
	for (const auto & i : mResourceCollectionMap)
	{
		if (i.first == resourcePosition)
		{
			//found
			if (i.second.owner == -1 || i.second.owner == tF)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	//not found
	return true;
}

//need test
void GameScene::refreshResourceCollectionState(const MyPointStruct & resourcePosition, bool increase, const int & tF)
{
	for (auto & i : mResourceCollectionMap)
	{
		if (i.first == resourcePosition)
		{
			//found
			int change = increase ? 1 : -1;
			CCLOG("refreshRCS: change: %d", change);
			i.second.numOfFarmer += change;
			i.second.owner = tF;
			if (i.second.numOfFarmer == 0)
			{
				//wild now
				CCLOG("change to wild!");
				i.second.owner = -1;
			}
			return;
		}
	}
	//not found, add new
	mResourceCollectionMap[resourcePosition] = ResourceCollectionStruct{
		tF,
		1
	};
}

//need test
void GameScene::collectionFarmerMove(const MyPointStruct & farmerFrom, const int & tF)
{
	//get resourcePoint
	bool found = false;
	MyPointStruct resourcePosition;
	for (auto & i : mFarmerResourceMap[tF])
	{
		if (i.first == farmerFrom)
		{
			//found
			found = true;
			resourcePosition = i.second;
		}
	}
	if (found)
	{
		refreshResourceCollectionState(resourcePosition, false, tF);
		mFarmerResourceMap[tF].erase(farmerFrom);
	}
}

//need test
void GameScene::collectionFamerAttack(const MyPointStruct & farmerFrom, const MyPointStruct & farmerTo, const int & tF)
{
	//check collecable
	if (!collecable(farmerTo, tF))
	{
		return;
	}
	bool found = false;
	for (auto & i : mFarmerResourceMap[tF])
	{
		if (i.first == farmerFrom)
		{
			found = true;
			//found farmer
			if (i.second == farmerTo)
			{
				//attack attacking resource
				return;
			}
		}
	}
	if (found)
	{
		//decrease numOfFarmer
		refreshResourceCollectionState(farmerTo, false, tF);
		mFarmerResourceMap[tF].erase(farmerFrom);
	}
	mFarmerResourceMap[tF][farmerFrom] = farmerTo;
	refreshResourceCollectionState(farmerTo, true, tF);
}

//need test
void GameScene::refreshResource(const int & tF)
{
	auto productivity = mResources[tF].numProductivity + mExtraResources[tF].numProductivity;
	std::vector<MyPointStruct> toDeleteResourceP;
	for (auto & i : mResourceCollectionMap)
	{
		if (i.second.owner == tF)
		{
			auto type = mResourceMap[i.first].type;
			int extraPro;
			if (type == fixedResource)
			{
				extraPro = mExtraResources[tF].numFixedResource;
			}
			else if (type == randomResource)
			{
				extraPro = mExtraResources[tF].numRandomResource;
			}
			int & leftHP = mResourceMap[i.first].property.numHitPoint;
			CCLOG("leftHP: %d", leftHP);
			int deltaHP = i.second.numOfFarmer * (mUnitInitDataMap[farmer].property.numAttack + mGameState[tF].extraProperty[farmer].numAttack + extraPro);
			CCLOG("deltaHP: %d", deltaHP);
			if (leftHP > deltaHP)
			{
				leftHP -= deltaHP;
			}
			else
			{
				//dried
				//die
				die(i.first, tF);
				toDeleteResourceP.push_back(i.first);
				//erase from farmer->resource map
				std::vector<MyPointStruct> toDeleteFarmerP;
				for (auto F2R : mFarmerResourceMap[tF])
				{
					if (F2R.second == i.first)
					{
						//found 
						toDeleteFarmerP.push_back(F2R.first);
					}
				}
				for (const auto & famerP : toDeleteFarmerP)
				{
					mFarmerResourceMap[tF].erase(famerP);
				}
			}
			//refresh mResources
			if (type == fixedResource)
			{
				mResources[tF].numFixedResource += deltaHP;
			}
			else if (type == randomResource)
			{
				mResources[tF].numRandomResource += deltaHP;
			}
		}
	}
	//delete dried resource from resourceCollectionMap
	for (auto reP : toDeleteResourceP)
	{
		mResourceCollectionMap.erase(reP);
	}
}

void GameScene::initMusic()
{
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(BG_MUSIC);
	SimpleAudioEngine::getInstance()->playBackgroundMusic(BG_MUSIC);
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(BG_MUSIC2);
	SimpleAudioEngine::getInstance()->preloadEffect(ELECTROSWITCH);
	SimpleAudioEngine::getInstance()->preloadEffect(BILIBILI);
	SimpleAudioEngine::getInstance()->preloadEffect(BONG);
	SimpleAudioEngine::getInstance()->preloadEffect(BWUBWU);
	SimpleAudioEngine::getInstance()->preloadEffect(BZIIU);
	SimpleAudioEngine::getInstance()->preloadEffect(FALLAPART);
	SimpleAudioEngine::getInstance()->preloadEffect(PSUUU);
	SimpleAudioEngine::getInstance()->preloadEffect(VISTA);
	SimpleAudioEngine::getInstance()->preloadEffect(BZZZ);
	SimpleAudioEngine::getInstance()->preloadEffect(BZANG);
	SimpleAudioEngine::getInstance()->preloadEffect(BWANG);
	SimpleAudioEngine::getInstance()->preloadEffect(METALCLI);
	SimpleAudioEngine::getInstance()->preloadEffect(BBBIOU);
	SimpleAudioEngine::getInstance()->preloadEffect(VICTORY);
	SimpleAudioEngine::getInstance()->preloadEffect(FAIL);
	SimpleAudioEngine::getInstance()->preloadEffect(WEAKEXPLOSIVE);
	SimpleAudioEngine::getInstance()->preloadEffect(RADARBUBU);
	SimpleAudioEngine::getInstance()->preloadEffect(WANWANG);
	SimpleAudioEngine::getInstance()->preloadEffect(DING);
	SimpleAudioEngine::getInstance()->preloadEffect(WANU);
	SimpleAudioEngine::getInstance()->preloadEffect(FOLLOWME);
	SimpleAudioEngine::getInstance()->preloadEffect(IMREADY);
	SimpleAudioEngine::getInstance()->preloadEffect(ELECTROBZZZ);
	SimpleAudioEngine::getInstance()->preloadEffect(WARNING);

	isPlayingIntensive = false;
}

void GameScene::playUnitSound(UnitEnum mUnit)
{
	switch (mUnit)
	{
	case base:
		break;
	case farmer:
		playEffect(BBBIOU);
		break;
	case shortrangeunit1:
		playEffect(METALCLI);
		break;
	case shortrangeunit2:
		playEffect(WANWANG);
		break;
	case longrangeunit1:
		playEffect(ELECTROBZZZ);
		break;
	case longrangeunit2:
		playEffect(WANU);
		break;
	case longrangeunit3:
		playEffect(RADARBUBU);
		break;
	case fixedResource:
		break;
	case randomResource:
		break;
	default:
		break;
	}

}

void GameScene::playEffect(const char * filePath)
{
	if (mUserDefault->getBoolForKey("seOn"))
	{
		SimpleAudioEngine::getInstance()->playEffect(filePath);
	}
}

void GameScene::playBackgroundMusic(const char * filePath)
{
	if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
	{
		if (!mUserDefault->getBoolForKey("musicOn"))
		{
			SimpleAudioEngine::getInstance()->stopBackgroundMusic();
		}
	}
	else
	{
		if (mUserDefault->getBoolForKey("musicOn"))
		{
			SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath, true);
		}
	}
}

void GameScene::surpriseEgg()
{
	if (mGameMode == vsPlayer)
	{
		if (mNumTurn > 500)
		{
			for (int tF = 0; tF < 2; ++tF)
			{
				for (auto i : mTechEnumList)
				{
					unlockTechTree(tF, i);
				}
			}
		}
	}
}

void GameScene::addMist(const int & tF, bool beginOfTurn)
{
	visibleUnit.clear();
	if (beginOfTurn)
	{
		for (auto mUnit : mGameState[1 - tF].unitMap)
		{
			if (!mUnit.second.sprite->isVisible())
			{
				mUnit.second.sprite->setVisible(true);
			}
			visibleUnit.push_back(mUnit.first);
		}
		for (auto & i : mGameState[tF].unitMap)
		{
			if (i.second.sprite->isVisible())
			{
				i.second.sprite->setVisible(false);
				CCLOG("already setinvisible");
			}
		}
		CCLOG("I'm In");
		for (auto mUnit : mGameState[1 - tF].unitMap)
		{
			for (auto & pUnit : mGameState[tF].unitMap)
			{
				//CCLOG("pathtree");
				for (auto p : getPathTree(mUnit.first, sightRange, std::set<MyPointStruct>{}))
				{
					//CCLOG("{%d,%d}",p.point.x, p.point.y);
					if (p.point == pUnit.first)
					{
						visibleUnit.push_back(p.point);
						pUnit.second.sprite->setVisible(true);
						CCLOG("set visible ");
					}
				}
				//CCLOG("\n");
			}
		}
		return;
	}
	/*bool foundUnit = false;
	for (auto & pUnit : mGameState[tF].unitMap)
	{
	for (auto mUnit : mGameState[1 - tF].unitMap)
	{
	for (auto p : getPathTree(mUnit.first, sightRange, std::set<MyPointStruct>{}))
	{
	if (p.point == pUnit.first)
	{
	if (!pUnit.second.sprite->isVisible())
	{
	pUnit.second.sprite->setVisible(true);
	foundUnit = true;
	}
	}
	}
	}
	if (!foundUnit)
	{
	if (pUnit.second.sprite->isVisible())
	{
	pUnit.second.sprite->setVisible(false);
	}
	}
	}*/
}

void GameScene::addMist()
{
	if (mist)
	{
		int tF = -1;
		if (mGameMode == server)
		{
			tF = 0;
		}
		else if (mGameMode == vsPlayer)
		{
			tF = mBlueTurn ? 0 : 1;
		}
		else if (mGameMode == client)
		{
			tF = 1;
		}
		addMist(1 - tF, true);
	}
}

void GameScene::initMist()
{
	if (mUserDefault->getBoolForKey("mistOn"))
	{
		mist = true;
	}
	else
	{
		mist = false;
	}
	sightRange = 7;
}

void GameScene::initTheme()
{
	if (mUserDefault->getStringForKey("theme") == "grey")
	{
		mBarColor = COLOR4F_GREY_T;
		mBgColor = COLOR4F_BLACK_T;
		mFonts = "fonts/STXIHEI.TTF";
		mIconFile = "unitIcon";
		mShape = 0;
		mCh = mUserDefault->getStringForKey("language");
	}
	else if (mUserDefault->getStringForKey("theme") == "pink")
	{
		mBarColor = COLOR4F_PINK;
		mBgColor = COLOR4F_DARKGREY;
		mFonts = "fonts/TGX.ttf";
		mIconFile = "unitIconPink";
		mShape = 1;
		mCh = mUserDefault->getStringForKey("language") + "Pink";
	}
}