
#ifndef __LayerGame_H__
#define __LayerGame_H__

#include "Common.h"
#include "TankFriend.h"
#include "MenuCtrl.h"
#include "AI.h"
class Item;

class LayerGame : public CCLayer
{
public:
//	CREATE_FUNC(LayerGame);

	static LayerGame* create(unsigned int index) \
	{ \
	LayerGame *pRet = new LayerGame(); \
	if (pRet && pRet->init(index)) \
	{ \
	pRet->autorelease(); \
	return pRet; \
	} \
	else \
	{ \
	delete pRet; \
	pRet = NULL; \
	return NULL; \
	} \
	}

	// 布局
	bool init(unsigned int index);

	// 结束时收尾
	void onExit();

	// Control
	void update(float);
	void DirCtrlPress(CCObject*);
	void Shoot(CCObject*);
	void Setup(CCObject*);

	void TimeOut(float);

	enum FAILURE_REASON {TIMEOUT, HOMEDESTROY, TANKDIE};
	static void gameOver(FAILURE_REASON reason);

	void setGodMode(int interval)
	{
		_godMode = true;
		CCDelayTime* delay = CCDelayTime::create(interval);
		CCCallFunc* callFunc = CCCallFunc::create(this, callfunc_selector(LayerGame::unsetGodMode));
		this->runAction(CCSequence::createWithTwoActions(delay, callFunc));
	}
	void unsetGodMode()
	{
		_godMode = false;
	}
	void addItem();
	void eatItem(Item*);
	void eatItemBomb();
	void eatItemSteel();
	void setPause()
	{
		CCDirector::sharedDirector()->pause();
	}
	// model
	CCTMXTiledMap*	_map;
	TankFriend*		_tankFriend;
	static int				_life;
	bool			_godMode;

	CCArray*		_bullets;
	CCArray*		_items;
	AI*				_ai;

	int				_victoryCount;  // 胜利条件
	int				_curCount;      // 当前得分

	int				_index;

	static int				_score;

	CCLabelTTF*			_labelScore;
//	CCLabelTTF*			_labelScoreTotal;
};

#endif
