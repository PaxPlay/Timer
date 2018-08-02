#include <ISDKHooks.h>
#include <igameevents.h>

#include "extension.h"
#include "CHookManager.h"
#include "CRootConsoleCmds.h"
#include "CMapZones.h"
#include "CTimerClients.h"
#include "CGameEventManager.h"

ISDKHooks *sdkhooks = nullptr;

IServerGameClients *gameclients = nullptr;
IServerGameEnts *gameents = nullptr;
IEngineTrace *enginetrace = nullptr;
IGameEventManager2 *gameevents = nullptr;
ICvar *icvar = nullptr;

CGlobalVars *globals = nullptr;

IPhraseCollection *phrases = nullptr;
IGameConfig *gameconf[GAMECONF_TOTAL];

static CRootConsoleCmds rcmds;

const char *gameconffiles[GAMECONF_TOTAL] =
{
        "sdktools.games/game.cstrike",
        "sdkhooks.games/game.cstrike"
};

/**
 * @file extension.cpp
 * @brief Implement core Timer extension class here.
 */

bool TimerExtension::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
    sharesys->AddDependency(myself, "sdkhooks.ext", true, true);

    phrases = translator->CreatePhraseCollection();
    if (!phrases)
    {
        smutils->Format(error, maxlength, "Cannot create Phrase Collection");
        return false;
    }

    phrases->AddPhraseFile("Timer.phrases");

    rootconsole->AddRootConsoleCommand3("timer", "Timer commands", &rcmds);

    for (int i = 0; i < GAMECONF_TOTAL; i++)
    {
        if (!gameconfs->LoadGameConfigFile(gameconffiles[i], &gameconf[i], error, maxlength))
            return false;
    }

    gameevents->AddListener(eventmanager, "player_jump", true);

    hooks->InitHooks();

    mapzones->ReconfigureHooks(); // call after parsing game config files
    timerclients->ReconfigureHooks();

    if (!timerclients->RegisterHud(new CBasicHud))
    {
        smutils->Format(error, maxlength, "Cannot register the basic hud.");
        return false;
    }

    return true;
}

void TimerExtension::SDK_OnUnload()
{
    for (auto &d : gameconf)
        gameconfs->CloseGameConfigFile(d);

    phrases->Destroy();
    hooks->RemoveHooks();

    if (QueryRunning(nullptr, 0))
    {
        sdkhooks->RemoveEntityListener(hooks);
    }

    timerclients->RemoveHud("Basic Hud");
}

void TimerExtension::SDK_OnAllLoaded()
{
    SM_GET_LATE_IFACE(SDKHOOKS, sdkhooks);

    playerhelpers->AddClientListener(hooks);

    if (QueryRunning(nullptr, 0))
    {
        sdkhooks->AddEntityListener(hooks);
    }
}

void TimerExtension::SDK_OnPauseChange(bool paused)
{

    SDKExtension::SDK_OnPauseChange(paused);
}

bool TimerExtension::QueryRunning(char *error, size_t maxlength)
{
    SM_CHECK_IFACE(SDKHOOKS, sdkhooks);

    return true;
}

bool TimerExtension::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    /* Get Interfaces */
    GET_V_IFACE_CURRENT(GetServerFactory, gameclients,  IServerGameClients, INTERFACEVERSION_SERVERGAMECLIENTS);
    GET_V_IFACE_CURRENT(GetServerFactory, gameents,  IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
    GET_V_IFACE_ANY(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
    GET_V_IFACE_ANY(GetEngineFactory, gameevents, IGameEventManager2, INTERFACEVERSION_GAMEEVENTSMANAGER2);
    GET_V_IFACE_ANY(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);


    globals = ismm->GetCGlobals();

    return true;
}

TimerExtension g_TimerExtension;

SMEXT_LINK(&g_TimerExtension);