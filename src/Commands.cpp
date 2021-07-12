#include "extension.h"
#include "CClientCommands.h"
#include "CTimerClients.h"
#include "CMapZones.h"
#include "CUtility.h"
#include "CAssetManager.h"
#include "CTempEnts.h"
#include "CVFuncs.h"

#include <iplayerinfo.h>
#include <iserver.h>

///////////////////////////////////////////////////////////////////////////
// help

CLIENT_COMMAND(help, "Shows available commands")
{
    client->PrintToChat("Open you console to see available commands.");
    clientcommands->ListCommands(client);
}

CLIENT_COMMAND_ALIAS(cmds, help);

///////////////////////////////////////////////////////////////////////////
// r

CLIENT_COMMAND(r, "Teleport to the current start zone")
{
    mapzones->TeleportEntityToZone(client->GetBaseEntity(), ZoneType::START, client->GetCurrentTrack());
}

CLIENT_COMMAND_ALIAS(start, r);

///////////////////////////////////////////////////////////////////////////
// track

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

CLIENT_COMMAND(track, "Opens the track selection menu")
{
    IBaseMenu *menu = menus->GetDefaultStyle()->CreateMenu(&s_TrackMenuHandler);

    smutils->SetGlobalTarget(client->GetIndex());

    char sTitle[32];
    CUtility::Format(sTitle, 32, "%t", 1, "Menu Track Title");
    menu->SetDefaultTitle(sTitle);

    menu->SetPagination(6);

    for (unsigned int i = 0; i < mapzones->GetTrackCount(); i++) {
        char info[4];
        smutils->Format(info, 4, "%d", i);

        char desc[16];
        CUtility::GetTrackName(desc, 16, i);

        menu->AppendItem(info, ItemDrawInfo(desc));
    }

    menu->Display(client->GetIndex(), MENU_TIME_FOREVER);
}

CLIENT_COMMAND_ALIAS(bonus, track);

///////////////////////////////////////////////////////////////////////////
// noclip

CLIENT_COMMAND(nc, "Noclip.")
{
    client->ToggleNoclip();
}

class BasicRecipientFilter : public IRecipientFilter
{
private:
    int ent = -1;
public:
    BasicRecipientFilter(int ent)
    {
        this->ent = ent;
    }
	
	bool IsInitMessage() const override
	{
        return false;
	}
	
	bool IsReliable() const override
	{
        return false;
	}

	int GetRecipientCount() const override
	{
        return 1;
	}

	int GetRecipientIndex(int slot) const override
	{
        if (slot == 0)
            return ent;
        return -1;
	}

};

CLIENT_COMMAND(beam, "Draw a beam from the player's head in the direction he's looking.")
{
    Vector pos;
    gameclients->ClientEarPosition(client->GetEdict(), &pos);
    QAngle ang = vfuncs->EyeAngles(client->GetBaseEntity());
    Vector normal{ cosf(ang.y * M_PI/180), sinf(ang.y * M_PI / 180), sinf(-ang.x * M_PI / 180) };
    BasicRecipientFilter filter{ client->GetIndex() };
    CTempEnts::Beam(filter, 0.0, pos, pos + 100.0f * normal,
        assetmanager->getBeam(), assetmanager->getHalo(),
        0, 0, 10.0f, 5.0f, 5.0f, 0.0f, 0.0f,
        70, 255, 255, 255, 0);
}
