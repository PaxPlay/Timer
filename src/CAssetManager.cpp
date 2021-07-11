#include "CAssetManager.h"

CAssetManager::CAssetManager()
{
	m_iBeam = -1;
	m_iHalo = -1;
}

void CAssetManager::precache()
{
	m_iBeam = engine->PrecacheModel("sprites/laser.vmt", true);
	m_iHalo = engine->PrecacheModel("sprites/halo01.vmt", true);
}

int CAssetManager::getBeam()
{
	return m_iBeam;
}

int CAssetManager::getHalo()
{
	return m_iHalo;
}

static CAssetManager _assetmanager;
CAssetManager* assetmanager = &_assetmanager;
