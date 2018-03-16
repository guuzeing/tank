#include "AI.h"
#include "TankEnemy.h"

bool AI::init()
{
	CCNode::init();

	schedule(schedule_selector(AI::createTank), TANK_CREATE_INTERVAL);
	_createFlag = false;

	schedule(schedule_selector(AI::moveTank));

	schedule(schedule_selector(AI::Shoot), TANK_SHOOT_INTERVAL);

	scheduleUpdate();

	_tanks = CCArray::create();
	_tanks->retain();
	_bullets = CCArray::create();
	_bullets->retain();

	_pause = false;

	return true;
}

void AI::onExit()
{
	CCNode::onExit();
	_tanks->release();
	_bullets->release();
}

void AI::pause()
{
	_pause = true;
	// 暂停正在运行的子弹
	CCObject* obj;
	CCARRAY_FOREACH(_bullets, obj)
	{
		Bullet* bullet = (Bullet*)obj;
		bullet->pauseSchedulerAndActions();
	}

	// 启动定时器，到时间恢复
	scheduleOnce(schedule_selector(AI::resume), 10);
}

void AI::resume(float)
{
	_pause = false;
	// 暂停正在运行的子弹
	CCObject* obj;
	CCARRAY_FOREACH(_bullets, obj)
	{
		Bullet* bullet = (Bullet*)obj;
		bullet->resumeSchedulerAndActions();
	}
}

void AI::createTank(float)
{
	// 在暂停状态下，不产生新坦克
	if (_pause)
		return;

	TankEnemy* tank = TankEnemy::create();

	// AI的Parent就是地图,把坦克加入到地图
	getMap()->addChild(tank);

	// 设置格子大小
	tank->setTileSize(tank->getMap()->getTileSize().width);

	// 设置位置
	_createFlag = !_createFlag;
	if (_createFlag)
	{
		tank->setPosition(Common::Tile2PointLB(getMap(), ccp(1, 0)));
	}
	else
	{
		tank->setPosition(Common::Tile2PointLB(getMap(), ccp(25, 0)));
	}

	// 加入到数组
	_tanks->addObject(tank);
}

void AI::moveTank(float)
{
	// 随机算法
	if (_pause)
		return;

	// 坦克是不是能走
	CCObject* obj;
	CCARRAY_FOREACH(_tanks, obj)
	{
		TankEnemy* tank = (TankEnemy*)obj;
		if (tank->canMove(tank->_dir))
		{
			tank->doMove();
		}
		else
		{
			int dir = (int)(CCRANDOM_0_1() * 4);
			tank->Turn((Common::DIRECTION)dir);
		}
	}
}

void AI::Shoot(float)
{
	if (_pause)
		return;

	CCObject* obj;
	Bullet* bullet;
	CCARRAY_FOREACH(_tanks, obj)
	{
		TankEnemy* tank = (TankEnemy*)obj;
		bullet = tank->Shoot();

		_bullets->addObject(bullet);
	}
}

void AI::update(float)
{
	Bullet::CheckBullets(getMap(), _bullets);
}