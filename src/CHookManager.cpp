#include <cstring>

#include "CHookManager.h"
#include "CUtility.h"
#include "CMapZones.h"
#include "CTimerClients.h"
#include "CClientCommands.h"

SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t *, const CCommand &);

void CHookManager::InitHooks()
{
    SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &CHookManager::Hook_ClientCommand, false);
}

void CHookManager::RemoveHooks()
{
    SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientCommand, gameclients, this, &CHookManager::Hook_ClientCommand, false);
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

void CHookManager::Hook_ClientCommand(edict_t *pEntity, const CCommand &args)
{
    CTimerClient *client = timerclients->GetClient(gamehelpers->IndexOfEdict(pEntity));
    if (!client)
        RETURN_META(MRES_IGNORED);

    if (clientcommands->ProcessCommand(client, args))
        RETURN_META(MRES_SUPERCEDE);
    else
        RETURN_META(MRES_IGNORED);
}

static CHookManager _hooks;
CHookManager *hooks = &_hooks;
