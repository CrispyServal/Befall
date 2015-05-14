#include "GameScene.h"


GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}


std::string GameScene::getDicValue(char * str)
{
	 CCLOG("getdicValue: result: %s",mDictionary->valueForKey(str)->getCString());
	 return std::string{mDictionary->valueForKey(str)->getCString()};
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
	if (point.x < mMapSize.width)
	{
		result.push_back(MyPointStruct{ point.x + 1, point.y });
	}
	if (point.y < mMapSize.height)
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
	return result;
}

bool GameScene::init()
{
	if (!Scene::init())
	{
		return false;
	}
	//global
	mDirector = Director::getInstance();
	mUserDefault = UserDefault::getInstance();
    mDictionary = Dictionary::createWithContentsOfFile(std::string{ "dictionary/" + mUserDefault->getStringForKey("language") + ".xml" }.c_str());
	mKeyStruct = {
		false,
		false,
		false,
		false
	};
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
	//init MapSize
	mMapSize = mTiledMapLayer->getMapSize();
	addChild(mTiledMapLayer,1);
	//menu
	initGameMenu();
	auto startMenu = Menu::create(MenuItemLabel::create( 
	[&]()->Label*{
		auto label = Label::create(getDicValue("menu"), "fonts/STXIHEI.TTF", 24);
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
	} ), NULL );
	startMenu->setPosition(startMenu->getPosition().x - mWinWidth / 2 + 40, startMenu->getPosition().y + mWinHeight / 2 - 20);
	addChild(startMenu,5);
	//timer
	mTimer = GameTimer::create();
	mTimer->setPosition(0, - mWinHeight / 2);
	mTimer->setMaxTime(60);
	mTimer->setEndName(getDicValue("endTurn"));
	addChild(mTimer, 5);
	//techtreelayer
	mTechTreeLayer = TechTreeLayer::create();
	mTechTreeLayer->setVisible(false);
	addChild(mTechTreeLayer, 2);
	//InfoMapLayer
	mInfoMapLayer = InfoMapLayer::create();
	mInfoMapLayer->setPosition(-mWinWidth / 2 + 100, -mWinHeight / 2 + 120);
	addChild(mInfoMapLayer, 5);
	//miniMapLayer
	mMiniMapLayer = MiniMapLayer::create();
	float miniPS = 8;
	mMiniMapLayer->setPointSize(miniPS);
	mMiniMapLayer->setMapSize(mTiledMapLayer->getMapSize().width, mTiledMapLayer->getMapSize().height);
	mMiniMapLayer->setPosition(Vec2((mWinWidth - mTiledMapLayer->getMapSize().width * miniPS - mWinHeight / mTiledMapLayer->getTileSize().width * miniPS) / 2, (- mWinHeight +mTiledMapLayer->getMapSize().width * miniPS +mWinHeight / mTiledMapLayer->getTileSize().width * miniPS ) / 2));
	addChild(mMiniMapLayer, 5);
	//gray bar
	mGrayBar = DrawNode::create();
	mGrayBar->drawSolidRect(Vec2(0, 0), Vec2(mWinWidth, 50), Color4F(0.607, 0.607, 0.607, 0.75));
	mGrayBar->drawSolidRect(Vec2(0, mWinHeight-45), Vec2(mWinWidth, mWinHeight), Color4F(0.607, 0.607, 0.607, 0.75));
	mGrayBar->drawSolidRect(Vec2(0, 50), Vec2(200 + 10, 240 + 10), Color4F(0.607, 0.607, 0.607, 0.75));
	mGrayBar->drawSolidRect(Vec2(mWinWidth - mTiledMapLayer->getMapSize().width * miniPS - mWinHeight / mTiledMapLayer->getTileSize().width * miniPS, 50), Vec2(mWinWidth,50 +mTiledMapLayer->getMapSize().width * miniPS +mWinHeight / mTiledMapLayer->getTileSize().width * miniPS -50 ),Color4F(0.607, 0.607, 0.607, 0.75));
	addChild(mGrayBar, 3);
	//turn label
	mTurnLabel = Label::createWithTTF("0", "fonts/STXIHEI.TTF", 30);
	mTurnLabel->setColor(Color3B(0, 0, 0));
	mTurnLabel->setPosition(mWinWidth - 50, mWinHeight - mTurnLabel->getContentSize().height / 2);
	addChild(mTurnLabel, 8);
	//resources icon
	initResourcesIcons();
	//unitcamplayer
	mUnitCampLayer = UnitCampLayer::create();
	//mUnitCampLayer->setPosition(0, 50);
	mUnitCampLayer->setUnlocked(farmer,true);
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
	mTechTreeLayerButton->setPosition(mWinWidth / 2 - 150, (mTechTreeLayerButton->getBoundingBox().getMaxY() - mTechTreeLayerButton->getBoundingBox().getMinY())/2 - 3);
	mTechTreeLayerButton->setScale(0.9);
	addChild(mTechTreeLayerButton, 8);

	mUnitCampLayerButtonTexture = {
		mDirector->getTextureCache()->addImage("uiComponent/icon_militarycamp_black.png"),
		mDirector->getTextureCache()->addImage("uiComponent/icon_militarycamp_white.png")
	};
	mUnitCampLayerButton = Sprite::createWithTexture(mUnitCampLayerButtonTexture.off);
	mUnitCampLayerButton->setPosition(mWinWidth / 2 + 150, (mUnitCampLayerButton->getBoundingBox().getMaxY() - mUnitCampLayerButton->getBoundingBox().getMinY())/2 - 3);
	mUnitCampLayerButton->setScale(0.9);
	addChild(mUnitCampLayerButton, 8);
	//2 working button
	mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(techroot);
	mTechMakingButton = Sprite::createWithTexture(mTechMakingButtonTexture);
	mTechMakingButton->setScale(0.8);
	mTechMakingButton->setPosition(mTechTreeLayerButton->getPosition().x - 2 * (mTechMakingButton->boundingBox().getMaxX() - mTechMakingButton->boundingBox().getMinX()), mTechTreeLayerButton->getPosition().y);
	mTechMakingButton->setVisible(false);
	addChild(mTechMakingButton, 8);
	mUnitMakingButtonTexture = mUnitCampLayer->getUnitTexture(farmer);
	mUnitMakingButton = Sprite::createWithTexture(mUnitMakingButtonTexture);
	mUnitMakingButton->setScale(0.8);
	mUnitMakingButton->setPosition(mUnitCampLayerButton->getPosition().x + 2 * (mUnitMakingButton->boundingBox().getMaxX() - mUnitMakingButton->boundingBox().getMinX()), mUnitCampLayerButton->getPosition().y);
	mUnitMakingButton->setVisible(false);
	addChild(mUnitMakingButton, 8);
	
	if (mGameMode == vsPlayer)
	{
		startGame();
	}
	else if (mGameMode == server || mGameMode == client)
	{
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
	mTouchListener->onTouchMoved= CC_CALLBACK_2(GameScene::onTouchMoved, this);
	mTouchListener->onTouchEnded= CC_CALLBACK_2(GameScene::onTouchEnded, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(GameScene::mTouchListener, this);
	mKeyboardListener = EventListenerKeyboard::create();
	mKeyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
	mKeyboardListener->onKeyReleased= CC_CALLBACK_2(GameScene::onKeyReleased, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(mKeyboardListener,this);
	return true;
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
	if (mNet.makeConnect((char *)mUserDefault->getStringForKey("ip").c_str(), mUserDefault->getIntegerForKey("port")))
	{
		CCLOG("connecting successed");
		startGame();
		unschedule(schedule_selector(GameScene::startConnecting));
	}
}

void GameScene::netUpdate(float delta)
{
	CCLOG("net update");
	//test
	if (mGameMode == server || mGameMode == client)
	{
		int tF = mGameMode == server ? 0 : 1;
		if ( mNet.read())
		{
			CCLOG("read!");
			if (!mNet.isLocked())
			{
				//read something
				whichEnum which = mNet.getWhich();
				if (which == newTech)
				{
					unlockTechTree(tF, mNet.getTech());
				}
				else if ( which == newSoldier)
				{
					//place soldier to enemy's spawn
				}
				else if (which == twoPoints)
				{
				}
				else if (which == end)
				{
				}
				else if (which == youwin)
				{
				}
				mNet.lockOn();
			}
		}
		else
		{
			CCLOG("read error");
			auto err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				CCLOG("he GGed!!!");
			}
		}
	}
}

void GameScene::update(float delta)
{
	//move
	Vec2 mapP = mTiledMapLayer->getPosition();
	//CCLOG("mapP %f,%f", mapP.x, mapP.y);
	Size mapS = mTiledMapLayer->getMapSizeF();
	//CCLOG("mapS %f,%f", mapS.width, mapS.height);
	if ( mKeyStruct.w && (mapP.y >= mWinHeight - mapS.height - mWinHeight / 2 + moveDis /* - mapS.height + moveDis - 50*/) )
	{
		mTiledMapLayer->setPosition(mapP.x, mapP.y - moveDis);
		mapP = mTiledMapLayer->getPosition();
	}
	if ( mKeyStruct.s && (mapP.y <= 0 + mWinHeight / 2 - moveDis /* - moveDis + 280*/) )
	{
		mTiledMapLayer->setPosition(mapP.x, mapP.y + moveDis);
		mapP = mTiledMapLayer->getPosition();
	}
	if ( mKeyStruct.a && (mapP.x <= 0 + mWinHeight / 2 - moveDis/* - moveDis*/) )
	{
		mTiledMapLayer->setPosition(mapP.x + moveDis, mapP.y);
		mapP = mTiledMapLayer->getPosition();
	}
	if ( mKeyStruct.d && (mapP.x >= mWinWidth - mapS.width - mWinHeight / 2 + moveDis/* - mapS.width + moveDis*/) )
	{
		mTiledMapLayer->setPosition(mapP.x - moveDis, mapP.y);
		mapP = mTiledMapLayer->getPosition();
	}
	Vec2 position = Vec2((mWinWidth / 2 - mTiledMapLayer->getPosition().x) / mTiledMapLayer->getMapSizeF().width,(mWinHeight / 2 - mTiledMapLayer->getPosition().y) / mTiledMapLayer->getMapSizeF().height);
	//setViewPosition
	mMiniMapLayer->setViewPosition(position);

	//timer
	mTimer->refresh(delta);
	if (mTimer->isEnded())
	{
		switchTurn();
	}
}

void GameScene::switchTurn()
{
	//end turn
	mBlueTurn = !mBlueTurn;
	//refresh NumTurn
	if (mBlueTurn)
	{
		++mNumTurn;
		std::stringstream ss;
		ss << mNumTurn;
		mTurnLabel->setString(ss.str());
	}
	CCLOG("changed turn now %s", mBlueTurn ? "blue" : "red");
	//start turn
	if (mGameMode == vsPlayer)
	{
		mUnitCampLayer->setVisible(false);
		mTechTreeLayer->setVisible(false);
		int tF = mBlueTurn ? 0 : 1;
		CCLOG("vsPlayer,tF: %d", tF);
		checkTechFactory(tF);
		checkUnitFactory(tF);
		refreshTechTree(tF);
		refreshUnitCamp(tF);
		refreshResourcesIcons(tF);
		refreshMakingButton(tF);
		mTimer->start();
		mTimer->setTimerColor(tF);
		//refresh 2 layer display from gamestate
		//start a new turn
	}
	else if (mGameMode == server || mGameMode == client)
	{
		//send end
		while (!mNet.sendEnd())
		{
			auto err = WSAGetLastError();
			if (err != WSAEWOULDBLOCK)
			{
				mDirector->popScene();
			}
		}
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
		checkTechFactory(tF);
		checkUnitFactory(tF);
		refreshTechTree(tF);
		refreshUnitCamp(tF);
		refreshResourcesIcons(tF);
		refreshMakingButton(tF);
		//timer
		if (mOperateEnable)
		{
			mTimer->start();
			mTimer->setTimerColor(tF);
		}
		else
		{
			mTimer->shutDown();
		}
	}
	mTechTreeLayerButton->setTexture(mTechTreeLayerButtonTexture.off);
	mUnitCampLayerButton->setTexture(mUnitCampLayerButtonTexture.off);
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
					mDirector->popScene();
				}
			}
		}
	}
}

