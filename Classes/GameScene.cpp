#include "GameScene.h"


GameScene::GameScene()
{
}

GameScene::~GameScene()
{
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
    //mDictionary->retain();
	mDispatcher = mDirector->getEventDispatcher();
	mWinHeight = mDirector->getWinSize().height;
	mWinWidth = mDirector->getWinSize().width;
	//texture
	initResourceTexture();
	initUnitTexture();
	//srand
	srand(static_cast<unsigned>(time(NULL)));
	//gamemode
	mGameMode = static_cast<GameModeEnum>(mUserDefault->getIntegerForKey("gamemode"));
	CCLOG("gameMode: %d", mGameMode);
	//tiledMapLayer
	mTiledMapLayer = TiledMapLayer::create();
	mMapSize = mTiledMapLayer->getMapSize();
	addChild(mTiledMapLayer,1);
	
	if (mGameMode == vsPlayer)
	{
		startGame();
	}
	else if (mGameMode == server || mGameMode == client)
	{
		initWelcomeLayer();
	}
	initYypNet();
	//place resources
	for (const auto & i : mResourceMap)
	{

	}
	//scheduleUpdate();
	//listeners
	mMouseListener = EventListenerMouse::create();
	mMouseListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMoved, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(GameScene::mMouseListener, this);
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

void GameScene::update(float delta)
{
	CCLOG("update");
}

void GameScene::backToMainScene(Ref * sender)
{
	mDirector->popScene();
	//mDirector->popToRootScene();
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
		checkBackToMainSceneItem();
	}
}

void GameScene::startGame()
{
	//
	if (mGameMode == server || mGameMode == client)
	{
		mWelcomeLayer->setVisible(false);
	}
	initGameState();
}

void GameScene::checkBackToMainSceneItem()
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

void GameScene::initGameState()
{
	//¹²Í¨
	initUnitData();
	initTechData();
	initResourceMap();
	//mResources
	mResources = ResourcesStruct{ 100, 100, 10, 10 };
	mCollectionEffeciency = ResourcesStruct{ 0, 0, 0, 0 };
	//Ë«±ß
	TechTree techTree;
	CCLOG("techroot? %d",techTree.isUnlocked(techroot));
	std::map<MyPointStruct, Unit> unitMap;
	std::map<UnitEnum, UnitPropertyStruct> extraProperty;
	GameStateStruct  gameState = {
		techTree,
		unitMap,
		extraProperty
	};
	mGameState[0] = mGameState[1] = gameState;
}

void GameScene::initResourceMap()
{
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
			if (element == "base")
			{
				//CCLOG("a base!");
				mBasePosition.push_back(point);
				Unit unit = {
					UnitEnum::base,
					mUnitInitDataMap[base].property,
					UnitStateEnum::attacked,
					Sprite::create("item1.png")
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
					Sprite::createWithTexture(mResourceTextureMap[fixedResource].abundant)
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
					Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant)
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
			for (const auto & i : mResourceMap)
			{
				if ((i.first.x == ranP.x) && (i.first.y == ranP.y))
				{
					occupied = true;
					break;
				}
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
					while (!mNet.sendOnePoint(ranP));
					CCLOG("sended. %d,%d", ranP.x, ranP.y);
				}
				mResourceMap[ranP] = Unit{
					UnitEnum::randomResource,
					UnitPropertyStruct{ mHitPointOfRandomResource, 0, 0, 0, 0, 0 },
					UnitStateEnum::attacked,
					Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant)
				};
				++i;
			}
		}
		if (mGameMode == server)
		{
			while (!mNet.sendEnd());
			CCLOG("sended end");
		}
	}
	else
	{
		//client: read ranP
		while (true)
		{
			while (!mNet.read());
			if (mNet.getWhich() == onePoint)
			{
				mResourceMap[mNet.getOnePoint()] = Unit{
					UnitEnum::randomResource,
					UnitPropertyStruct{ mHitPointOfRandomResource, 0, 0, 0, 0, 0 },
					UnitStateEnum::attacked,
					Sprite::createWithTexture(mResourceTextureMap[randomResource].abundant)
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
		i.second.sprite->setPosition(mTiledMapLayer->floatCoorForPosition(i.first));
		addChild(i.second.sprite, 2);
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
	addChild(mWelcomeLayer, 3);
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
