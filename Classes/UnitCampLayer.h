#ifndef UNITCAMPLAYER_H
#define	UNITCAMPLAYER_H

#include "cocos2d.h"
#include <vector>
#include <map>
#include "MyEnums.h"
#include "json\document.h"

using namespace cocos2d;

namespace UnitCamp
{
	struct ItemInCampStruct
	{
		UnitEnum unit;
		Sprite * sprite;
		bool unlocked;
		Texture2D * texture;
		std::string name;
		std::string introduction;
	};
}

class UnitCampLayer : public Layer
{
public:
	UnitCampLayer();
	~UnitCampLayer();
	virtual bool init();
	CREATE_FUNC(UnitCampLayer);
	//�ڹ���ڰ�ť����ʱ��ŷ���true
	bool containPoint(Vec2 mousePoint);
	bool blockClick()
	{
		return true;
	}
	//���ر���İ�ť���������������������ťʱ����
	const UnitEnum getunitMouseOn();
	//��������ͣʱ��ı䰴ťͼƬ
	void onMouseMoved(Vec2 mousePoint);
	//lock on or lock off
	void setUnlocked(UnitEnum unit,bool unlock);
	//ÿ����λ�����ݴ洢
	std::vector<UnitCamp::ItemInCampStruct> mItemsList;
private:
	std::vector<UnitEnum> mUnitsList;
	std::map<UnitEnum, bool> mUnitUnlockMap;
	//չʾ��λ�Ĵ�ͼ
	Sprite * mUnitImage;
	//չʾ��λ���ƺ���ϸ��Ϣ�ı�ǩ
	Label * mUnitName;
	Label * mUnitIntroduction;
	//��͸���ܱ���
	Sprite * mBg;
	//��ǰ���λ���ϵİ�ť
	UnitCamp::ItemInCampStruct mNowItem;
	//�����һ��texture, δ�����ĵ�λ�Ĵ�ͼ
	//Texture2D * mCloseTexture;
};


#endif // UNITCAMPLAYER_H

/**
	������ѯ����
	Label ���� createTTF ���ɣ�����ʹ��STXIHEI.TTF
*/