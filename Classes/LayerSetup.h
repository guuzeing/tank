#ifndef __LayerSetup_H__
#define __LayerSetup_H__

#include "Common.h"
class MyCCControlSlider : public CCControlSlider
{
public:
	static MyCCControlSlider* create(const char* bgFile, const char* progressFile, const char* thumbFile)
	{
		// Prepare background for slider
		CCSprite *backgroundSprite = CCSprite::create(bgFile);

		// Prepare progress for slider
		CCSprite *progressSprite = CCSprite::create(progressFile);

		// Prepare thumb (menuItem) for slider
		CCSprite *thumbSprite = CCSprite::create(thumbFile);

		MyCCControlSlider *pRet = new MyCCControlSlider();
		pRet->initWithSprites(backgroundSprite, progressSprite, thumbSprite);
		pRet->autorelease();
		return pRet;
	}
	bool ccTouchBegan(CCTouch*t, CCEvent*e)
	{
		return CCControlSlider::ccTouchBegan(t, e);
	}
	void ccTouchEnded(CCTouch*t, CCEvent*e)
	{
		CCControlSlider::ccTouchEnded(t, e);
	}
	void ccTouchMoved(CCTouch*t, CCEvent*e)
	{
		CCControlSlider::ccTouchMoved(t, e);
	}
	void ccTouchCancelled(CCTouch*t, CCEvent*e)
	{
		CCControlSlider::ccTouchCancelled(t, e);
	}
};

class LayerSetup : public CCLayer
{
public:
	CREATE_FUNC(LayerSetup);
	bool init();

	void ChangeBGVolume(CCObject*, CCControlEvent);
	void ChangeEffectVolume(CCObject*, CCControlEvent);

	void Close(CCObject*);

	bool ccTouchBegan(CCTouch*, CCEvent*) ;
	void ccTouchEnded(CCTouch*, CCEvent*) ;
	void ccTouchMoved(CCTouch*, CCEvent*) ;
	void ccTouchCancelled(CCTouch*, CCEvent*);

	CCMenu* _menu;
	bool _bMenuItemClicked;

	MyCCControlSlider* _volumeBG;
	bool _bVolumeBGClicked;

	MyCCControlSlider* _volumeEffect;
	bool _bVolumeEffectClicked;
};

#endif