void GameScene::checkUnitFactory(int turnFlag)
{
	mUnitFactory[turnFlag].refresh(mResources[turnFlag].numProductivity);
	if (mUnitFactory[turnFlag].finished() && (!spawnOccupied(turnFlag)) )
	{
		//getUnit
		auto newUnit = mUnitFactory[turnFlag].getFinishedUnit();
		//spawn it 
		spawnUnit(newUnit,turnFlag);
		//add population
		mPopulation[turnFlag] += (mUnitInitDataMap[newUnit].property.numPopulation + mGameState[turnFlag].extraProperty[newUnit].numPopulation);
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
					mDirector->popScene();
				}
			}
		}
	}
}

//need to be tested
void GameScene::moveUnit(std::vector<MyPointStruct> path, int turnFlag)
{
	if (path.size() < 2)
	{
		CCLOG("invalid path");
		return;
	}
	bool oeBak = mOperateEnable;
	mOperateEnable = false;
	const float duration = 0.3;
	float disX = mTiledMapLayer->getTileSize().width;
	float disY = mTiledMapLayer->getTileSize().height;
	//actions
	auto moveUp = MoveBy::create(duration, Vec2(disY, 0));
	auto moveDown = MoveBy::create(duration, Vec2(- disY, 0));
	auto moveLeft = MoveBy::create(duration, Vec2(- disX, 0));
	auto moveRight = MoveBy::create(duration, Vec2(disX, 0));
	auto rotateRight = RotateBy::create(0, 90);
	auto rotateLeft = RotateBy::create(0, -90);
	//get unit sprite;
	Sprite * unit;
	bool found = false;
	for (auto & i : mGameState[turnFlag].unitMap)
	{
		if (i.first == path[0])
		{
			unit = i.second.sprite;
			found = true;
		}
	}
	//error
	if (!found)
	{
		CCLOG("error, unit not found!");
		return;
	}
	//final position in Node
	Vec2 finalP = mTiledMapLayer->floatNodeCoorForPosition(path[path.size() - 1]);
	//sequence vector
	Vector<FiniteTimeAction*> actionVector;
	for (int i = 0; i < path.size() - 1; ++i)
	{
		auto & thisP = path[i];
		auto & nextP = path[i + 1];
		//check direction
		if ( (thisP.x == nextP.x) && (thisP.y < nextP.y) )
		{
			//move up
			actionVector.pushBack(moveUp);
			continue;
		}
		else if ((thisP.x == nextP.x) && (thisP.y > nextP.y))
		{
			//move down
			actionVector.pushBack(moveDown);
			continue;
		}
		else if ((thisP.y == nextP.y) && (thisP.x > nextP.x))
		{
			//move left
			actionVector.pushBack(moveLeft);
			continue;
		}
		else if ((thisP.y == nextP.y) && (thisP.x < nextP.x))
		{
			//move left
			actionVector.pushBack(moveRight);
			continue;
		}
	}
	//sequence
	auto fixPosition = CallFunc::create([&]()->void{unit->setPosition(finalP); });
	auto unLockOE = CallFunc::create([&]()->void{mOperateEnable = true; });
	actionVector.pushBack(fixPosition);
	actionVector.pushBack(unLockOE);
	auto sequence = Sequence::create(actionVector);
	unit->runAction(sequence);
}

