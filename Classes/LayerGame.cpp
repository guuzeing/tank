#include "LayerGame.h"
#include "Bullet.h"
#include "TankEnemy.h"
#include "LayerMenu.h"
#include "Item.h"
#include "LayerSetup.h"
#include "LayerScore.h"
int LayerGame::_life = 3;
int LayerGame::_score = 0;

bool LayerGame::init(unsigned int index)
{
	CCLayer::init();

	_index = index;

	_labelScore = CCLabelTTF::create(Common::format(_score, "score:"), "Arial", 24);
	addChild(_labelScore);
	// label的锚点，同时也是文字的对齐方向
	_labelScore->setAnchorPoint(ccp(0, 1));
	_labelScore->setPosition(ccp(0, winSize.height));

//	_labelScoreTotal = CCLabelTTF::create();

	// 预加载音效
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/bonus.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/brickhit.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/eexplosion.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/gameover.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/ice.wav");

	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/levelstarting.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/life.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/moving.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/nmoving.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/pause.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/shieldhit.wav");

	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/shoot.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/steelhit.wav");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sounds/tbonushit.wav");

	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/levelstarting.wav");

	// 创建地图
	_map = CCTMXTiledMap::create(Common::format(index + 1, "Round", ".tmx"));
	addChild(_map);

	// 设置层的ZORDER，为了让子弹的ZORDER小于坦克的ZORDER，所以要调整层的ZORDER
	_map->layerNamed("layer_0")->setZOrder(Common::ZO_LAYER_0);
	_map->layerNamed("layer_1")->setZOrder(Common::ZO_LAYER_1);

	// 地图居中,向右移动 (窗口宽度-地图宽度)/2
	Common::moveNodeX(_map, (winSize.width - _map->getContentSize().width) / 2);

	// 创建友军坦克
	_tankFriend = TankFriend::create();
	_map->addChild(_tankFriend);
	_tankFriend->setTileSize(_map->getTileSize().width);
	// 友军坦克位置在大本营左边 （10，24）是地图格子的坐标，地图格子的坐标体系是左上角为原点
	_tankFriend->setPosition(Common::Tile2PointLB(_map, ccp(10, 24)));

	// 创建方向键
	MenuCtrl* ctrl = MenuCtrl::create();
	addChild(ctrl);
	
	// 四个menuitem
	CCMenuItemImage* t = CCMenuItemImage::create("paddle/top.png", "paddle/top_press.png");
	CCMenuItemImage* b = CCMenuItemImage::create("paddle/buttom.png", "paddle/buttom_press.png");
	CCMenuItemImage* l = CCMenuItemImage::create("paddle/left.png", "paddle/left_press.png");
	CCMenuItemImage* r = CCMenuItemImage::create("paddle/right.png", "paddle/right_press.png");
	ctrl->addChild(t);
	ctrl->addChild(b);
	ctrl->addChild(l);
	ctrl->addChild(r);
	t->setTarget(this, menu_selector(LayerGame::DirCtrlPress));
	b->setTarget(this, menu_selector(LayerGame::DirCtrlPress));
	l->setTarget(this, menu_selector(LayerGame::DirCtrlPress));
	r->setTarget(this, menu_selector(LayerGame::DirCtrlPress));
	t->setTag(Common::UP);
	b->setTag(Common::DOWN);
	l->setTag(Common::LEFT);
	r->setTag(Common::RIGHT);
	
	// 设置位置
	CCPoint ptCenter = ccp(-180, -100);
	int dist = 50;
	t->setPosition(ccp(ptCenter.x, ptCenter.y + dist));
	b->setPosition(ccp(ptCenter.x, ptCenter.y - dist));
	l->setPosition(ccp(ptCenter.x - dist, ptCenter.y));
	r->setPosition(ccp(ptCenter.x + dist, ptCenter.y));

	//创建发射子弹的按钮
	CCMenu* menu = CCMenu::create();
	addChild(menu);
	CCMenuItemImage* itemShoot = CCMenuItemImage::create("paddle/fire.png", "paddle/fire_press.png");
	menu->addChild(itemShoot);
	itemShoot->setTarget(this, menu_selector(LayerGame::Shoot));
	// 移动发射子弹按钮的位置
	itemShoot->setPosition(ccp(-ptCenter.x, ptCenter.y));

	// 设置按钮
	CCMenuItemFont* setup = CCMenuItemFont::create("Setup", this, menu_selector(LayerGame::Setup));
	menu->addChild(setup);
	// 把按钮放置在右上角
	setup->setPosition(ccp(winSize.width / 2 - setup->boundingBox().size.width / 2,
		winSize.height / 2 - setup->boundingBox().size.height / 2));

	// 保存友军子弹的数组
	_bullets = CCArray::create();
	_bullets->retain();

	// 碰撞检测
	scheduleUpdate();

	// 创建AI对象
	_ai = AI::create();
	_map->addChild(_ai);

	// 胜利条件
	this->_victoryCount = 50;
	this->_curCount = 0;

	// 失败超时的定时器
	scheduleOnce(schedule_selector(LayerGame::TimeOut), TIME_OVER);

	// 预加载坦克爆炸的动画

	// 加载动画帧（CCArray）
	CCArray* arrFrames = CCArray::create();

	CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	cache->addSpriteFramesWithFile("blast.plist");
	for (int i = 1; i <= 8; ++i)
	{
		const char* spriteFrameName = Common::format(i, "blast", ".gif");
		CCSpriteFrame* frame = cache->spriteFrameByName(spriteFrameName);
		arrFrames->addObject(frame);
	}

	// 通过动画帧创建Animation
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(arrFrames, 0.1f);
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, TANK_BOMB);

	// 预加载坦克爆炸的动画 结束

	//_life = 3;
	_godMode = false;
	

	_items = CCArray::create();
	_items->retain();
	return true;
}

