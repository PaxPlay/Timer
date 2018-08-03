#ifndef TIMER_IHOOKS_H
#define TIMER_IHOOKS_H

#include <ISDKHooks.h>

#include "extension.h"
#include <shareddefs.h>
#include <usercmd.h>

class CHookManager :
        public ISMEntityListener,
        public IClientListener,
        public IBitBufUserMessageListener
{
public:
    void InitHooks();
    void RemoveHooks();

    /* ISMEntityListener */
    void OnEntityCreated(CBaseEntity *pEntity, const char *classname) override;

    /* IClientListener */
    //bool InterceptClientConnect(int client, char *error, size_t maxlength) override;
    void OnClientConnected(int client) override;
    void OnClientPutInServer(int client) override;
    //void OnClientDisconnecting(int client) override;
    void OnClientDisconnected(int client) override;
    //void OnClientAuthorized(int client, const char *authstring) override;
    //void OnServerActivated(int max_clients) override;
    //bool OnClientPreAdminCheck(int client) override;
    //void OnClientPostAdminCheck(int client) override;
    //void OnMaxPlayersChanged(int newvalue) override;
    //void OnClientSettingsChanged(int client) override;

    /* IBitBufUserMessageListener */
    ResultType InterceptUserMessage(int msg_id, bf_write *bf, IRecipientFilter *pFilter) override;

    /* Custom Hooks */
    void Hook_ClientCommand(edict_t *pEntity, const CCommand &args);
};

extern CHookManager *hooks;

#endif // !TIMER_IHOOKS_H
