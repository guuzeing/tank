#ifndef __Item_H__
#define __Item_H__
#include "Common.h"

class Item : public CCSprite
{
public:
	enum ItemType {IT_ACTIVE, IT_BOMB, IT_MINTANK, IT_STAR, IT_STEEL, IT_TIMER, IT_COUNT};
	CREATE_FUNC(Item);
	bool init();
	void setPosition();
	ItemType _type;
};

#endif

