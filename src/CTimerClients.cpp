#include "extension.h"
#include "CTimerClients.h"
#include "CUtility.h"
#include "CVFuncs.h"
#include "CClientCommands.h"
#include "CStyleManager.h"

#include <cstdarg>
#include <shareddefs.h>
#include <usercmd.h>
#include <const.h>
#include <in_buttons.h>

// for some reason all of these includes are needed for takedamageinfo...
#include <isaverestore.h>

#ifndef _DEBUG
#include <ehandle.h>
#else
#undef _DEBUG
#include <ehandle.h>
#define _DEBUG 1
#endif

#include <server_class.h>
#include <takedamageinfo.h>

#include "CMapZones.h"


SH_DECL_MANUALHOOK2_void(PlayerRunCmd, 0, 0, 0, CUserCmd *, IMoveHelper *);
SH_DECL_MANUALHOOK1(OnTakeDamage, 0, 0, 0, int, const CTakeDamageInfo &);

CTimerClient::CTimerClient(int index) :
        m_iIndex(index),
        m_pGamePlayer(nullptr),
		m_pEdict(nullptr),
		m_pEntity(nullptr),
        m_bRunning(false),
        m_iTrack(0),
        m_iCurrentCP(0),
        m_flTime(0.0f),
		m_pSelectedStyle(styles->getDefaultStyle()),
        m_iHudIndex(0),
        m_bBhopBlocked(false),
        m_iTicksOnGround(0)
{
}

CTimerClient::~CTimerClient()
{
    SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerRunCmd, m_pEntity, this, &CTimerClient::PlayerRunCmd, false);
    SH_REMOVE_MANUALHOOK_MEMFUNC(OnTakeDamage, m_pEntity, this, &CTimerClient::OnTakeDamage, false);
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

    CUtility::PrintToChatVA(m_iIndex, format, argc, list);
}

void CTimerClient::PrintToConsole(const char *format, int argc, ...)
{
    va_list list;
    va_start(list, argc);

    CUtility::PrintToConsoleVA(m_iIndex, format, argc, list);
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

int CTimerClient::GetSelectedHud() const
{
    return m_iHudIndex;
}

IStyle *CTimerClient::GetSelectedStyle() const
{
    return m_pSelectedStyle;
}

void CTimerClient::SetStyle(IStyle* style)
{
	if (style && style != m_pSelectedStyle)
	{
        StopTimer(false, m_iTrack);
        mapzones->TeleportEntityToZone(m_pEntity, ZoneType::START, m_iTrack);
        m_pSelectedStyle = style;
	}
}

void CTimerClient::BlockBhop(bool block)
{
    m_bBhopBlocked = block;
}

void CTimerClient::ReachCheckpoint(float time)
{
    smutils->SetGlobalTarget(m_iIndex);

    char sTrack[16];
    CUtility::GetTrackName(sTrack, 16, m_iTrack);

    char sTime[32];
    CUtility::FormatTime(sTime, 32, m_flTime);

    PrintToChat("%t", 4, "Timer Checkpoint Reached", sTrack, &m_iCurrentCP, sTime);
}

void CTimerClient::Finish()
{
    smutils->SetGlobalTarget(m_iIndex);

    char sTrack[16];
    CUtility::GetTrackName(sTrack, 16, m_iTrack);

    char sTime[32];
    CUtility::FormatTime(sTime, 32, m_flTime);

    PrintToChat("%t", 3, "Timer Finished", sTrack, sTime);

    smutils->LogMessage(myself, "[%s] %s finished with a time of %s.", sTrack, m_pGamePlayer->GetName(), sTime);
}

void CTimerClient::OnClientPutInServer()
{
    m_pGamePlayer = playerhelpers->GetGamePlayer(m_iIndex);
    m_pEdict = gamehelpers->EdictOfIndex(m_iIndex);
    m_pEntity = m_pEdict->GetUnknown()->GetBaseEntity();
	
    m_MoveType = CUtility::EntPropData<MoveType_t>(m_pEntity, "m_MoveType");
    m_vecVelocity = CUtility::EntPropData<Vector>(m_pEntity, "m_vecVelocity");
    m_flStamina = CUtility::EntPropSend<float>(m_pEntity, "CCSPlayer", "m_flStamina");
    m_fFlags = CUtility::EntPropData<int>(m_pEntity, "m_fFlags");
	
    SH_ADD_MANUALHOOK_MEMFUNC(PlayerRunCmd, m_pEntity, this, &CTimerClient::PlayerRunCmd, false);
    SH_ADD_MANUALHOOK_MEMFUNC(OnTakeDamage, m_pEntity, this, &CTimerClient::OnTakeDamage, false);
}

void CTimerClient::PlayerRunCmd(CUserCmd *pCmd, IMoveHelper *movehelper)
{
    if (*m_fFlags & FL_ONGROUND)
        m_iTicksOnGround++;
    else
        m_iTicksOnGround = 0;
	
    m_pSelectedStyle->PlayerRunCmd(this, pCmd, movehelper);

    if (m_bBhopBlocked && m_iTicksOnGround < 10)
        pCmd->buttons &= ~IN_JUMP;
}

int CTimerClient::OnTakeDamage(const CTakeDamageInfo &info)
{
    RETURN_META_VALUE(MRES_SUPERCEDE, 1);
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

void CTimerClient::SetStamina(float value)
{
    if (!m_flStamina)
        return;
    *m_flStamina = value;
}

int CTimerClient::GetTicksOnGround()
{
    if (*m_fFlags & FL_ONGROUND)
		return m_iTicksOnGround;

	return -1;
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

#define RECONFIGURE_HOOK(hook, conf) \
            if (gameconf[conf]->GetOffset(#hook, &offset))             \
                SH_MANUALHOOK_RECONFIGURE(hook, offset, 0, 0);                      \
            else                                                                    \
            smutils->LogError(myself, "Couldn't find the " #hook " offset.");

void CTimerClients::ReconfigureHooks()
{
    int offset;
    RECONFIGURE_HOOK(PlayerRunCmd, GAMECONF_SDKTOOLS);
    RECONFIGURE_HOOK(OnTakeDamage, GAMECONF_SDKHOOKS);

    smutils->AddGameFrameHook(GameFrame);
}

CTimerClient *CTimerClients::GetClient(CBaseEntity *pEntity)
{
    int index = gamehelpers->EntityToBCompatRef(pEntity);
    if (0 < index && index <= MAX_PLAYERS)
        return GetClient(index);
    return nullptr;
}

bool CTimerClients::RegisterHud(IBaseHud *hud)
{
    for (size_t i = 0; i < m_vHuds.size(); i++)
    {
        if (strcmp(hud->GetName(), m_vHuds[i]->GetName()) == 0)
            return false;
    }

    m_vHuds.push_back(hud);
    return true;
}

bool CTimerClients::RemoveHud(const char *name)
{
    for (size_t i = 0; i < m_vHuds.size(); i++)
    {
        if (strcmp(name, m_vHuds[i]->GetName()) == 0)
        {
            delete m_vHuds[i];
            m_vHuds.erase(m_vHuds.begin() + i);
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
