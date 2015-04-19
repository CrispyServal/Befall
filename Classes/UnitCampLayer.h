#ifndef UNITCAMPLAYER_H
#define	UNITCAMPLAYER_H

#include "cocos2d.h"
#include <map>
#include "MyEnums.h"

using namespace cocos2d;

class UnitCampLayer : public Layer
{
public:
	UnitCampLayer();
	~UnitCampLayer();
	virtual bool init();
	CREATE_FUNC(UnitCampLayer);
	//�ڹ���ڰ�ť����ʱ��ŷ���true
	bool containPoint(Vec2 mousePoint);
	bool blockClick = true;
	//���ر���İ�ť���������������������ťʱ����
	UnitEnum getClickedUnit(Vec2 mousePoint);
	//��������ͣʱ��ı䰴ťͼƬ
	void onMouseMoved(Vec2 mousePoint);
	//չʾ��ͬ���ݵĽӿ�
	void displayUnitInfo(const std::string & imageFileName, const std::string & unitName, const std::string & unitIntroduction);
private:
	//��ͬ���İ�ť�����˾�����ġ�
	std::map<UnitEnum, Sprite *> mUnitButtonMap;
	//չʾ��λ�Ĵ�ͼ
	Sprite * mUnitImage;
	//չʾ��λ���ƺ���ϸ��Ϣ�ı�ǩ
	Label * mUnitName;
	Label * mUnitIntroduction;
};


#endif // UNITCAMPLAYER_H

/**
	������ѯ����
	Label ���� createTTF ���ɣ�����ʹ��STXIHEI.TTF
*/