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
	// ��ͣ�������е��ӵ�
	CCObject* obj;
	CCARRAY_FOREACH(_bullets, obj)
	{
		Bullet* bullet = (Bullet*)obj;
		bullet->pauseSchedulerAndActions();
	}

	// ������ʱ������ʱ��ָ�
	scheduleOnce(schedule_selector(AI::resume), 10);
}

void AI::resume(float)
{
	_pause = false;
	// ��ͣ�������е��ӵ�
	CCObject* obj;
	CCARRAY_FOREACH(_bullets, obj)
	{
		Bullet* bullet = (Bullet*)obj;
		bullet->resumeSchedulerAndActions();
	}
}

void AI::createTank(float)
{
	// ����ͣ״̬�£���������̹��
	if (_pause)
		return;

	TankEnemy* tank = TankEnemy::create();

	// AI��Parent���ǵ�ͼ,��̹�˼��뵽��ͼ
	getMap()->addChild(tank);

	// ���ø��Ӵ�С
	tank->setTileSize(tank->getMap()->getTileSize().width);

	// ����λ��
	_createFlag = !_createFlag;
	if (_createFlag)
	{
		tank->setPosition(Common::Tile2PointLB(getMap(), ccp(1, 0)));
	}
	else
	{
		tank->setPosition(Common::Tile2PointLB(getMap(), ccp(25, 0)));
	}

	// ���뵽����
	_tanks->addObject(tank);
}

void AI::moveTank(float)
{
	// ����㷨
	if (_pause)
		return;

	// ̹���ǲ�������
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