void LayerGame::onExit()
{
	CCLayer::onExit();
	_bullets->release();
	_items->release();
}

void LayerGame::TimeOut(float)
{
	// 超时失败
	CCLog("Failure timeout");
	gameOver(TIMEOUT);
}

void LayerGame::gameOver(FAILURE_REASON reason)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/gameover.wav");

	// 记录分数
	int totalScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("TotalScore", 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("TotalScore", totalScore + _score);

	int score = CCUserDefault::sharedUserDefault()->getIntegerForKey("Score", 0);
	if (_score > score)
	{
		CCUserDefault::sharedUserDefault()->setIntegerForKey("Score", _score);
	}

	_score = 0;
	_life = 3;

	// 进入排行榜场景
	CCDirector::sharedDirector()->replaceScene(Common::scene(LayerScore::create()));
}

void LayerGame::update(float)
{
#ifdef WIN32
	if (GetKeyState('A') < 0) _tankFriend->Move(Common::LEFT);
	if (GetKeyState('S') < 0) _tankFriend->Move(Common::DOWN);
	if (GetKeyState('D') < 0) _tankFriend->Move(Common::RIGHT);
	if (GetKeyState('W') < 0) _tankFriend->Move(Common::UP);
	if (GetKeyState('K') < 0) _bullets->addObject(_tankFriend->Shoot());
#endif
	Bullet::CheckBullets(_map, _bullets);
	
	int count;

	// 子弹和子弹碰撞
	count = _bullets->count();
	for (int i = count - 1; i >= 0; --i)
	{
		Bullet* bullet = (Bullet*)_bullets->objectAtIndex(i);

		int countEnemy = _ai->_bullets->count();
		for (int j = countEnemy - 1; j >= 0; --j)
		{
			Bullet* bulletEnemy = (Bullet*)_ai->_bullets->objectAtIndex(j);
			if (bullet->boundingBox().intersectsRect(bulletEnemy->boundingBox()))
			{
				bullet->removeFromParent();
				_bullets->removeObjectAtIndex(i);

				bulletEnemy->removeFromParent();
				_ai->_bullets->removeObjectAtIndex(j);
				break;
			}
		}
	}

	// 友军坦克的碰撞检测
	count = _ai->_bullets->count();
	
	for (int i = count - 1; i >= 0; --i)
	{
		Bullet* bullet = (Bullet*)_ai->_bullets->objectAtIndex(i);
		if (bullet->boundingBox().intersectsRect(_tankFriend->boundingBox()))
		{
			// 删除子弹
			bullet->removeFromParent();
			_ai->_bullets->removeObjectAtIndex(i);

			if (!_godMode)
			{
				if (_life == 1)
					gameOver(TANKDIE);
				else
				{
					--_life;
					_godMode = true;

					CCBlink* blink = CCBlink::create(5, 10);
					CCCallFunc* callFunc = CCCallFunc::create(this, callfunc_selector(LayerGame::unsetGodMode));
					_tankFriend->runAction( CCSequence::create(blink, callFunc, NULL));
				}
			}
			return;
		}
	}

	count = _items->count();
	for (int i = count - 1; i >= 0;--i)
	{
		Item* item = (Item*)_items->objectAtIndex(i);
		if (item->boundingBox().intersectsRect(_tankFriend->boundingBox()))
		{
			eatItem(item);
			break;
		}
	}
	

	// 检测友军子弹和敌军坦克的碰撞
	count = _bullets->count();
	for (int i = count - 1; i >= 0; --i)
	{
		// 获取友军的子弹
		Bullet* bullet = (Bullet*)_bullets->objectAtIndex(i);
		int enemyCount = _ai->_tanks->count();
		for (int j = enemyCount - 1; j >= 0; --j)
		{
			// 敌军坦克
			TankEnemy* enemy = (TankEnemy*)_ai->_tanks->objectAtIndex(j);
			if (enemy->boundingBox().intersectsRect(bullet->boundingBox()))
			{
				// 敌军坦克删除
				++_score;
				_labelScore->setString(Common::format(_score, "Score:"));

				// enemy->removeFromParent();
				_ai->_tanks->removeObjectAtIndex(j);
				// 坦克爆炸的动画
				CCAnimate* animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName(TANK_BOMB));
				CCCallFunc* callFunc = CCCallFunc::create(enemy, callfunc_selector(TankEnemy::removeFromParent));
				CCCallFunc* callAddItem = CCCallFunc::create(this, callfunc_selector(LayerGame::addItem));
				enemy->runAction(CCSequence::create(animate, callFunc, callAddItem, NULL));

				
				// 通过Animation创建Animate
				// 执行Animate，动画完毕之后，调用removeFromParent


				// 子弹删除
				bullet->removeFromParent();
				_bullets->removeObjectAtIndex(i);

				// 计数器加1
				++_curCount;
				// 判断是否胜利
				if (_curCount == _victoryCount)
				{
					// 如果已经是最后一个场景，那么切换到菜单场景
					if (_index == 19) 
						CCDirector::sharedDirector()->replaceScene(Common::scene(LayerMenu::create()));
					else // 进入下一个场景
						CCDirector::sharedDirector()->replaceScene(Common::scene(LayerGame::create(_index + 1)));
				}

				break;
			}
		}
	}
}

