#include <cstring>

#include "CHookManager.h"
#include "CUtility.h"
#include "CMapZones.h"


void CHookManager::InitHooks() {

}

void CHookManager::RemoveHooks() {

}

void CheckTrigger(void *userp) {
    auto *pEntity = reinterpret_cast<CBaseEntity *>(userp);

    const char *name = *util->EntPropData<const char *>(pEntity, "m_iName");

    if (name && name[0]) {
        if (strncmp(name, "mod_zone_", 9) == 0) {
            mapzones->RegisterZone(pEntity, name);
        }
    }
}

void CHookManager::OnEntityCreated(CBaseEntity *pEntity, const char *classname) {
    if (strcmp(classname, "trigger_multiple") == 0) {
        smutils->AddFrameAction(CheckTrigger, pEntity);
    }
}

void CHookManager::OnClientPutInServer(int client) {
    edict_t *pEdict = gamehelpers->EdictOfIndex(client);
    if (!pEdict)
        return;

    IServerUnknown *pUnknown = pEdict->GetUnknown();
    if (!pUnknown)
        return;

    CBaseEntity *pEntity = pUnknown->GetBaseEntity();
    if (!pEntity)
        return;
}


static CHookManager _hooks;
CHookManager *hooks = &_hooks;