//testing
void GameScene::spawnUnit(UnitEnum unit, int turnFlag)
{
	Unit newUnit = {
		unit,
		UnitPropertyStruct(mUnitInitDataMap[unit].property + mGameState[turnFlag].extraProperty[unit] ),
		UnitStateEnum::fresh,
		[&]()->Sprite*
		{
			auto sprite = Sprite::createWithTexture(mUnitTextureMap[turnFlag][unit].front);
			sprite->setPosition(mTiledMapLayer->floatNodeCoorForPosition(mSpawn[turnFlag]));
			mTiledMapLayer->addChild(sprite,2);
			return sprite;
		}()
		};
	//add to unitMap
	mGameState[turnFlag].unitMap[mSpawn[turnFlag]] = newUnit;
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
				switchTurn();
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
		} while (0);
	}
}

void GameScene::unlockTechTree(const int & flag, TechEnum tech)
{
	mGameState[flag].techTree.unlock(tech);
	//CCLOG("ssss: %d", mGameState[flag].techTree.isUnlocked(tech));
	setTechInfluence(flag,tech);
	refreshTechTree(flag);
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
			if (mType == "numRangeAttack")
			{
				mGameState[flag].extraProperty[longrangeunit1].numRangeAttack += mValue;
				mGameState[flag].extraProperty[longrangeunit2].numRangeAttack += mValue;
				mGameState[flag].extraProperty[longrangeunit3].numRangeAttack += mValue;
			}
			if (mType == "numRangeMove")
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
			if (mType == "numRangeAttack")
			{
				mGameState[flag].extraProperty[shortrangeunit1].numRangeAttack += mValue;
				mGameState[flag].extraProperty[shortrangeunit2].numRangeAttack += mValue;
			}
			if (mType == "numRangeMove")
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
		return;
	}
	else
	{
		if (mType == "numResearchLevel")
		{
			CCLOG("numResearchLevelAdded");
			mExtraResources[flag].numResearchLevel += mValue;
		}
		if (mType == "numProductivity")
		{
			CCLOG("numProductivityAdded");
			mExtraResources[flag].numProductivity += mValue;
		}
	}
	return;
}