void LayerGame::eatItemBomb()
{
	int enemyCount = _ai->_tanks->count();
	for (int j = enemyCount - 1; j >= 0; --j)
	{
		++_score;
		_labelScore->setString(Common::format(_score, "Score:"));

		// 敌军坦克
		TankEnemy* enemy = (TankEnemy*)_ai->_tanks->objectAtIndex(j);
		
		// 敌军坦克删除
		// enemy->removeFromParent();
		_ai->_tanks->removeObjectAtIndex(j);
		// 坦克爆炸的动画
		CCAnimate* animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName(TANK_BOMB));
		CCCallFunc* callFunc = CCCallFunc::create(enemy, callfunc_selector(TankEnemy::removeFromParent));
	//	CCCallFunc* callAddItem = CCCallFunc::create(this, callfunc_selector(LayerGame::addItem));
		enemy->runAction(CCSequence::create(animate, callFunc, NULL));
	}
}
void LayerGame::eatItemSteel()
{
	// 设置加墙的位置
	static CCPoint pts[] = { ccp(11, 25), ccp(11, 24), ccp(11, 23),
	                  ccp(12, 23), ccp(13, 23),
	                  ccp(14, 23), ccp(14, 24), ccp(14, 25)};
	
	// 设置墙
	for (int i = 0; i < sizeof(pts) / sizeof(*pts); ++i)
		_map->layerNamed("layer_0")->setTileGID(13, pts[0]);
}
void LayerGame::eatItem(Item* item)
{
	switch (item->_type)
	{
	case Item::IT_ACTIVE:
		setGodMode(10);
		break;
	case Item::IT_BOMB:
		eatItemBomb();
		break;
	case Item::IT_MINTANK:
		++_life;
		break;
	case Item::IT_STAR:
		++_tankFriend->_damage;
		break;
	case Item::IT_STEEL:
		eatItemSteel();
		break;
	case Item::IT_TIMER:
		_ai->pause();
		break;
	}

	item->removeFromParent();
	_items->removeObject(item);
}

void LayerGame::addItem()
{
	// 10%的概率
	int v = CCRANDOM_0_1() * 10;
	if (v != 5)
		return;

	// 创建一个item精灵
	// 放在一个随机位置

	Item* item = Item::create();
	_map->addChild(item);
	_items->addObject(item);

	item->setPosition();
}

void LayerGame::Shoot(CCObject*)
{
	Bullet* bullet = _tankFriend->Shoot();
	_bullets->addObject(bullet);
}

void LayerGame::DirCtrlPress(CCObject* sender)
{
	CCMenuItem* item = (CCMenuItem*)sender;
	Common::DIRECTION dir = (Common::DIRECTION)item->getTag();
	// 根据按钮的方向去移动坦克
	this->_tankFriend->Move(dir);
}

void LayerGame::Setup(CCObject*)
{
	LayerSetup* setup = LayerSetup::create();
	addChild(setup);
	
	Common::moveNode(setup, ccp(winSize.width, 0));
	CCCallFunc* pauseAction = CCCallFunc::create(this, callfunc_selector(LayerGame::setPause));
	CCMoveTo* move = CCMoveTo::create(5, ccp(0, 0));
	CCEaseElasticInOut* elastic = CCEaseElasticInOut::create(move);
	setup->runAction(CCSequence::createWithTwoActions(elastic, pauseAction));
}