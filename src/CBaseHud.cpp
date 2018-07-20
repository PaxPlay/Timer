#include "CBaseHud.h"
#include "CTimerClients.h"
#include "CUtility.h"

#include <iplayerinfo.h>

const char *CBasicHud::GetName()
{
    return "Basic Hud";
}

void CBasicHud::DrawTimerInformation(CTimerClient *client)
{
    char msg[253];
    Vector vel = *util->EntPropData<Vector>(client->GetBaseEntity(), "m_vecVelocity");

    if (client->IsRunning())
    {
        smutils->Format(msg, 253,
                        "Running %d.\n"
                        "CP: %d/%d\n"
                        "Time: %f\n"
                        "Speed: %f",
                        client->GetCurrentTrack(),
                        client->GetCurrentCP(),
                        -1,
                        client->GetCurrentTime(),
                        vel.Length2D());
    }
    else
    {
        smutils->Format(msg, 253,
                        "Not Running.\n"
                        "Track: %d\n"
                        "\n"
                        "Speed: %f",
                        client->GetCurrentTrack(),
                        vel.Length2D());

    }

    gamehelpers->HintTextMsg(client->GetIndex(), msg);
}