void GameScene::refreshTechTree(const int & flag)
{
	for (const auto & i : mTechEnumList)
	{
		if (mGameState[flag].techTree.unlockable(i))
		{
			//CCLOG("tech a: %d", i);
			mTechTreeLayer->setTechState(i, available);
		}
		else if(mGameState[flag].techTree.isUnlocked(i))
		{
			//CCLOG("tech un: %d", i);
			mTechTreeLayer->setTechState(i, unlocked);
		}
	}
	//waiting for factory coming soon~
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
	ss << resources.numProductivity + mExtraResources[turnFlag].numProductivity;
	mProductivityLabel->setString(ss.str());
	ss.clear();
	ss.str("");
	ss << resources.numResearchLevel + mExtraResources[turnFlag].numResearchLevel;
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
		mUnitMakingButton->setVisible(true);
		mUnitMakingButton->setTexture(mUnitCampLayer->getUnitTexture(mUnitFactory[turnF].getMakingUnit()));
	}
	else
	{
		mUnitMakingButton->setVisible(false);
	}
	if (mTechFactory[turnF].techExistence())
	{
		mTechMakingButton->setVisible(true);
		mTechMakingButton->setTexture(mTechTreeLayer->getTechTexture(mTechFactory[turnF].getMakingTech()));
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
		checkBackToMainSceneItemOnMouseMoved();
	}
	checkLayersOnMouseMoved();
}

