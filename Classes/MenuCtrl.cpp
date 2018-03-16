#include "MenuCtrl.h"


bool MenuCtrl::init()
{
	CCMenu::init();

	// ������������
	schedule(schedule_selector(MenuCtrl::CheckActive));

	return true;
}

void MenuCtrl::CheckActive(float)
{
	if (this->m_pSelectedItem && m_eState == kCCMenuStateTrackingTouch)
	{
		m_pSelectedItem->activate();
	}
}