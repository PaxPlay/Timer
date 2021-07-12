#include "IBaseHud.h"
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
    CUtility::GetTrackName(sTrack, 16, track);

    unsigned int iCP = client->GetCurrentCP();
    unsigned int iTotalCPs = mapzones->GetCPCount(track);
    char sCP[16];
    if (iTotalCPs)
        smutils->Format(sCP, 16, "CP: %d/%d", iCP, iTotalCPs);
    else
        strncpy(sCP, "Linear", 16);


    if (client->IsRunning())
    {
        char sTime[32];
        CUtility::FormatTime(sTime, 32, client->GetCurrentTime(), 2);

        smutils->Format(msg, 253,
                        "%s\n"
                        "%s\n"
                        "Time: %s\n"
                        "Speed: %.3f",
                        sTrack,
                        sCP,
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
