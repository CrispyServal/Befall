#ifndef WELCOMESCENE_H
#define WELCOMESCENE_H

#include "cocos2d.h"
#include "MainScene.h"

using namespace cocos2d;

//!!!NUMBG < 10
#define NUMBG 2

class WelcomeScene : public Scene
{
public:
	WelcomeScene();
	~WelcomeScene();
	virtual bool init();
	CREATE_FUNC(WelcomeScene);
private:
	Sprite * mWelcomeBg;
	Texture2D * mTextures[NUMBG];
	MainScene * mNextScene;
	void jumpToNextScene();
	void changeTexture(Texture2D * texture);
	EventListenerTouchOneByOne * mTouchListener;
	EventListenerKeyboard * mKeyboardListener;
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event);
	void onTouchEnded(Touch * touch, Event * event);
};

#endif // !WELCOMESCENE_H
