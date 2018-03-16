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
	// label��ê�㣬ͬʱҲ�����ֵĶ��뷽��
	_labelScore->setAnchorPoint(ccp(0, 1));
	_labelScore->setPosition(ccp(0, winSize.height));

//	_labelScoreTotal = CCLabelTTF::create();

	// Ԥ������Ч
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

	// ������ͼ
	_map = CCTMXTiledMap::create(Common::format(index + 1, "Round", ".tmx"));
	addChild(_map);

	// ���ò��ZORDER��Ϊ�����ӵ���ZORDERС��̹�˵�ZORDER������Ҫ�������ZORDER
	_map->layerNamed("layer_0")->setZOrder(Common::ZO_LAYER_0);
	_map->layerNamed("layer_1")->setZOrder(Common::ZO_LAYER_1);

	// ��ͼ����,�����ƶ� (���ڿ��-��ͼ���)/2
	Common::moveNodeX(_map, (winSize.width - _map->getContentSize().width) / 2);

	// �����Ѿ�̹��
	_tankFriend = TankFriend::create();
	_map->addChild(_tankFriend);
	_tankFriend->setTileSize(_map->getTileSize().width);
	// �Ѿ�̹��λ���ڴ�Ӫ��� ��10��24���ǵ�ͼ���ӵ����꣬��ͼ���ӵ�������ϵ�����Ͻ�Ϊԭ��
	_tankFriend->setPosition(Common::Tile2PointLB(_map, ccp(10, 24)));

	// ���������
	MenuCtrl* ctrl = MenuCtrl::create();
	addChild(ctrl);
	
	// �ĸ�menuitem
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
	
	// ����λ��
	CCPoint ptCenter = ccp(-180, -100);
	int dist = 50;
	t->setPosition(ccp(ptCenter.x, ptCenter.y + dist));
	b->setPosition(ccp(ptCenter.x, ptCenter.y - dist));
	l->setPosition(ccp(ptCenter.x - dist, ptCenter.y));
	r->setPosition(ccp(ptCenter.x + dist, ptCenter.y));

	//���������ӵ��İ�ť
	CCMenu* menu = CCMenu::create();
	addChild(menu);
	CCMenuItemImage* itemShoot = CCMenuItemImage::create("paddle/fire.png", "paddle/fire_press.png");
	menu->addChild(itemShoot);
	itemShoot->setTarget(this, menu_selector(LayerGame::Shoot));
	// �ƶ������ӵ���ť��λ��
	itemShoot->setPosition(ccp(-ptCenter.x, ptCenter.y));

	// ���ð�ť
	CCMenuItemFont* setup = CCMenuItemFont::create("Setup", this, menu_selector(LayerGame::Setup));
	menu->addChild(setup);
	// �Ѱ�ť���������Ͻ�
	setup->setPosition(ccp(winSize.width / 2 - setup->boundingBox().size.width / 2,
		winSize.height / 2 - setup->boundingBox().size.height / 2));

	// �����Ѿ��ӵ�������
	_bullets = CCArray::create();
	_bullets->retain();

	// ��ײ���
	scheduleUpdate();

	// ����AI����
	_ai = AI::create();
	_map->addChild(_ai);

	// ʤ������
	this->_victoryCount = 50;
	this->_curCount = 0;

	// ʧ�ܳ�ʱ�Ķ�ʱ��
	scheduleOnce(schedule_selector(LayerGame::TimeOut), TIME_OVER);

	// Ԥ����̹�˱�ը�Ķ���

	// ���ض���֡��CCArray��
	CCArray* arrFrames = CCArray::create();

	CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();
	cache->addSpriteFramesWithFile("blast.plist");
	for (int i = 1; i <= 8; ++i)
	{
		const char* spriteFrameName = Common::format(i, "blast", ".gif");
		CCSpriteFrame* frame = cache->spriteFrameByName(spriteFrameName);
		arrFrames->addObject(frame);
	}

	// ͨ������֡����Animation
	CCAnimation* animation = CCAnimation::createWithSpriteFrames(arrFrames, 0.1f);
	CCAnimationCache::sharedAnimationCache()->addAnimation(animation, TANK_BOMB);

	// Ԥ����̹�˱�ը�Ķ��� ����

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
	// ��ʱʧ��
	CCLog("Failure timeout");
	gameOver(TIMEOUT);
}

