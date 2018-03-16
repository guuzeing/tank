#include "TankEnemy.h"

bool TankEnemy::init()
{
	CCSprite::initWithFile("tank/armor/armor2U.png");

	_tileSize = 0;
	_dir = Common::UP;
	this->setZOrder(Common::ZO_TANK);
	
	Turn(Common::DOWN);
	return true;
}
