#include "extension.h"
#include "CTimerClients.h"
#include "CUtility.h"
#include "CVFuncs.h"
#include "CClientCommands.h"

#include <cstdarg>
#include <shareddefs.h>
#include <usercmd.h>
#include <const.h>
#include <in_buttons.h>

SH_DECL_MANUALHOOK2_void(PlayerRunCmd, 0, 0, 0, CUserCmd *, IMoveHelper *);

CTimerClient::CTimerClient(int index) :
        m_iIndex(index),
        m_pGamePlayer(nullptr),
        m_bRunning(false),
        m_iTrack(0),
        m_iCurrentCP(0),
        m_flTime(0.0f),
        m_iHudIndex(0),
        m_bBhopBlocked(false),
        m_iTicksOnGround(0)
{
}

CTimerClient::~CTimerClient()
{
    SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerRunCmd, m_pEntity, this, &CTimerClient::PlayerRunCmd, false);
}

int CTimerClient::GetIndex()
{
    return m_iIndex;
}

IGamePlayer *CTimerClient::GetGamePlayer()
{
    return m_pGamePlayer;
}

CBaseEntity *CTimerClient::GetBaseEntity()
{
    return m_pEntity;
}

edict_t *CTimerClient::GetEdict()
{
    return m_pEdict;
}

bool CTimerClient::IsInGame()
{
    if (!m_pGamePlayer)
        return false;

    return m_pGamePlayer->IsInGame();
}

void CTimerClient::PrintToChat(const char *format, int argc, ...)
{
    va_list list;
    va_start(list, argc);

    util->PrintToChatVA(m_iIndex, format, argc, list);
}

void CTimerClient::PrintToConsole(const char *format, int argc, ...)
{
    va_list list;
    va_start(list, argc);

    util->PrintToConsoleVA(m_iIndex, format, argc, list);
}

bool CTimerClient::ToggleNoclip()
{
    if (m_bRunning)
    {
        smutils->SetGlobalTarget(m_iIndex);
        PrintToChat("%t", 1, "Timer Stopped Noclip");
        StopTimer(false, m_iTrack);
    }

    if (*m_MoveType == MOVETYPE_NOCLIP)
    {
        *m_MoveType = MOVETYPE_WALK;
        return false;
    }
    else
    {
        *m_MoveType = MOVETYPE_NOCLIP;
        return true;
    }
}

void CTimerClient::StartTimer(int track)
{
    if (m_bRunning)
        return;

    if (*m_MoveType == MOVETYPE_NOCLIP)
        return;

    if (m_vecVelocity->Length2DSqr() > 290.0f * 290.0f)
    {
        Vector newVel(0.0f, 0.0f, 0.0f);
        vfuncs->TeleportEntity(m_pEntity, nullptr, nullptr, &newVel);
    }

    m_iTrack = track;
    m_iCurrentCP = 0;
    m_flTime = 0.0f;
    m_bRunning = true;
}

void CTimerClient::CheckpointReached(int track, int index, float offset)
{
    if (m_bRunning && track == m_iTrack && index > m_iCurrentCP)
    {
        m_iCurrentCP = index;
        ReachCheckpoint(m_flTime - offset);
    }
}

void CTimerClient::StopTimer(bool finish, int track, float offset)
{
    if (!m_bRunning || track != m_iTrack)
        return;

    if (finish)
    {
        m_flTime -= offset;

        Finish();

        m_iCurrentCP = 0;
        m_flTime = 0.0f;
        m_bRunning = false;
    }
    else
    {
        m_iCurrentCP = 0;
        m_flTime = 0.0f;
        m_bRunning = false;
    }
}

bool CTimerClient::IsRunning()
{
    return m_bRunning;
}

int CTimerClient::GetCurrentTrack()
{
    return m_iTrack;
}

int CTimerClient::GetCurrentCP()
{
    return m_iCurrentCP;
}

float CTimerClient::GetCurrentTime()
{
    return m_flTime;
}

int CTimerClient::GetSelectedHud()
{
    return m_iHudIndex;
}

void CTimerClient::BlockBhop(bool block)
{
    m_bBhopBlocked = block;
}

void CTimerClient::ReachCheckpoint(float time)
{
    smutils->SetGlobalTarget(m_iIndex);

    char sTrack[16];
    util->GetTrackName(sTrack, 16, m_iTrack);

    char sTime[32];
    util->FormatTime(sTime, 32, m_flTime, 0);

    PrintToChat("%t", 4, "Timer Checkpoint Reached", sTrack, &m_iCurrentCP, sTime);
}

void CTimerClient::Finish()
{
    smutils->LogMessage(myself, "%s finished with %.3f seconds on track %d.", m_pGamePlayer->GetName(), m_flTime, m_iTrack);

    smutils->SetGlobalTarget(m_iIndex);

    char sTrack[16];
    util->GetTrackName(sTrack, 16, m_iTrack);

    char sTime[32];
    util->FormatTime(sTime, 32, m_flTime, 0);

    PrintToChat("%t", 3, "Timer Finished", sTrack, sTime);
}

