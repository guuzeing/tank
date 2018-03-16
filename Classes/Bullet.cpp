#include "Bullet.h"
#include "LayerGame.h"

bool Bullet::init()
{
	CCSprite::init();
	this->setZOrder(Common::ZO_BULLET);
	return true;
}

void Bullet::setPicture(const char* filename)
{
	CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage(filename);

	setTexture(texture);
	setTextureRect(CCRectMake(0, 0, texture->getContentSize().width,
		texture->getContentSize().height));
}

void Bullet::CheckBullets(CCTMXTiledMap* map, CCArray* bullets)
{
	int count = bullets->count();
	CCSize szMap = map->getContentSize();
	CCRect rcMap = CCRectMake(0, 0, szMap.width - 1, szMap.height - 1);

	CCTMXLayer* layer = map->layerNamed("layer_0");

	for (int i = count - 1; i >= 0; --i)
	{
		Bullet* bullet = (Bullet*)bullets->objectAtIndex(i);
		CCPoint ptBullet = bullet->getPosition();
		// 判断子弹是不是飞出去了
		if (!rcMap.containsPoint(ptBullet))
		{
			bullet->removeFromParent();
			bullets->removeObjectAtIndex(i);
			continue;
		}

		// 判断子弹是不是碰到砖头／铁块／大本营了
		CCPoint ptTile = Common::Point2Tile(map, ptBullet);
		int gid = layer->tileGIDAt(ptTile);
		Common::TileType tt = Common::getTileType(gid);
		if (tt == Common::BLOCK)
		{
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/brickhit.wav");

			bullet->removeFromParent();
			bullets->removeObjectAtIndex(i);
			// 砖头被打掉了
			layer->setTileGID(0, ptTile);
			continue;
		}
		else if (tt == Common::STEEL)
		{
			if (bullet->_damage >= 3)
			{
				// 钢铁砖头被打掉了
				layer->setTileGID(0, ptTile);
			}
			CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("sounds/steelhit.wav");
			bullet->removeFromParent();
			bullets->removeObjectAtIndex(i);
			continue;
		}
		else if (tt == Common::HOME)
		{
			
			bullet->removeFromParent();
			bullets->removeObjectAtIndex(i);
			CCLog("Game Over");
			LayerGame::gameOver(LayerGame::HOMEDESTROY);
			continue;
		}

		// 判断子弹是不是碰到敌方子弹了
		// 判断子弹是不是碰到敌方坦克了
	}
}