#include "WelcomeScene.h"

WelcomeScene::WelcomeScene()
{
}

WelcomeScene::~WelcomeScene()
{
}

bool WelcomeScene::init()
{
	if (!Scene::init())
	{
		return false;
	}
	mNextScene = MainScene::create();
	mNextScene->retain();
	for (int i = 0; i < NUMBG; ++i)
	{
		mTextures[i] = TextureCache::getInstance()->addImage("uiComponent/welcomeImage" + std::string{ (char)('0' + i) } +".png");
	}
	//init
	mWelcomeBg = Sprite::createWithTexture(mTextures[0]);
	mWelcomeBg->setPosition(
		Director::getInstance()->getWinSize().width / 2,
		Director::getInstance()->getWinSize().height / 2
		);
	mWelcomeBg->setOpacity(0);
	addChild(mWelcomeBg);
	auto fadein = FadeIn::create(0.3);
	auto delay = DelayTime::create(1);
	auto fadeout = FadeOut::create(0.7);
	auto jumpFunc = CallFunc::create(CC_CALLBACK_0(WelcomeScene::jumpToNextScene, this));
	auto sequence = Sequence::create(fadein, delay, fadeout, NULL);
	auto sequenceR = Sequence::create(
		DelayTime::create(0.5),
		sequence,
		CallFunc::create(CC_CALLBACK_0(WelcomeScene::changeTexture, this,mTextures[1])),
		sequence,
		jumpFunc,
		NULL
		);
	//actions
	//final action
	mWelcomeBg->runAction(sequenceR);

	mKeyboardListener = EventListenerKeyboard::create();
	mKeyboardListener->onKeyReleased = CC_CALLBACK_2(WelcomeScene::onKeyReleased, this);
	mTouchListener = EventListenerTouchOneByOne::create();
	mTouchListener->onTouchBegan = [](Touch * touch, Event * event)->bool{return true; };
	mTouchListener->onTouchEnded = CC_CALLBACK_2(WelcomeScene::onTouchEnded, this);
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	dispatcher->addEventListenerWithSceneGraphPriority(mTouchListener, this);
	dispatcher->addEventListenerWithSceneGraphPriority(mKeyboardListener, this);
	return true;
}

void WelcomeScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event)
{
	if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE || keyCode == EventKeyboard::KeyCode::KEY_SPACE)
	{
		jumpToNextScene();
	}
}

void WelcomeScene::onTouchEnded(Touch * touch, Event * event)
{
	jumpToNextScene();
}

void WelcomeScene::jumpToNextScene()
{
	static bool called = false;
	if (!called)
	{
		called = true;
		Director::getInstance()->replaceScene(TransitionFade::create(0.8, mNextScene));
	}
}

void WelcomeScene::changeTexture(Texture2D * texture)
{
	mWelcomeBg->setTexture(texture);
}