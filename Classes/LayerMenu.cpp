#include "LayerMenu.h"
#include "LayerGame.h"

bool LayerMenu::init()
{
	CCLayer::init();

	_index = 0;

	// 先创建子菜单
	CCArray* arr = CCArray::create();
	for (int i = 1; i <= 20; ++i)
	{
		CCMenuItemFont* subItem = CCMenuItemFont::create(Common::format(i, "Round "));
		arr->addObject(subItem);
	}

	CCMenuItemToggle* itemToggle = CCMenuItemToggle::createWithTarget(this, menu_selector(LayerMenu::Troggle), arr);
	// CCMenuItemToggle::create();
//	itemToggle->setSubItems(arr);
//	itemToggle->setTarget(this, menu_selector(LayerMenu::Troggle));

	CCMenuItemFont* go = CCMenuItemFont::create("GO", this, menu_selector(LayerMenu::Start));


	CCMenu* menu = CCMenu::create(itemToggle, go, NULL);
	addChild(menu);

	menu->alignItemsVerticallyWithPadding(10);

	return true;
}

void LayerMenu::Start(CCObject*)
{
	// 切换场景，并且要把troggle的选择传递过去
	LayerGame::_life = 3;
	LayerGame::_score = 0;
	CCDirector::sharedDirector()->replaceScene(Common::scene(LayerGame::create(_index)));
}

void LayerMenu::Troggle(CCObject* sender)
{
	CCMenuItemToggle* item = (CCMenuItemToggle*)sender;
	_index = item->getSelectedIndex();
}

