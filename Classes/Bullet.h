
#ifndef __Bullet_H__
#define __Bullet_H__

#include "Common.h"
 // #include "LayerGame"

class Bullet : public CCSprite
{
public:
	CREATE_FUNC(Bullet);
	bool init();

	void setPicture(const char* filename);

	static void CheckBullets(CCTMXTiledMap* map, CCArray* bullets);

	int _damage;
};

#endif

