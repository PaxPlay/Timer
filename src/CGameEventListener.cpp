#include "CGameEventListener.h"
#include "CTimerClients.h"
#include "extension.h"

#include <cstring>

void CGameEventListener::FireGameEvent(IGameEvent *event)
{
    const char *name = event->GetName();

    if (strcmp(name, "player_jump") == 0)
        player_jump(event->GetInt("userid", 0));
}

void CGameEventListener::player_jump(int userid)
{
    CTimerClient *client = timerclients->GetClient(playerhelpers->GetClientOfUserId(userid));

    client->Jump();
}

static CGameEventListener _eventmanager;
CGameEventListener *eventmanager = &_eventmanager;