#include <cstring>

#include "CHookManager.h"
#include "CUtility.h"
#include "CMapZones.h"
#include "CTimerClients.h"


void CHookManager::InitHooks()
{

}

void CHookManager::RemoveHooks()
{

}

void CheckTrigger(void *userp)
{
    auto *pEntity = reinterpret_cast<CBaseEntity *>(userp);

    const char *name = *util->EntPropData<const char *>(pEntity, "m_iName");

    if (name && name[0]) {
        if (strncmp(name, "mod_zone_", 9) == 0) {
            mapzones->RegisterZone(pEntity, name);
        }
    }
}

void CHookManager::OnEntityCreated(CBaseEntity *pEntity, const char *classname)
{
    if (strcmp(classname, "trigger_multiple") == 0)
    {
        smutils->AddFrameAction(CheckTrigger, pEntity);
    }
}

void CHookManager::OnClientConnected(int client)
{
    if (client)
        timerclients->CreateClient(client);
}

void CHookManager::OnClientPutInServer(int client)
{
    if (!client)
        return;

    CTimerClient *pClient = timerclients->GetClient(client);

    if (pClient)
        pClient->OnClientPutInServer();
}

void CHookManager::OnClientDisconnected(int client)
{
    if (client)
        timerclients->RemoveClient(client);
}

static CHookManager _hooks;
CHookManager *hooks = &_hooks;
