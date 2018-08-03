#include "CBaseHud.h"
#include "CTimerClients.h"
#include "CUtility.h"
#include "CMapZones.h"

#include <iplayerinfo.h>

const char *CBasicHud::GetName()
{
    return "Basic Hud";
}

void CBasicHud::DrawTimerInformation(CTimerClient *client)
{
    char msg[253];
    Vector vel = client->GetVelocity();

    smutils->SetGlobalTarget(client->GetIndex());

    int track = client->GetCurrentTrack();

    char sTrack[16];
    util->GetTrackName(sTrack, 16, track);

    if (client->IsRunning())
    {
        char sTime[32];
        util->FormatTime(sTime, 32, client->GetCurrentTime(), 2);

        smutils->Format(msg, 253,
                        "%s\n"
                        "CP: %d/%d\n"
                        "Time: %s\n"
                        "Speed: %.3f",
                        sTrack,
                        client->GetCurrentCP(),
                        mapzones->GetCPCount(track),
                        sTime,
                        vel.Length2D());
    }
    else
    {
        smutils->Format(msg, 253,
                        "Not Running.\n"
                        "%s\n"
                        "\n"
                        "Speed: %.3f",
                        sTrack,
                        vel.Length2D());

    }

    gamehelpers->HintTextMsg(client->GetIndex(), msg);
}
