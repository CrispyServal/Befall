#ifndef MAINMENU_H
#define MAINMENU_H

//#include "usefulFuncs.h"
#include "cocos2d.h"
//#include "cocos-ext.h"
#include "MyEnums.h"
#include <map>
#include <set>
#include "json/document.h"

//using namespace cocos2d;
//USING_NS_CC_EXT;
USING_NS_CC;

#include "GameScene.h"
#include "MiniMapLayer.h"

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
	Menu * mapMenu;
	Layer * serverLayer;
	Layer * clientLayer;
	Label * editBoxInServer;
	Label * editBoxInClient;
	MenuItemLabel * confirm;
	std::map<std::string, MenuItemLabel *> items;
	std::map<std::string, Menu *> menuMap;
	std::set<GameModeEnum> closeModeSet;
	Label * mTitle;
	Label * mVersionLabel;
	//creates
	Label * createMenuLabel(char * str);

	std::vector<string> mapNameArray;
	//MiniMap
	MiniMapLayer * mMiniMapLayer;
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
	void displayMapCallBack(Ref * sender, const std::string & mapName);
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

	void playEffect(const char * filePath);
	void playBackgroundMusic(const char * filePath);

	//map preview
	std::vector<MyPointStruct> mBasePosition;
	void refreshMiniMap(std::string mapName);
	std::map<MyPointStruct, Unit> mResourceMap;
	void refreshMiniMap();
};

#endif