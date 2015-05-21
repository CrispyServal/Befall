#include "MainScene.h"

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

bool MainScene::init()
{
	if (!Scene::init())
	{
		return false;
	}
	srand(static_cast<unsigned>(time(NULL)));
	//
	pressed = false;
	//userdefault
	if (!userDefault->getBoolForKey("isExisted"))
	{
		userDefault->setBoolForKey("isExisted", true);
		userDefault->setBoolForKey("musicOn", true);
		userDefault->setBoolForKey("seOn", true);
		userDefault->setStringForKey("language", "ch");
	}
	//dictionary = Dictionary::createWithContentsOfFile("ch.xml");
	dictionary = Dictionary::createWithContentsOfFile(std::string{ "dictionary/" + userDefault->getStringForKey("language") + ".xml" }.c_str());
	dictionary->retain();
	//layer
	mainLayer = Layer::create();
	addChild(mainLayer);
	//director
	director = Director::getInstance();
	vHeight = director->getVisibleSize().height;
	vWidth= director->getVisibleSize().width;
	//bg
	bgSprite = Sprite::create("uiComponent/bg.png");
	bgSprite->setPosition(vWidth / 2, vHeight / 2);
	bgSprite->setColor(Color3B(120,120,120));
	mainLayer->addChild(bgSprite, -2);
	//label
	initLabelPAK();
	//ball
	ball = Node::create();
	ball->setPosition(vWidth / 2, vHeight / 2);
	Sprite * ring[11];
	RotateBy * ringRotate[11];
	RepeatForever * ringRepeat[11];
	for (int i = 0; i < 11; ++ i)
	{
		std::stringstream ss;
		ss << "ring/ring" << i << ".png";
		ring[i] = Sprite::create(ss.str());
		//ring[i] = Sprite::create(std::string{"ring/ring0.png"});
		//delta angle
		int de;
		if (CCRANDOM_0_1() > 0.5)
		{
			de = 10;
		}
		else
		{
			de = -10;
		}
		//duration
		int ringSpeed = (4) / (CCRANDOM_0_1() * 3 + 1);
		//CCLOG("%f", ringSpeed);
		ringRotate[i] = RotateBy::create(ringSpeed, de);
		ringRepeat[i] = RepeatForever::create(ringRotate[i]);
		ring[i]->runAction(ringRepeat[i]);
		//rotate
		ball->addChild(ring[i]);
	}
	float scale = (vHeight / 3 * 2) / ring[10]->getContentSize().height;
	ball->setScale(scale);
	mainLayer->addChild(ball, 0);
	//dispatcher
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	//listener
	mouseListener = EventListenerMouse::create();
	mouseListener->onMouseMove = CC_CALLBACK_1(MainScene::onMouseMove,this);
	touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	//test
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
	//
	dispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	//
	dispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
	//
	//scheduleUpdate();
	return true;
}

void MainScene::update(float delta)
{
	//debug
}


Rect getNodeRect(Node * node)
{
	auto pos = node->getPosition();
	auto size = node->getContentSize();
	auto anchorPoint = node->getAnchorPoint();
	auto rect = Rect(
		pos.x - anchorPoint.x * size.width,
		pos.y - anchorPoint.y * size.height,
		size.width,
		size.height);
	return rect;
}

void MainScene::onMouseMove(Event * event)
{
	//get coordinate
	auto e = static_cast<EventMouse *>(event);
	mMouseCoordinate.x = e->getLocation().x;
	mMouseCoordinate.y = 2 * vHeight - e->getLocation().y;
	//
	if (!pressed)
	{
		//PAK的小效果 鼠标悬停动画停止
		if (labelPAK->boundingBox().containsPoint(mMouseCoordinate))
		{
			director->getActionManager()->pauseTarget(labelPAK);
		}
		else
		{
			director->getActionManager()->resumeTarget(labelPAK);
		}
	}
	else
	{
	}
}

bool MainScene::onTouchBegan(Touch * touch, Event * event)
{
	return true;
}

void MainScene::onTouchMoved(Touch * touch, Event * event)
{
}

void MainScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	displayMainMenu();
	//CCLOG("KeyCode: %d", keyCode);
}

void MainScene::onTouchEnded(Touch * touch, Event * event)
{
	displayMainMenu();
}

void MainScene::displayMainMenu()
{
	if (!pressed)
	{
		labelPAK->removeFromParentAndCleanup(true);
		auto ballBiger = ScaleTo::create(0.7, 0.6);
		auto ease = EaseExponentialOut::create(ballBiger);
		ball->runAction(ease);
		//add menu
		mainMenu = MainMenu::create();
		addChild(mainMenu, 2);
		//pressed to true
		pressed = true;
	}
}

void MainScene::initLabelPAK()
{
	//press any key
	labelPAK = Label::createWithTTF(dictionary->valueForKey("pak")->getCString(), "fonts/STXIHEI.TTF", 25);
	//labelPAK = Label::createWithTTF(strUTF8("按任意键继续......"),"fonts/STXIHEI.TTF", 25);
	labelPAK->setColor(Color3B(78, 95, 101));
	labelPAK->setPosition(vWidth / 2, 1.0 * labelPAK->getContentSize().height);
	labelPAK->enableGlow(Color4B(255, 255, 255, 255));
	auto pakScaleBy = ScaleBy::create(1, 1.111111);
	auto pakScaleBy2 = ScaleBy::create(1, 0.9);
	auto pakSequence = Sequence::create(pakScaleBy, pakScaleBy2, NULL);
	auto pakRepeat = RepeatForever::create(pakSequence);
	labelPAK->runAction(pakRepeat);
	mainLayer->addChild(labelPAK,1);
}

/*
{
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* result = new char[len + 1];
	memset(result, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, result, len, NULL, NULL);
	if (wstr)delete[] wstr;
	return result;
}
*/