void LayerGame::gameOver(FAILURE_REASON reason)
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/gameover.wav");

	// ��¼����
	int totalScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("TotalScore", 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("TotalScore", totalScore + _score);

	int score = CCUserDefault::sharedUserDefault()->getIntegerForKey("Score", 0);
	if (_score > score)
	{
		CCUserDefault::sharedUserDefault()->setIntegerForKey("Score", _score);
	}

	_score = 0;
	_life = 3;

	// �������а񳡾�
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

	// �ӵ����ӵ���ײ
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

	// �Ѿ�̹�˵���ײ���
	count = _ai->_bullets->count();
	
	for (int i = count - 1; i >= 0; --i)
	{
		Bullet* bullet = (Bullet*)_ai->_bullets->objectAtIndex(i);
		if (bullet->boundingBox().intersectsRect(_tankFriend->boundingBox()))
		{
			// ɾ���ӵ�
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
	

	// ����Ѿ��ӵ��͵о�̹�˵���ײ
	count = _bullets->count();
	for (int i = count - 1; i >= 0; --i)
	{
		// ��ȡ�Ѿ����ӵ�
		Bullet* bullet = (Bullet*)_bullets->objectAtIndex(i);
		int enemyCount = _ai->_tanks->count();
		for (int j = enemyCount - 1; j >= 0; --j)
		{
			// �о�̹��
			TankEnemy* enemy = (TankEnemy*)_ai->_tanks->objectAtIndex(j);
			if (enemy->boundingBox().intersectsRect(bullet->boundingBox()))
			{
				// �о�̹��ɾ��
				++_score;
				_labelScore->setString(Common::format(_score, "Score:"));

				// enemy->removeFromParent();
				_ai->_tanks->removeObjectAtIndex(j);
				// ̹�˱�ը�Ķ���
				CCAnimate* animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName(TANK_BOMB));
				CCCallFunc* callFunc = CCCallFunc::create(enemy, callfunc_selector(TankEnemy::removeFromParent));
				CCCallFunc* callAddItem = CCCallFunc::create(this, callfunc_selector(LayerGame::addItem));
				enemy->runAction(CCSequence::create(animate, callFunc, callAddItem, NULL));

				
				// ͨ��Animation����Animate
				// ִ��Animate���������֮�󣬵���removeFromParent


				// �ӵ�ɾ��
				bullet->removeFromParent();
				_bullets->removeObjectAtIndex(i);

				// ��������1
				++_curCount;
				// �ж��Ƿ�ʤ��
				if (_curCount == _victoryCount)
				{
					// ����Ѿ������һ����������ô�л����˵�����
					if (_index == 19) 
						CCDirector::sharedDirector()->replaceScene(Common::scene(LayerMenu::create()));
					else // ������һ������
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

		// �о�̹��
		TankEnemy* enemy = (TankEnemy*)_ai->_tanks->objectAtIndex(j);
		
		// �о�̹��ɾ��
		// enemy->removeFromParent();
		_ai->_tanks->removeObjectAtIndex(j);
		// ̹�˱�ը�Ķ���
		CCAnimate* animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName(TANK_BOMB));
		CCCallFunc* callFunc = CCCallFunc::create(enemy, callfunc_selector(TankEnemy::removeFromParent));
	//	CCCallFunc* callAddItem = CCCallFunc::create(this, callfunc_selector(LayerGame::addItem));
		enemy->runAction(CCSequence::create(animate, callFunc, NULL));
	}
}
void LayerGame::eatItemSteel()
{
	// ���ü�ǽ��λ��
	static CCPoint pts[] = { ccp(11, 25), ccp(11, 24), ccp(11, 23),
	                  ccp(12, 23), ccp(13, 23),
	                  ccp(14, 23), ccp(14, 24), ccp(14, 25)};
	
	// ����ǽ
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
	// 10%�ĸ���
	int v = CCRANDOM_0_1() * 10;
	if (v != 5)
		return;

	// ����һ��item����
	// ����һ�����λ��

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
	// ���ݰ�ť�ķ���ȥ�ƶ�̹��
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