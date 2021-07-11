#include <amtl/am-string.h>

#include "extension.h"
#include "CMapZones.h"
#include "CTimerClients.h"
#include "CUtility.h"
#include "CVFuncs.h"

#include <iplayerinfo.h>
//#include <itempents.h>

static int _stoi(const char *str, int *res) // returns the number of consumed characters
{
    int consumed = 0;
    *res = 0;
    while (*str >= '0' && *str <= '9')
    {
        *res *= 10;
        *res += *str - '0';
        str++;
        consumed++;
    }
    return consumed;
}

template <typename T>
static void resize(std::vector<T> &v, size_t size)
{
    if (v.size() < size)
        v.resize(size);
}

SH_DECL_MANUALHOOK1_void(StartTouch, 0, 0, 0, CBaseEntity*);
SH_DECL_MANUALHOOK1_void(Touch, 0, 0, 0, CBaseEntity*);
SH_DECL_MANUALHOOK1_void(EndTouch, 0, 0, 0, CBaseEntity*);

CBaseZone::CBaseZone(CBaseEntity *pEntity, int track) : m_pEntity(pEntity), m_iTrack(track)
{
    SH_ADD_MANUALHOOK_MEMFUNC(StartTouch, m_pEntity, this, &CBaseZone::StartTouch, true);
    SH_ADD_MANUALHOOK_MEMFUNC(Touch, m_pEntity, this, &CBaseZone::Touch, true);
    SH_ADD_MANUALHOOK_MEMFUNC(EndTouch, m_pEntity, this, &CBaseZone::EndTouch, true);
}

CBaseZone::~CBaseZone()
{
    SH_REMOVE_MANUALHOOK_MEMFUNC(StartTouch, m_pEntity, this, &CBaseZone::StartTouch, true);
    SH_REMOVE_MANUALHOOK_MEMFUNC(Touch, m_pEntity, this, &CBaseZone::Touch, true);
    SH_REMOVE_MANUALHOOK_MEMFUNC(EndTouch, m_pEntity, this, &CBaseZone::EndTouch, true);
}

CBaseEntity *CBaseZone::GetBaseEntity()
{
    return m_pEntity;
}

CStartZone::CStartZone(CBaseEntity *pEntity, int track) : CBaseZone(pEntity, track)
{
}

void CStartZone::StartTouch(CBaseEntity *pOther)
{
    CTimerClient *client = timerclients->GetClient(pOther);
    if (!client)
        return;

    client->BlockBhop(true);
}

void CStartZone::Touch(CBaseEntity *pOther)
{
    CTimerClient *client = timerclients->GetClient(pOther);
    if (!client)
        return;

    int flags = client->GetFlags();

    if (!(flags & FL_ONGROUND))
    {
        client->StartTimer(m_iTrack);
    }
    else
    {
        client->StopTimer(false, m_iTrack);
    }
}

void CStartZone::EndTouch(CBaseEntity *pOther)
{
    CTimerClient *client = timerclients->GetClient(pOther);
    if (!client)
        return;

    client->BlockBhop(false);
    client->StartTimer(m_iTrack);
}

CEndZone::CEndZone(CBaseEntity *pEntity, int track) : CBaseZone(pEntity, track)
{
}

void CEndZone::StartTouch(CBaseEntity *pOther)
{
    CTimerClient *client = timerclients->GetClient(pOther);
    if (!client)
        return;

    if (!client->IsRunning() || client->GetCurrentTrack() != m_iTrack)
        return;

    /*
    IPlayerInfo *playerinfo = client->GetGamePlayer()->GetPlayerInfo();
    Vector origin = playerinfo->GetAbsOrigin();
    Vector velocity = *util->EntPropData<Vector>(pOther, "m_vecVelocity");
    Vector previousOrigin = origin - velocity.Length() * globals->interval_per_tick;
    */

    /* Get the exact time the player enters the zone. */
    /* TODO: Precise timing
    Ray_t ray;
    ray.Init(origin, previousOrigin, playerinfo->GetPlayerMins(), playerinfo->GetPlayerMaxs());

    auto *pHandleEntity = reinterpret_cast<IHandleEntity *>(pOther);
    trace_t trace;

    enginetrace->ClipRayToEntity(ray, MASK_ALL, pHandleEntity, &trace);

    Vector endpos = trace.endpos;
    smutils->LogMessage(myself, "Hit: %d", trace.DidHit());
    smutils->LogMessage(myself, "Previous x: %f, y: %f, z: %f", previousOrigin.x, previousOrigin.y, previousOrigin.z);
    smutils->LogMessage(myself, "Endpos x: %f, y: %f, z: %f", endpos.x, endpos.y, endpos.z);
    smutils->LogMessage(myself, "Origin x: %f, y: %f, z: %f", origin.x, origin.y, origin.z);
    */

    client->StopTimer(true, m_iTrack);
}

CCheckpointZone::CCheckpointZone(CBaseEntity *pEntity, int track, int cpnum) : CBaseZone(pEntity, track), m_iIndex(cpnum)
{
}

void CCheckpointZone::StartTouch(CBaseEntity *pOther)
{
    CTimerClient *client = timerclients->GetClient(pOther);
    if (!client)
        return;

    client->CheckpointReached(m_iTrack, m_iIndex);
}

CMapZones::~CMapZones()
{
    ClearZones();
}

