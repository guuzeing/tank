
#ifndef __AI_H__
#define __AI_H__

#include "Common.h"

class AI : public CCNode
{
public:
	CREATE_FUNC(AI);
	bool init();
	void onExit();

	CCTMXTiledMap* getMap()
	{
		return (CCTMXTiledMap*)getParent();
	}

	// �����о�̹��
	void createTank(float);
	bool _createFlag;

	CCArray* _tanks;
	CCArray* _bullets;

	// �ƶ��о�̹��
	void moveTank(float);

	// �õо�̹�˷����ӵ�
	void Shoot(float);

	// ��ײ���
	void update(float);

	bool			_pause;
	void pause();
	void resume(float);
	
};

#endif

