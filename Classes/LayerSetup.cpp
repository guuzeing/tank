#include "LayerSetup.h"

bool LayerSetup::init()
{
	CCLayer::init();

	CCLayerColor* layerTrans = CCLayerColor::create(ccc4(192, 192, 192, 128));
	addChild(layerTrans);

	CCLayerColor* layerBG = CCLayerColor::create(ccc4(192, 192, 192, 255), winSize.width / 2, winSize.height / 2);
	addChild(layerBG);
	layerBG->setPosition(ccp(winSize.width / 4, winSize.height / 4));

	// 创建两个slider
	MyCCControlSlider* volumeBG = MyCCControlSlider::create("sliderTrack.png", "sliderProgress.png", "sliderThumb.png");
	MyCCControlSlider* volumeEffect = MyCCControlSlider::create("sliderTrack.png", "sliderProgress.png", "sliderThumb.png");
	addChild(volumeBG);
	addChild(volumeEffect);
	_volumeBG = volumeBG;
	_volumeEffect = volumeEffect;

	volumeBG->setMinimumValue(0.0f);
	volumeBG->setMaximumValue(1.0f);

	volumeEffect->setMinimumValue(0.0f);
	volumeEffect->setMaximumValue(1.0f);

	volumeBG->setPosition(ccp(winSize.width / 2, winSize.height / 2));
	volumeEffect->setPosition(ccp(winSize.width / 2, winSize.height / 2));

	Common::moveNodeY(volumeBG, 40);
//	Common::moveNodeY(volumeEffect, -30);

	volumeBG->addTargetWithActionForControlEvents(this, cccontrol_selector(LayerSetup::ChangeBGVolume), CCControlEventValueChanged);
	volumeEffect->addTargetWithActionForControlEvents(this, cccontrol_selector(LayerSetup::ChangeEffectVolume), CCControlEventValueChanged);

	setZOrder(Common::ZO_LAYER_SETUP);

	// 读配置文件
	float fVolumeBG = CCUserDefault::sharedUserDefault()->getFloatForKey("BGVolume", 1.0f);
	float fVolumeEffect = CCUserDefault::sharedUserDefault()->getFloatForKey("EffectVolume", 1.0f);

	volumeBG->setValue(fVolumeBG);
	volumeEffect->setValue(fVolumeEffect);

	// 创建一个确定按钮
	CCMenuItemFont* item = CCMenuItemFont::create("Close", this, menu_selector(LayerSetup::Close));
	CCMenu* menu = CCMenu::createWithItem(item);
	addChild(menu);
	Common::moveNode(item, ccp(80, -60));
	_menu = menu;

	// 暂停
	// CCDirector::sharedDirector()->pause();

	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);
	setTouchPriority(-130);

	return true;
}

// LayerSetup截取所有的触摸消息，自己来做派发
// 而且只派发给自己的菜单
bool LayerSetup::ccTouchBegan(CCTouch* t, CCEvent* e)
{
	_bMenuItemClicked = _menu->ccTouchBegan(t, e);
	_bVolumeBGClicked = _volumeBG->ccTouchBegan(t, e);
	_bVolumeEffectClicked = _volumeEffect->ccTouchBegan(t, e);
	return true;
}

void LayerSetup::ccTouchCancelled(CCTouch* t, CCEvent* e)
{
	if (_bMenuItemClicked) _menu->ccTouchCancelled(t, e);
	if (_bVolumeBGClicked) _volumeBG->ccTouchCancelled(t, e);
	if (_bVolumeEffectClicked) _volumeEffect->ccTouchCancelled(t, e);
}
void LayerSetup::ccTouchEnded(CCTouch*t, CCEvent*e)
{
	if (_bMenuItemClicked) _menu->ccTouchEnded(t, e);
	if (_bVolumeBGClicked) _volumeBG->ccTouchEnded(t, e);
	if (_bVolumeEffectClicked) _volumeEffect->ccTouchEnded(t, e);
}
void LayerSetup::ccTouchMoved(CCTouch*t, CCEvent*e)
{
	if (_bMenuItemClicked) _menu->ccTouchMoved(t, e);
	if (_bVolumeBGClicked) _volumeBG->ccTouchMoved(t, e);
	if (_bVolumeEffectClicked) _volumeEffect->ccTouchMoved(t, e);
}

void LayerSetup::Close(CCObject*)
{
	this->removeFromParent();
	CCDirector::sharedDirector()->resume();
}

void LayerSetup::ChangeBGVolume(CCObject* obj, CCControlEvent)
{
	CCControlSlider* slider = (CCControlSlider*)obj;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(slider->getValue());

	CCUserDefault::sharedUserDefault()->setFloatForKey("BGVolume", slider->getValue());
}

void LayerSetup::ChangeEffectVolume(CCObject* obj, CCControlEvent)
{
	CCControlSlider* slider = (CCControlSlider*)obj;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(slider->getValue());

	CCUserDefault::sharedUserDefault()->setFloatForKey("EffectVolume", slider->getValue());
}