#ifndef MAINSCENE_H
#define MAINSCENE_H

#define COCOS2D_DEBUG 1
#include "cocos2d.h"
#include "MainMenu.h"
//#include "usefulFunc.h"
#include <sstream>
using namespace cocos2d;

class MainScene : public Scene
{
public:
	MainScene();
	~MainScene();
    CREATE_FUNC(MainScene);
	virtual bool init();
	void update(float delta);
private:
	//global things
	Director * director;
	UserDefault * userDefault;
	float vHeight;
	float vWidth;
	Dictionary * dictionary;
	//main thing
	Layer * mainLayer;
	MainMenu * mainMenu;
	Node * ball;
	//pressed something, displayed menu
	bool pressed;
	Sprite * bgSprite;
	EventListenerMouse * mouseListener;
	EventListenerTouchOneByOne * touchListener;
	Vec2 mMouseCoordinate;
	Label * labelPAK;
	//method
	void onMouseMove(Event * event);
	bool onTouchBegan(Touch * touch, Event * event);
	void onTouchMoved(Touch * touch, Event * event);
	void onTouchEnded(Touch * touch, Event * event);
	//
	Rect getNodeRect(Node * node);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event);
	void initLabelPAK();
	void displayMainMenu();
};

#endif