#include "extension.h"
#include "CClientCommands.h"
#include "CTimerClients.h"
#include "CMapZones.h"
#include "CUtility.h"
#include "CAssetManager.h"
#include "CVFuncs.h"
#include "CMenuHandler.h"
#include "CStyleManager.h"

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

static CMenuHandler s_TrackMenuHandler{[](IBaseMenu *menu, CTimerClient *client, unsigned int item)
{
    const char* info = menu->GetItemInfo(item, nullptr);
	int track = strtol(info, nullptr, 10);

    client->StopTimer(false, client->GetCurrentTrack());

	mapzones->TeleportEntityToZone(client->GetBaseEntity(), ZoneType::START, track);
}};

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
// style

static CMenuHandler s_StyleMenuHandler
{[](IBaseMenu* menu, CTimerClient* client, unsigned int item)
{
    if (item >= styles->getNumStyles())
        return;

    client->SetStyle(styles->getStyle(item));
}};

CLIENT_COMMAND(style, "Select a style")
{
    IBaseMenu* menu = menus->GetDefaultStyle()->CreateMenu(&s_StyleMenuHandler);

    smutils->SetGlobalTarget(client->GetIndex());

    char sTitle[32];
    CUtility::Format(sTitle, 32, "%t", 1, "Menu Style Title");
    menu->SetDefaultTitle(sTitle);

    menu->SetPagination(6);

    for (unsigned int i = 0; i < styles->getNumStyles(); i++)
    {
        IStyle* style = styles->getStyle(i);
        menu->AppendItem(style->getShortName().c_str(), ItemDrawInfo(style->getName().c_str()));
    }

    menu->Display(client->GetIndex(), MENU_TIME_FOREVER);
}

CLIENT_COMMAND_ALIAS(mode, style);

CLIENT_COMMAND(cstyle, "Get your current style")
{
    client->PrintToChat("Your current style is %s", 1, client->GetSelectedStyle()->getName().c_str());
}

///////////////////////////////////////////////////////////////////////////
// noclip

CLIENT_COMMAND(nc, "Noclip")
{
    client->ToggleNoclip();
}