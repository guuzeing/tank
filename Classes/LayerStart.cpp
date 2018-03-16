#include "LayerStart.h"
#include "LayerMenu.h"

bool LayerStart::init()
{
	CCLayer::init();

	// �ȼӱ���ͼƬ
	CCSprite* bg = CCSprite::create("ScenceStart.png");
	addChild(bg);
	bg->setPosition(ccp(winSize.width/2, winSize.height/2));
	
	// �ټ�һ����ť
	CCMenuItemImage* item = CCMenuItemImage::create("playgameNormal.png", "playgameClose.png", 
		this, menu_selector(LayerStart::Start));
	CCMenu* menu = CCMenu::createWithItem(item);
	addChild(menu);

	item->setPositionY(item->getPositionY() - 50);

	return true;
}

void LayerStart::Start(CCObject*)
{
	// �л�
	CCDirector::sharedDirector()->replaceScene(Common::scene(LayerMenu::create()));
}
