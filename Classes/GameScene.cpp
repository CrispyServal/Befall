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
	if (point.x < MAXSIZE)
	{
		result.push_back(MyPointStruct{ point.x + 1, point.y });
	}
	if (point.y < MAXSIZE)
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
    mDictionary->retain();
	mDispatcher = mDirector->getEventDispatcher();
	mWinHeight = mDirector->getWinSize().height;
	mWinWidth = mDirector->getWinSize().width;
	//gamemode
	mGameMode = static_cast<GameModeEnum>(mUserDefault->getIntegerForKey("gamemode"));
	CCLOG("gameMode: %d", mGameMode);
	//welcome layer : waiting and connecting
	if ((mGameMode == GameModeEnum::client) || (mGameMode == GameModeEnum::server))
	{
		//init juFlower
		mWelcomeLayer = Layer::create();
		auto juFlower = Sprite::create("item1.png");
		juFlower->setScale(0.3);
		juFlower->setPosition(
			mWinWidth / 2,
			mWinHeight / 2);
		auto juRotate = RotateBy::create(1, -30);
		auto juRepaet = RepeatForever::create(juRotate);
		juFlower->runAction(juRepaet);
		//wating label
		auto watingLabel = Label::createWithTTF("fuck", "/fonts/STXIHEI.TTF", 30);
        //CCLOG("%s",watingLabel->getString().c_str());
		if (mGameMode == GameModeEnum::server)
		{
            CCLOG("waiting:  %s",mDictionary->valueForKey("wating")->getCString());
            watingLabel->setString(mDictionary->valueForKey("waiting")->getCString());
		}
		if (mGameMode = GameModeEnum::client)
		{
			watingLabel->setString(mDictionary->valueForKey("connecting")->getCString());
		}
		watingLabel->setPosition(
			mWinWidth / 2,
			2 * juFlower->getBoundingBox().getMaxY() - juFlower->getBoundingBox().getMinY()
			);
		auto backToMainSceneLabel = Label::createWithTTF(mDictionary->valueForKey("cancel")->getCString(), "/fonts/STXIHEI.TTF", 30);
		backToMainSceneItem = MenuItemLabel::create(backToMainSceneLabel, CC_CALLBACK_1(GameScene::backToMainScene, this));
		backToMainSceneItem->setPosition(
			backToMainSceneItem->getPosition().x,
			backToMainSceneItem->getPosition().y - juFlower->getBoundingBox().getMaxY() + juFlower->getBoundingBox().getMinY() - 10
			);
		auto welcomeMenu = Menu::create(backToMainSceneItem, NULL);
		/*
		welcomeMenu->setPosition(
			welcomeMenu->getPosition().x,
			juFlower->getBoundingBox().getMinY() * 2- juFlower->getBoundingBox().getMaxY()
			);
			*/
		mWelcomeLayer->addChild(watingLabel);
		mWelcomeLayer->addChild(welcomeMenu);
		mWelcomeLayer->addChild(juFlower);
		addChild(mWelcomeLayer, 3);
	}
	//yypNet
	switch (mGameMode)
	{
	case server:
		mNet.startServer(mUserDefault->getIntegerForKey("port"));
		//schedule(schedule_selector(GameScene::startServer),0,CC_REPEAT_FOREVER,0.1);
		break;
	case client:
		//display juFlower
		//schedule(schedule_selector(GameScene::startConnecting), 0, CC_REPEAT_FOREVER, 0.1);
		break;
	case vsPlayer:
		break;
	default:
		break;
	}
	//scheduleUpdate();
	//listeners
	mMouseListener = EventListenerMouse::create();
	mMouseListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMoved, this);
	mDispatcher->addEventListenerWithSceneGraphPriority(GameScene::mMouseListener, this);
	return true;
}

void GameScene::startServer(float delta)
{
	//listen
	CCLOG("lisening...");
	mNet.serverListen(mUserDefault->getIntegerForKey("port"));
	if (mNet.beConnected())
	{
		CCLOG("someone connected!");
		unschedule(schedule_selector(GameScene::startServer));
	}
}

void GameScene::startConnecting(float delta)
{
	CCLOG("connecting...");
	if (mNet.makeConnect((char *)mUserDefault->getStringForKey("ip").c_str(), mUserDefault->getIntegerForKey("port")))
	{
		CCLOG("connecting successed");
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
	//cancel's scale effect
	auto box = backToMainSceneItem->boundingBox();
	auto size = backToMainSceneItem->getContentSize();
	auto pos = backToMainSceneItem->getPosition();
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
		backToMainSceneItem->setScale(1.2);
	}
	else
	{
		backToMainSceneItem->setScale(1);
	}

}