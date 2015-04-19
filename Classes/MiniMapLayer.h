#ifndef MINIMAPLAYER_H
#define MINIMAPLAYER_H

#include "cocos2d.h"
#include "MyStructs.h"
#include <set>

using namespace cocos2d;

class MiniMapLayer : public Layer
{
public:
	MiniMapLayer();
	~MiniMapLayer();
	virtual bool init();
	CREATE_FUNC(MiniMapLayer);
	//method
	bool containPoint(Vec2 mousePoint);
	//��û��������´���
	bool blockClick = true;
	//������λ�ã�������Ұ�����ڵ�ͼ��С�еı�����ֵ�����緵��0.3��0.42�������������ʱ���á�
	Vec2 getViewPosition(Vec2 mousePoint);
	//ˢ��С��ͼ��4����ɫ�Ķ��������������drawNode�ػ�
	void refresh(
		std::set<MyPointStruct> unitSet0,
		std::set<MyPointStruct> unitSet1,
		std::set<MyPointStruct> fixedResourceSet,
		std::set<MyPointStruct> randomResourceSet
		);
	//ʵ��:�϶���Ұ�򣬲����������Ʒ�Χ��
	void onTouchBegan(Vec2 mousePoint);
	void onTouchMoved(Vec2 mousePoint);
	void onTouchEnded(Vec2 mousePoint);
private:
	//һ��������
	DrawNode * mDrawNode;
	//��Ұ��
	DrawNode * mViewRect;
};

#endif // !MINIMAPLAYER_H
