# Befall



###请各位自行使用cocos建立新的工程并且自行倒腾git

##几个Layer类的接口标准：
#####1、bool containPoint
	参数为鼠标的坐标，坐标都是左下角为原点的坐标，要求返回本层是否包含了该坐标。
	用途：游戏场景判断光标在某层内时才会调用那个层的后续接口。
#####2、bool blockClick
	如果为true，意义为点击被吞没，显示该层是点击该层以外区域也不会传递点击。