void CTimerClient::OnClientPutInServer()
{
    m_pGamePlayer = playerhelpers->GetGamePlayer(m_iIndex);
    m_pEdict = gamehelpers->EdictOfIndex(m_iIndex);
    m_pEntity = m_pEdict->GetUnknown()->GetBaseEntity();

    m_MoveType = util->EntPropData<MoveType_t>(m_pEntity, "m_MoveType");
    m_vecVelocity = util->EntPropData<Vector>(m_pEntity, "m_vecVelocity");
    m_flStamina = util->EntPropData<float>(m_pEntity, "m_flStamina");
    m_fFlags = util->EntPropData<int>(m_pEntity, "m_fFlags");

    SH_ADD_MANUALHOOK_MEMFUNC(PlayerRunCmd, m_pEntity, this, &CTimerClient::PlayerRunCmd, false);
}

void CTimerClient::PlayerRunCmd(CUserCmd *pCmd, IMoveHelper *movehelper)
{
    if (pCmd->buttons & IN_JUMP && !(*m_fFlags & FL_ONGROUND))
        pCmd->buttons &= ~IN_JUMP;


    if (*m_fFlags & FL_ONGROUND)
        m_iTicksOnGround++;
    else
        m_iTicksOnGround = 0;

    if (m_bBhopBlocked && m_iTicksOnGround < 10)
    {
        pCmd->buttons &= ~IN_JUMP;
    }
}

void CTimerClient::Jump()
{
    if (m_bBhopBlocked)
    {
        if (m_vecVelocity->Length2DSqr() > 290 * 290)
        {
            Vector vel(0.0f, 0.0f, 0.0f);
            vfuncs->TeleportEntity(m_pEntity, nullptr, nullptr, &vel);
            return;
        }
    }

    *m_flStamina = 0.0f;
}

void CTimerClient::GameFrame()
{
    if (m_bRunning)
    {
        if (*m_MoveType == MOVETYPE_NOCLIP)
        {
            StopTimer(false, m_iTrack);
            return;
        }

        m_flTime += globals->frametime;
    }
}

Vector CTimerClient::GetVelocity()
{
    return *m_vecVelocity;
}

int CTimerClient::GetFlags()
{
    return *m_fFlags;
}

bool CTimerClients::CreateClient(int index) {
    if (m_Clients[index - 1])
        return false;

    m_Clients[index - 1] = new CTimerClient(index);
    return true;
}

bool CTimerClients::RemoveClient(int index) {
    if (!m_Clients[index - 1])
        return false;

    delete m_Clients[index - 1];
    m_Clients[index - 1] = nullptr;
    return true;
}

CTimerClient *CTimerClients::GetClient(int index)
{
    return m_Clients[index - 1];
}

void GameFrame(bool simulating)
{
    if (!simulating)
        return;

    for (int i = 1; i <= MAX_PLAYERS; i++)
    {
        CTimerClient *client = timerclients->GetClient(i);
        if (client)
        {
            client->GameFrame();
        }
    }
}

void CTimerClients::ReconfigureHooks()
{
    int offset;
    if (gameconf[GAMECONF_SDKTOOLS]->GetOffset("PlayerRunCmd", &offset))
        SH_MANUALHOOK_RECONFIGURE(PlayerRunCmd, offset, 0, 0);
    else
        smutils->LogError(myself, "Couldn't find the PlayerRunCmd offset.");

    smutils->AddGameFrameHook(GameFrame);

    timersys->CreateTimer(this, 0.1, nullptr, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
}

CTimerClient *CTimerClients::GetClient(CBaseEntity *pEntity)
{
    int index = gamehelpers->EntityToBCompatRef(pEntity);
    if (0 < index && index <= MAX_PLAYERS)
        return GetClient(index);
    return nullptr;
}

bool CTimerClients::RegisterHud(CBaseHud *hud)
{
    for (size_t i = 0; i < m_vHuds.length(); i++)
    {
        if (strcmp(hud->GetName(), m_vHuds[i]->GetName()) == 0)
            return false;
    }

    m_vHuds.append(hud);
    return true;
}

bool CTimerClients::RemoveHud(const char *name)
{
    for (size_t i = 0; i < m_vHuds.length(); i++)
    {
        if (strcmp(name, m_vHuds[i]->GetName()) == 0)
        {
            delete m_vHuds[i];
            m_vHuds.remove(i);
            return true;
        }
    }
    return false;
}

CTimerClients::~CTimerClients()
{
    smutils->RemoveGameFrameHook(GameFrame);
}

ResultType CTimerClients::OnTimer(ITimer *pTimer, void *pData)
{
    for (auto &d : m_Clients)
    {
        if (d && d->IsInGame())
            m_vHuds[d->GetSelectedHud()]->DrawTimerInformation(d);
    }

    return Pl_Continue;
}

void CTimerClients::OnTimerEnd(ITimer *pTimer, void *pData)
{

}

static CTimerClients _timerclients;
CTimerClients *timerclients = &_timerclients;
