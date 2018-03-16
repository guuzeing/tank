
#ifndef __Common_H__
#define __Common_H__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;

#define winSize CCDirector::sharedDirector()->getWinSize()
#define TANK_SPEED 2
#define BULLET_SPEED 4
#define TANK_CREATE_INTERVAL 3
#define TANK_SHOOT_INTERVAL 1
#define TIME_OVER 300
#define TANK_BOMB "TANK_BOMB"

class Common
{
public:
	enum DIRECTION {UP, RIGHT, DOWN, LEFT, INVALID_DIR};
	enum TileType {NONE, BLOCK, STEEL, HOME};
	enum {ZO_LAYER_0 = 100, ZO_BULLET, ZO_TANK, ZO_LAYER_1, ZO_ITEM, ZO_LAYER_SETUP};

	static TileType getTileType(int gid)
	{
		if (gid == 5 || gid == 6 || gid == 11 || gid == 12)
			return STEEL;
		if (gid == 13 || gid == 14 || gid == 19 || gid == 20)
			return BLOCK;
		if (gid == 17 || gid == 18 || gid == 23 || gid == 24)
			return HOME;

		return NONE;
	}
	static CCScene* scene(CCLayer* layer)
	{
		CCScene* s = CCScene::create();
		s->addChild(layer);
		return s;
	}



	static const char* format(int v, const char* prefix = "", const char* suffix = "")
	{
		static char buf[1024];
		sprintf(buf, "%s%d%s", prefix, v, suffix);
		return buf;
	}

	static const CCPoint Tile2PointLB(CCTMXTiledMap* map, CCPoint ptTile)
	{
		ptTile.y = map->getMapSize().height - 1 - ptTile.y;

		return ccp(ptTile.x * map->getTileSize().width,
			ptTile.y * map->getTileSize().height);
	}

	static CCPoint Point2Tile(CCTMXTiledMap* map, CCPoint ptMap)
	{
		int dx = map->getTileSize().width;
		int dy = map->getTileSize().height;

		int x = ptMap.x / dx;
		int y = ptMap.y / dy;
		y = map->getMapSize().height - 1 - y;


		return ccp(x, y);
	}

	static void moveNodeX(CCNode* n, int dx)
	{
		n->setPositionX(n->getPositionX() + dx);
	}
	static void moveNodeY(CCNode* n, int dy)
	{
		n->setPositionY(n->getPositionY() + dy);
	}
	static void moveNode(CCNode* n, CCPoint pt)
	{
		moveNodeX(n, pt.x);
		moveNodeY(n, pt.y);
	}

	
};


#endif