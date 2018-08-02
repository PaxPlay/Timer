#include "extension.h"
#include "CClientCommands.h"
#include "CTimerClients.h"
#include "CMapZones.h"

///////////////////////////////////////////////////////////////////////////
// tm_help

CLIENT_COMMAND(tm_help, "Shows available commands")
{
    client->PrintToChat("Open you console to see available commands.");
    clientcommands->ListCommands(client);
}

CLIENT_COMMAND_ALIAS(tm_cmds, tm_help);

///////////////////////////////////////////////////////////////////////////
// tm_r

CLIENT_COMMAND(tm_r, "Teleport to the current start zone")
{
    mapzones->TeleportEntityToZone(client->GetBaseEntity(), ZoneType::START, client->GetCurrentTrack());
}

CLIENT_COMMAND_ALIAS(tm_start, tm_r);

///////////////////////////////////////////////////////////////////////////
// tm_track

class TrackMenuHandler : public IMenuHandler
{
    void OnMenuSelect(IBaseMenu *menu, int client, unsigned int item) override
    {
        CTimerClient *pClient = timerclients->GetClient(client);
        if (!pClient)
        {
            menus->CancelMenu(menu);
            return;
        }

        const char *info = menu->GetItemInfo(item, nullptr);
        int track = strtol(info, nullptr, 10);

        pClient->StopTimer(false, pClient->GetCurrentTrack());

        mapzones->TeleportEntityToZone(pClient->GetBaseEntity(), ZoneType::START, track);
    }

    void OnMenuEnd(IBaseMenu *menu, MenuEndReason reason) override
    {
        // probably not the best solution, but just calling menu->Destroy crashed my server...

        auto frameaction = [](void *userp) -> void {
            reinterpret_cast<IBaseMenu *>(userp)->Destroy();
        };
        smutils->AddFrameAction(frameaction, menu);
    }
};

static TrackMenuHandler s_TrackMenuHandler;

CLIENT_COMMAND(tm_track, "Opens the track selection menu")
{

    IBaseMenu *menu = menus->GetDefaultStyle()->CreateMenu(&s_TrackMenuHandler);
    menu->SetDefaultTitle("Select a track");

    menu->SetPagination(6);

    for (unsigned int i = 0; i < mapzones->GetTrackCount(); i++)
    {
        char info[4];
        smutils->Format(info, 4, "%d", i);

        char desc[16];
        smutils->Format(desc, 16, "%d", i);

        menu->AppendItem(info, ItemDrawInfo(desc));
    }

    menu->Display(client->GetIndex(), MENU_TIME_FOREVER);
}

CLIENT_COMMAND_ALIAS(tm_bonus, tm_track);
