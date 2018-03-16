#ifndef __TankFriend_H__
#define __TankFriend_H__

#include "Common.h"
#include "Bullet.h"

class TankFriend : public CCSprite
{
public:
	CREATE_FUNC(TankFriend);
	bool init();

	CCTMXTiledMap* getMap();

	void setTileSize(int tileSize);
	int _tileSize;

	void Move(Common::DIRECTION dir);
	void doMove();
	void Turn(Common::DIRECTION dir);
	bool canMove(Common::DIRECTION dir);
	Bullet* Shoot();

	Common::DIRECTION _dir;

	int _damage;
};

#endif

