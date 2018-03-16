
#ifndef __TankEnemy_H__
#define __TankEnemy_H__

#include "TankFriend.h"

class TankEnemy : public TankFriend
{
public:
	CREATE_FUNC(TankEnemy);
	bool init();
};

#endif
