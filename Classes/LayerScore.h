#ifndef __LayerScore_H__
#define __LayerScore_H__

#include "Common.h"
class LayerScore : public CCLayer
{
public:
	CREATE_FUNC(LayerScore);
	bool init();

	void HttpResponse(CCHttpClient* client, CCHttpResponse* response);
};

#endif

