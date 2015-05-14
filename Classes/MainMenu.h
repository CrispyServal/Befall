#ifndef MAINMENU_H
#define MAINMENU_H

//#include "usefulFuncs.h"
#include "cocos2d.h"
//#include "cocos-ext.h"
#include "MyEnums.h"
#include <map>
#include <set>

//using namespace cocos2d;
//USING_NS_CC_EXT;
USING_NS_CC;

#include "GameScene.h"

class MainMenu : public Layer
{
public:
	MainMenu();
	~MainMenu();
	CREATE_FUNC(MainMenu);
	virtual bool init();
private:
	//global
	UserDefault * userDefault;
	Dictionary * dictionary;
	//Menus
	Menu * rootMenu;
	Menu * startMenu;
	Menu * settingMenu;
	Menu * exitMenu;
	Menu * netMenu;
	Layer * serverLayer;
	Layer * clientLayer;
	Label * editBoxInServer;
	Label * editBoxInClient;
	std::map<std::string, MenuItemLabel *> items;
	std::map<std::string, Menu *> menuMap;
	std::set<GameModeEnum> closeModeSet;
	Label * mTitle;
	//creates
	Label * createMenuLabel(char * str);
	//listener
	EventListenerMouse * mouseListener;
	EventListenerKeyboard * keyboardListener;
	//callbacks
	void enterCallback(Ref * sender, const std::string & thisMenu, const std::string & rightMenu, int nowDeep);
	void backCallback(Ref * sender, const std::string & thisMenu, const std::string & leftMenu, int nowDeep);
	//void enterGameCallback(Ref * sender, GameModeEnum gamemode);
	void enterGameCallback(GameModeEnum gamemode);
	void settingCallback(Ref * sender, const std::string & setting);
	void exitCallback(Ref * sender);
	void displayEditBoxCallback(Ref * sender, const std::string & mode);
	void cancelCallback(Ref * sender, const std::string & thisMenu);
	void onMouseMove(Event * evnet);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event * event);
	//lock
	bool menuOpen[3];
	//debug
	//char * MainMenu::strUTF8(const char * str);
	//useful
	std::string getDicValue(char * str);
	bool checkClientInput(std::string & ipAndPortStr);
};

#endif