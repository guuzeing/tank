#include "TankFriend.h"


bool TankFriend::init()
{
	CCSprite::initWithFile("tank/player2/player2U.png");

	_tileSize = 0;
	_dir = Common::UP;

	this->setZOrder(Common::ZO_TANK);
	_damage = 1;

	return true;
}

void TankFriend::setTileSize(int tileSize)
{
	if (_tileSize == 0)
	{
		_tileSize = tileSize;
		setScale((tileSize * 2 - 4) / getContentSize().width);
	}
}

void TankFriend::Turn(Common::DIRECTION dir)
{
	this->setRotation(dir * 90);
	_dir = dir;
}

CCTMXTiledMap* TankFriend::getMap()
{
	return (CCTMXTiledMap*)this->getParent();
}

bool TankFriend::canMove(Common::DIRECTION dir)
{
	CCPoint pt[3];
	CCRect rc = boundingBox();
	CCTMXTiledMap* map = getMap();
	
	if (dir == Common::UP)
	{
		pt[0] = ccp(rc.getMinX(), rc.getMaxY() + TANK_SPEED);
		pt[1] = ccp(rc.getMidX(), rc.getMaxY() + TANK_SPEED);
		pt[2] = ccp(rc.getMaxX(), rc.getMaxY() + TANK_SPEED);
	}
	else if (dir == Common::DOWN)
	{
		pt[0] = ccp(rc.getMinX(), rc.getMinY() - TANK_SPEED);
		pt[1] = ccp(rc.getMidX(), rc.getMinY() - TANK_SPEED);
		pt[2] = ccp(rc.getMaxX(), rc.getMinY() - TANK_SPEED);
	}
	else if (dir == Common::LEFT)
	{
		pt[0] = ccp(rc.getMinX() - TANK_SPEED, rc.getMinY());
		pt[1] = ccp(rc.getMinX() - TANK_SPEED, rc.getMidY());
		pt[2] = ccp(rc.getMinX() - TANK_SPEED, rc.getMaxY());
	}
	else if (dir == Common::RIGHT)
	{
		pt[0] = ccp(rc.getMaxX() + TANK_SPEED, rc.getMinY());
		pt[1] = ccp(rc.getMaxX() + TANK_SPEED, rc.getMidY());
		pt[2] = ccp(rc.getMaxX() + TANK_SPEED, rc.getMaxY());
	}

	// pt[3]ת���ɵ�ͼ����
	CCSize sizeMap = map->getContentSize();
	CCRect rcMap = CCRectMake(0, 0, sizeMap.width-1, sizeMap.height-1);

	for (int i = 0; i < 3; i++)
	{
		// �ж�̹���ǲ����ܵ���ͼ����,����ڵ�ͼ����,Ҳ�����ƶ�
		if (!rcMap.containsPoint(pt[i]))
			return false;

		// ��ײ��ת���ɵ�ͼ����,���ݵ�ͼ�����õ�ͼ��GID,�ٸ���GID��ȡ��������
		CCPoint ptTile = Common::Point2Tile(map, pt[i]);
		int gid = map->layerNamed("layer_0")->tileGIDAt(ptTile);
		Common::TileType tt = Common::getTileType(gid);
		// ���ݸ����������ж��Ƿ�����ƶ�
		if (tt == Common::BLOCK || tt == Common::STEEL)
			return false;
	}
	
	return true;
}

void TankFriend::doMove()
{
	// ��ʵ�ʵ��ƶ�
	switch (_dir)
	{
	case Common::LEFT:
		Common::moveNodeX(this, -TANK_SPEED);
		break;
	case Common::RIGHT:
		Common::moveNodeX(this, TANK_SPEED);
		break;
	case Common::UP:
		Common::moveNodeY(this, TANK_SPEED);
		break;
	case Common::DOWN:
		Common::moveNodeY(this, -TANK_SPEED);
		break;
	}
}

void TankFriend::Move(Common::DIRECTION dir)
{
	// ת��
	if (_dir != dir)
	{
		Turn(dir);
	}

	// �ж�̹���ǲ��ǿ������÷����ƶ�
	if (!canMove(dir))
		return;

	doMove();


	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/moving.wav");
}

Bullet* TankFriend::Shoot()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/shoot.wav");

	Bullet* bullet = Bullet::create();
	bullet->setPicture("tank/bullet.png");

	getMap()->addChild(bullet);
	bullet->setPosition(getPosition());

	// ���ӵ��˶�
	CCPoint ptDest;
	switch (_dir)
	{
	case Common::LEFT:
		ptDest = ccp(-winSize.width, 0);
		break;
	case Common::RIGHT:
		ptDest = ccp(winSize.width, 0);
		break;
	case Common::UP:
		ptDest = ccp(0, winSize.width);
		break;
	case Common::DOWN:
		ptDest = ccp(0, -winSize.width);
		break;
	}
	CCMoveBy* moveBy = CCMoveBy::create(winSize.width / (BULLET_SPEED * 60), ptDest);
	bullet->runAction(moveBy);

	bullet->_damage = _damage;

	return bullet;
}