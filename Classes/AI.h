
#ifndef __AI_H__
#define __AI_H__

#include "Common.h"

class AI : public CCNode
{
public:
	CREATE_FUNC(AI);
	bool init();
	void onExit();

	CCTMXTiledMap* getMap()
	{
		return (CCTMXTiledMap*)getParent();
	}

	// 产生敌军坦克
	void createTank(float);
	bool _createFlag;

	CCArray* _tanks;
	CCArray* _bullets;

	// 移动敌军坦克
	void moveTank(float);

	// 让敌军坦克发射子弹
	void Shoot(float);

	// 碰撞检测
	void update(float);

	bool			_pause;
	void pause();
	void resume(float);
	
};

#endif

