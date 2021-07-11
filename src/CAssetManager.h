#ifndef TIMER_CASSETMANAGER_H
#define TIMER_CASSETMANAGER_H

#include "extension.h"

class CAssetManager
{
public:
	CAssetManager();
	void precache();
	
	int getBeam();
	int getHalo();
private:
	int m_iBeam;
	int m_iHalo;
};

extern CAssetManager *assetmanager;

#endif // !TIMER_CASSETMANAGER_H
