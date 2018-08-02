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
        m_fTime(0.0f),
        m_iHudIndex(0)
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

void CTimerClient::StartTimer(int track)
{
    if (m_bRunning)
        return;

    m_iTrack = track;
    m_iCurrentCP = 0;
    m_fTime = 0.0f;
    m_bRunning = true;
}

void CTimerClient::CheckpointReached(int track, int index, float offset)
{
    if (m_bRunning && track == m_iTrack && index > m_iCurrentCP)
    {
        m_iCurrentCP = index;
        ReachCheckpoint(m_fTime - offset);
    }
}

void CTimerClient::StopTimer(bool finish, int track, float offset)
{
    if (!m_bRunning || track != m_iTrack)
        return;

    if (finish)
    {
        m_fTime -= offset;

        Finish();

        m_iCurrentCP = 0;
        m_fTime = 0.0f;
        m_bRunning = false;
    }
    else
    {
        PrintToChat("Your timer was stopped.");

        m_iCurrentCP = 0;
        m_fTime = 0.0f;
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
    return m_fTime;
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
    PrintToChat("You reached checkpoint %d on track %d with a time of %.3f", 3, &m_iCurrentCP, &m_iTrack, &time);
}

void CTimerClient::Finish()
{
    smutils->LogMessage(myself, "%s finished with %.3f seconds on track %d.", m_pGamePlayer->GetName(), m_fTime, m_iTrack);
    PrintToChat("You finished with a time of %.3f seconds on track %d.", 2, &m_fTime, &m_iTrack);
}

void CTimerClient::OnClientPutInServer()
{
    m_pGamePlayer = playerhelpers->GetGamePlayer(m_iIndex);
    m_pEdict = gamehelpers->EdictOfIndex(m_iIndex);
    m_pEntity = m_pEdict->GetUnknown()->GetBaseEntity();

    SH_ADD_MANUALHOOK_MEMFUNC(PlayerRunCmd, m_pEntity, this, &CTimerClient::PlayerRunCmd, false);
}

void CTimerClient::PlayerRunCmd(CUserCmd *pCmd, IMoveHelper *movehelper)
{
    int flags = *util->EntPropData<int>(m_pEntity, "m_fFlags");
    if (pCmd->buttons & IN_JUMP && !(flags & FL_ONGROUND))
        pCmd->buttons &= ~IN_JUMP;


    if (flags & FL_ONGROUND)
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
        if (util->EntPropData<Vector>(m_pEntity, "m_vecVelocity")->Length2DSqr() > 290 * 290)
        {
            Vector vel(0.0f, 0.0f, 0.0f);
            vfuncs->TeleportEntity(m_pEntity, nullptr, nullptr, &vel);
            return;
        }
    }

    *util->EntPropSend<float>(m_pEntity, "m_flStamina") = 0.0f;
}

void CTimerClient::GameFrame()
{
    if (m_bRunning)
        m_fTime += globals->frametime;
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

    timersys->CreateTimer(this, 1.0, nullptr, TIMER_FLAG_REPEAT | TIMER_FLAG_NO_MAPCHANGE);
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
