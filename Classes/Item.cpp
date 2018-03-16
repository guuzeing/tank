#include "Item.h"


bool Item::init()
{
	// 随机生成类型
	 ItemType it = (ItemType)(int)(CCRANDOM_0_1() * IT_COUNT);
	//ItemType it = IT_TIMER;
	static const char* files[] = {
		"item/active.png",
		"item/bomb.png",
		"item/mintank.png",
		"item/star.png",
		"item/steel.png",
		"item/timer.png",
	};
	CCSprite::initWithFile(files[it]);
	_type = it;
	setZOrder(Common::ZO_ITEM);
	return true;
}

void Item::setPosition()
{
	CCTMXTiledMap* map = (CCTMXTiledMap*)getParent();
	CCSize sz = boundingBox().size;
	int x = CCRANDOM_0_1() * (map->getContentSize().width - sz.width) + sz.width/2;
	int y = CCRANDOM_0_1() * (map->getContentSize().height - sz.height) + sz.height/2;

	CCSprite::setPosition(ccp(x, y));
}