void GameScene::startGame()
{
	//
	if (mGameMode == server || mGameMode == client)
	{
		mWelcomeLayer->setVisible(false);
	}
	initGameState();
	//turn
	if (mGameMode != client)
	{
		mBlueTurn = true;
		mOperateEnable = true;
	}
	else
	{
		mBlueTurn = false;
		mOperateEnable = false;
	}
	//test
	//spawnUnit(farmer, 0);
	spawnUnit(farmer, 1);
	//refresh minimap
	refreshMiniMap();
	//Test for info map
	mInfoMapLayer->displayTech("TECH", "FUCK YOU\nLIU QI!!\nAND FUCK YOUR MOTHER AND FATHER AND SISTER AND BROTHER", stringPredict + std::to_string(100) + stringTurn);
	//update
	mTimer->start();
	scheduleUpdate();

	//TechTreeLayerRefreshing
	if (mGameMode == server || mGameMode == vsPlayer)
	{
		refreshTechTree(0);
	}
	else if (mGameMode == client)
	{
		refreshTechTree(1);
	}

	//net update, 0.5s
	if (mGameMode == server || mGameMode == client)
	{
		schedule(schedule_selector(GameScene::netUpdate), 0.5, CC_REPEAT_FOREVER, 0);
	}
}

void GameScene::refreshMiniMap()
{
	std::set<MyPointStruct> unitSet[2];
	for (int k = 0; k < 2; ++ k)
	{
		for (const auto & i : mGameState[k].unitMap)
		{
			unitSet[k].insert(i.first);
		}
		//base
		unitSet[k].insert(mBasePosition[k]);
		for (auto i : getNearPoint(mBasePosition[k]))
		{
			unitSet[k].insert(i);
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
	mMiniMapLayer->refresh(unitSet[0], unitSet[1], fixedRSet, randomRSet);
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
			if (!mTechFactory[tF].techExistence())
			{
				//add to factory
				if (mResources[tF] >= mTechInitDataMap[tech])
				{
					mResources[tF] -= mTechInitDataMap[tech];
					mTechFactory[tF].addNewTech(tech);
					mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(tech);
					refreshMakingButton(tF);
					refreshResourcesIcons(tF);
				}
			}
		}
	}
	else if (mGameMode == vsPlayer)
	{
		int tF = mBlueTurn ? 0 : 1;
		if (!mTechFactory[tF].techExistence())
		{
			if (mResources[tF] >= mTechInitDataMap[tech])
			{
				mResources[tF] -= mTechInitDataMap[tech];
				//mPopulation[tF] += mUnitInitDataMap[unit].property.numPopulation;
				mTechFactory[tF].addNewTech(tech);
				mTechMakingButtonTexture = mTechTreeLayer->getTechTexture(tech);
				refreshMakingButton(tF);
				CCLOG("vsPlayer; added new Tech!");
				refreshResourcesIcons(tF);
			}
		}
	}
}