bool CMapZones::RegisterZone(CBaseEntity *pEntity, ZoneType type, int track, int cpnum)
{
    switch (type)
    {
        case ZoneType::START:
            resize(m_vStartZones, track + 1);

            if (m_vStartZones[track])
                delete m_vStartZones[track];

            m_vStartZones[track] = new CStartZone(pEntity, track);

            break;
        case ZoneType::END:
            resize(m_vEndZones, track + 1);

            if (m_vEndZones[track])
                delete m_vEndZones[track];

            m_vEndZones[track] = new CEndZone(pEntity, track);

            break;
        case ZoneType::CHECKPOINT:
            resize(m_vCheckpointZones, cpnum + 1);

            resize(m_vCheckpointZones[track], cpnum);

            if (m_vCheckpointZones[track][cpnum - 1])
                delete m_vCheckpointZones[track][cpnum - 1];

            m_vCheckpointZones[track][cpnum - 1] = new CCheckpointZone(pEntity, track, cpnum);

            break;
    }
    return false;
}

bool CMapZones::RegisterZone(CBaseEntity *pEntity, const char *classname)
{
    classname += 9; // mod_zone_
    if (strncmp(classname, "bonus_", 6) == 0)
    {
        classname += 6;
        int track;
        classname += _stoi(classname, &track);
        if (*classname && *++classname)
        { // skip the '_', also check that the string didn't end
            return RegisterZone(pEntity, classname, track);
        }
    }
    else
    {
        return RegisterZone(pEntity, classname, 0);
    }

    return false;
}

bool CMapZones::RegisterZone(CBaseEntity *pEntity, const char *identifier, int track)
{
    if (strcmp(identifier, "start") == 0)
    {
        return RegisterZone(pEntity, ZoneType::START, track);
    }
    else if (strcmp(identifier, "end") == 0)
    {
        return RegisterZone(pEntity, ZoneType::END, track);
    }
    else if (strncmp(identifier, "checkpoint_", 11) == 0)
    {
        identifier += 11;
        int cpnum;
        identifier += _stoi(identifier, &cpnum);
        if (!(*identifier))
        { // ensure that the string ended
            return RegisterZone(pEntity, ZoneType::CHECKPOINT, track, cpnum);
        }
    }

    return false;
}

void CMapZones::ClearZones()
{
    for (size_t i = 0; i < m_vStartZones.size(); i++)
    {
        delete m_vStartZones[i];
    }
    m_vStartZones.clear();

    for (size_t i = 0; i < m_vEndZones.size(); i++)
    {
        delete m_vEndZones[i];
    }
    m_vEndZones.clear();

    for (size_t i = 0; i < m_vCheckpointZones.size(); i++)
    {
        for (size_t j = 0; j < m_vCheckpointZones[i].size(); j++)
        {
            delete m_vCheckpointZones[i][j];
        }
        m_vCheckpointZones[i].clear();
    }
    m_vCheckpointZones.clear();
}

void CMapZones::PrintZones()
{

    rootconsole->ConsolePrint("----------------");
    rootconsole->ConsolePrint("Listing mapzones");
    rootconsole->ConsolePrint("----------------");

    rootconsole->ConsolePrint("Start Zones:");
    for (size_t i = 0; i < m_vStartZones.size(); i++)
    {
        rootconsole->ConsolePrint("%d:%p", i, m_vStartZones[i]->GetBaseEntity());
    }

    rootconsole->ConsolePrint("End Zones:");
    for (size_t i = 0; i < m_vEndZones.size(); i++)
    {
        rootconsole->ConsolePrint("%d:%p", i, m_vEndZones[i]->GetBaseEntity());
    }

    rootconsole->ConsolePrint("Checkpoint Zones:");
    for (size_t i = 0; i < m_vCheckpointZones.size(); i++)
    {
        for (size_t j = 0; j < m_vCheckpointZones[i].size(); j++)
            rootconsole->ConsolePrint("%d:%d:%p", i, j, m_vCheckpointZones[i][j]->GetBaseEntity());
    }
}

#define RECONFIGURE(hook)                                                   \
    if (gameconf[GAMECONF_SDKHOOKS]->GetOffset(#hook, &offset))             \
    {                                                                       \
        SH_MANUALHOOK_RECONFIGURE(hook, offset, 0, 0);                      \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        smutils->LogError(myself, "Couldn't find " #hook " offset.");       \
    }

void CMapZones::ReconfigureHooks()
{
    int offset;
    RECONFIGURE(StartTouch);
    RECONFIGURE(Touch);
    RECONFIGURE(EndTouch);
}

void CMapZones::TeleportEntityToZone(CBaseEntity *pEntity, ZoneType type, int track, int cpnum)
{
    CBaseZone *pZone = GetZone(type, track, cpnum);
    CBaseEntity *pZoneEntity = pZone->GetBaseEntity();

    // TODO: this might teleport people into the ground
    Vector origin = *util->EntPropData<Vector>(pZoneEntity, "m_vecOrigin");
    vfuncs->TeleportEntity(pEntity, &origin, nullptr, nullptr);
}

CBaseZone *CMapZones::GetZone(ZoneType type, int track, int cpnum)
{
    switch (type)
    {
        case ZoneType::START:
            if ((int) m_vStartZones.size() <= track)
                return nullptr;

            return m_vStartZones[track];
        case ZoneType::END:
            if ((int) m_vEndZones.size() <= track)
                return nullptr;

            return m_vEndZones[track];
        case ZoneType::CHECKPOINT:
            if ((int) m_vCheckpointZones.size() <= track)
                return nullptr;

            if ((int) m_vCheckpointZones[track].size() < cpnum)
                return nullptr;

            return m_vCheckpointZones[track][cpnum - 1];
    }
    return nullptr;
}

unsigned int CMapZones::GetTrackCount()
{
    return m_vStartZones.size();
}

unsigned int CMapZones::GetCPCount(unsigned int track)
{
    if (m_vCheckpointZones.size() <= track)
        return 0u;
    return m_vCheckpointZones[track].size();
}

static CMapZones _mapzones;
CMapZones *mapzones = &_mapzones;
