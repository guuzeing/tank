
#ifndef __LayerMenu_H__
#define __LayerMenu_H__

#include "Common.h"

class LayerMenu : public CCLayer
{
public:
	CREATE_FUNC(LayerMenu);
	bool init();

	void Start(CCObject*);
	void Troggle(CCObject*);

	unsigned int _index;
};

#endif