void GameScene::checkUnitCampLayerOnTouchEnded()
{
	auto unit = mUnitCampLayer->getunitMouseOn();
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
		}
	}
}

void GameScene::checkMakingButtonOnMouseMoved()
{
	if (mUnitMakingButton->isVisible())
	{
		//unit
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
		//
	} while (0);
}

void GameScene::checkTechAndUnitButton()
{
	//open/close techTreeLayer/UnitCampLayer
	if (mTechTreeLayerButton->boundingBox().containsPoint(mMouseCoordinate))
	{
		if (!mTechTreeLayer->isVisible())
		{
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

void GameScene::initGameState()
{
	//num turn
	mNumTurn = 0;
	//spawn
	mSpawn[0] = MyPointStruct{ 4, 17 };
	mSpawn[1] = MyPointStruct{ 17, 4 };
	//population
	mPopulation[0] = mPopulation[1] = 0;
	//¹²Í¨
	initUnitData();
	initTechData();
	initResourceMap();
	//set unit resources and property in campLayer
	for (const auto & i : mUnitInitDataMap)
	{
		mUnitCampLayer->setUnitResourceAndProperty(i.first, i.second.consumption, i.second.property);
	}
	//mResources
	mResources[0] = mResources[1] = ResourcesStruct{ 100, 100, 10, 10 };
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
	//Ë«±ß
	TechTree techTree;
	//CCLOG("techroot? %d",techTree.isUnlocked(techroot));
	std::map<MyPointStruct, Unit> unitMap;
	std::map<UnitEnum, UnitPropertyStruct> extraProperty;
	std::map<UnitEnum, bool> unitLockMap = {
		{farmer, false},
		{shortrangeunit1, true},
		{shortrangeunit2, true},
		{longrangeunit1, true},
		{longrangeunit2, true},
		{longrangeunit3, true},
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
		mUnitFactory[i].setUnitTime(farmer,mUnitCampLayer->getUnitResources(farmer).numProductivity);
		mUnitFactory[i].setUnitTime(shortrangeunit1,mUnitCampLayer->getUnitResources(shortrangeunit1).numProductivity);
		mUnitFactory[i].setUnitTime(shortrangeunit2,mUnitCampLayer->getUnitResources(shortrangeunit2).numProductivity);
		mUnitFactory[i].setUnitTime(longrangeunit1,mUnitCampLayer->getUnitResources(longrangeunit1).numProductivity);
		mUnitFactory[i].setUnitTime(longrangeunit2,mUnitCampLayer->getUnitResources(longrangeunit2).numProductivity);
		mUnitFactory[i].setUnitTime(longrangeunit3,mUnitCampLayer->getUnitResources(longrangeunit3).numProductivity);
	}
	for (int i = 0; i < 2; i++)
	{
		for (auto refreshTech : mTechEnumList)
		{
			mTechFactory[i].setTechTime(refreshTech, mTechInitDataMap[refreshTech].numResearchLevel);
		}
	}
	//init 
	stringTurn = getDicValue("Turn");
	stringPredict = getDicValue("Predict");
	stringGoing = getDicValue("Going");

}

void GameScene::initResourceMap()
{
	//const float ranScale = 0.5;
	//const float fixScale = 0.5;
	//fixed: json
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("dictionary/mapinitialize.json");
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string( (const char *)loadStr, size );

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
	rapidjson::Value & mapElements= jDocument["mapelement"];
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
			CCLOG("i = %d, ranP: %d,%d",i, ranP.x, ranP.y);
			bool occupied = false;
			//near spawn
			for (const auto & spawn : mSpawn)
			{
				if ((spawn.x == ranP.x) && (spawn.y == ranP.y))
				{
					occupied = true;
					break;
				}
				for (auto p : getNearPoint(spawn))
				{
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
						auto err = WSAGetLastError();
						if (err != WSAEWOULDBLOCK)
						{
							CCLOG("he GGed so fast!!!");
							mDirector->popScene();
						}
					}
					CCLOG("sended. %d,%d", ranP.x, ranP.y);
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
			while (!mNet.sendEnd());
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
			while (!mNet.read())
			{
				auto err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					CCLOG("he GGed so fast!!!");
					mDirector->popScene();
				}
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
			if (mNet.getWhich() == end)
				break;
		}
		CCLOG("read end");
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
		}
	}
}

void GameScene::initUnitData()
{
	auto jsonFile = FileUtils::getInstance()->fullPathForFilename("dictionary/unitdata.json");
	ssize_t size = 0;
	unsigned char * loadStr = FileUtils::getInstance()->getFileData(jsonFile, "r", &size);
	std::string jsonStr = std::string( (const char *)loadStr, size );

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
			std::string type = unit["unit"].GetString();
			//CCLOG("type: %s", type.c_str());
			if (type == "base")
			{
				mUnitInitDataMap[base] = data;
				continue;
			}
			if (type == "farmer")
			{
				mUnitInitDataMap[farmer] = data;
				continue;
			}
			if (type == "shortrangeunit1")
			{
				mUnitInitDataMap[shortrangeunit1] = data;
				continue;
			}
			if (type == "shortrangeunit2")
			{
				mUnitInitDataMap[shortrangeunit2] = data;
				continue;
			}
			if (type == "longrangeunit1")
			{
				mUnitInitDataMap[longrangeunit1] = data;
				continue;
			}
			if (type == "longrangeunit2")
			{
				mUnitInitDataMap[longrangeunit2] = data;
				continue;
			}
			if (type == "longrangeunit3")
			{
				mUnitInitDataMap[longrangeunit3] = data;
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
	auto resourcesIcons = Node::create();
	auto fixedResourceIcon = Sprite::create("uiComponent/icon_gravity.png");
	fixedResourceIcon->setPosition(0, 0);
	resourcesIcons->addChild(fixedResourceIcon);
	mFixedResourceLabel = Label::createWithTTF("", "fonts/STXIHEI.TTF", iconsFontSize);
	mFixedResourceLabel->setColor(Color3B(0, 0, 0));
	mFixedResourceLabel->setPosition(fixedResourceIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mFixedResourceLabel);

	auto randomResourceIcon = Sprite::create("uiComponent/icon_hydrogen.png");
	randomResourceIcon->setPosition(offset, 0);
	resourcesIcons->addChild(randomResourceIcon);
	mRandomResourceLabel = Label::createWithTTF("", "fonts/STXIHEI.TTF", iconsFontSize);
	mRandomResourceLabel->setColor(Color3B(0, 0, 0));
	mRandomResourceLabel->setPosition(randomResourceIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mRandomResourceLabel);

	auto productivityIcon= Sprite::create("uiComponent/icon_productivity.png");
	productivityIcon->setPosition(offset * 2, 0);
	resourcesIcons->addChild(productivityIcon);
	mProductivityLabel = Label::createWithTTF("","fonts/STXIHEI.TTF", iconsFontSize);
	mProductivityLabel->setColor(Color3B(0, 0, 0));
	mProductivityLabel->setPosition(productivityIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mProductivityLabel);

	auto researchIcon= Sprite::create("uiComponent/icon_researchlevel.png");
	researchIcon->setPosition(offset * 3, 0);
	resourcesIcons->addChild(researchIcon);
	mResearchLabel = Label::createWithTTF("", "fonts/STXIHEI.TTF", iconsFontSize);
	mResearchLabel->setColor(Color3B(0, 0, 0));
	mResearchLabel->setPosition(researchIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mResearchLabel);

	auto populationIcon= Sprite::create("uiComponent/icon_cpu.png");
	populationIcon->setPosition(offset * 4, 0);
	resourcesIcons->addChild(populationIcon);
	std::stringstream ssP;
	ssP << "0/" << mPopulationLimit;
	mPopulationLabel = Label::createWithTTF(ssP.str(), "fonts/STXIHEI.TTF", iconsFontSize);
	mPopulationLabel->setColor(Color3B(0, 0, 0));
	mPopulationLabel->setPosition(populationIcon->boundingBox().getMaxX() + offset / 2 - offset2, 0);
	resourcesIcons->addChild(mPopulationLabel);

	resourcesIcons->setScale(0.6);
	resourcesIcons->setPosition(mWinWidth / 2 - 2.5 * offset * resourcesIcons->getScale(), mWinHeight - (fixedResourceIcon->boundingBox().getMaxY() - fixedResourceIcon->boundingBox().getMinY())/2);
	addChild(resourcesIcons, 3);
}

void GameScene::initGameMenu()
{
	auto youWinLabel = Label::createWithTTF(getDicValue("youWin"), "fonts/STXIHEI.TTF", 30);
	auto GGLabel = Label::createWithTTF(getDicValue("GG"), "fonts/STXIHEI.TTF", 30);
	auto youWinItem = MenuItemLabel::create(youWinLabel, [&](Ref * sender)->void{
		if (mGameMode == server || mGameMode == client)
		{
			while (!mNet.sendYouWin())
			{
				auto err = WSAGetLastError();
				if (err != WSAEWOULDBLOCK)
				{
					CCLOG("he GG!!");
					mDirector->popScene();
				}
			}
		}
		//lose
		CCLOG("i lose!");
	});
	auto GGItem = MenuItemLabel::create(GGLabel, [&](Ref * sender)->void{
		mDirector->popScene();
		//autorelease();
	});
	auto menuBg = DrawNode::create();
	menuBg->drawSolidRect(Vec2(mWinWidth / 2 - 100, mWinHeight / 2 - 150), Vec2(mWinWidth / 2 + 100, mWinHeight / 2 + 150), Color4F(0.607, 0.607, 0.607, 0.75));
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
	auto watingLabel = Label::createWithTTF("no word", "/fonts/STXIHEI.TTF", 30);
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
	addChild(mWelcomeLayer, 6);
}

void GameScene::initYypNet()
{
	//yypNet
	switch (mGameMode)
	{
	case server:
		mNet.startServer(mUserDefault->getIntegerForKey("port"));
		schedule(schedule_selector(GameScene::acceptConnect),1,CC_REPEAT_FOREVER,0);
		break;
	case client:
		//display juFlower
		//mNet.makeConnect((char *)(mUserDefault->getStringForKey("ip").c_str()), mUserDefault->getIntegerForKey("port"));
		schedule(schedule_selector(GameScene::startConnecting),1,CC_REPEAT_FOREVER,0);
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
			mDirector->getTextureCache()->addImage("unitIcon/farmer_front_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/farmer_back_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/farmer_side_" + std::string{ (char)('0' + i) }+".png")
		};
		mUnitTextureMap[i][longrangeunit1] = {
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit1_front_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit1_back_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit1_side_" + std::string{ (char)('0' + i) }+".png")
		};
		mUnitTextureMap[i][longrangeunit2] = {
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit2_front_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit2_back_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit2_side_" + std::string{ (char)('0' + i) }+".png")
		};
		mUnitTextureMap[i][longrangeunit3] = {
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit3_front_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit3_back_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/longrangeunit3_side_" + std::string{ (char)('0' + i) }+".png")
		};
		mUnitTextureMap[i][shortrangeunit1] = {
			mDirector->getTextureCache()->addImage("unitIcon/shortrangeunit1_front_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/shortrangeunit1_back_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/shortrangeunit1_side_" + std::string{ (char)('0' + i) }+".png")
		};
		mUnitTextureMap[i][shortrangeunit2] = {
			mDirector->getTextureCache()->addImage("unitIcon/shortrangeunit2_front_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/shortrangeunit2_back_" + std::string{ (char)('0' + i) }+".png"),
			mDirector->getTextureCache()->addImage("unitIcon/shortrangeunit2_side_" + std::string{ (char)('0' + i) }+".png")
		};
	}
	CCLOG("munittexturemap size: %d", mUnitTextureMap[0].size());
}

