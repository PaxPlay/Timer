#ifndef TIMER_CMAPZONES_H
#define TIMER_CMAPZONES_H

#include <amtl/am-vector.h>
#include "extension.h"

enum class ZoneType
{
    START,
    END,
    CHECKPOINT
};

class CBaseZone
{
public:
    CBaseZone(CBaseEntity *pEntity, int track);
    ~CBaseZone();

    virtual void StartTouch(CBaseEntity *pOther) {}
    virtual void Touch(CBaseEntity *pOther) {}
    virtual void EndTouch(CBaseEntity *pOther) {}

    CBaseEntity *GetBaseEntity();
protected:
    CBaseEntity *m_pEntity;
    int m_iTrack;
};

class CStartZone : public CBaseZone
{
public:
    CStartZone(CBaseEntity *pEntity, int track);

    void StartTouch(CBaseEntity *pOther) override;
    void Touch(CBaseEntity *pOther) override;
    void EndTouch(CBaseEntity *pOther) override;
};

class CEndZone : public CBaseZone
{
public:
    CEndZone(CBaseEntity *pEntity, int track);

    void StartTouch(CBaseEntity *pOther) override;
};

class CCheckpointZone : public CBaseZone
{
public:
    CCheckpointZone(CBaseEntity *pEntity, int track, int cpnum);

    void StartTouch(CBaseEntity *pOther) override;
private:
    int m_iIndex;
};

class CMapZones
{
public:
    void PrintZones();

    /**
     * Reconfigures Trigger hooks with offsets from the game configs. This will delete all hooks.
     */
    void ReconfigureHooks();

    /**
     * Registers a zone. If this specific zone already exists, it will be overwritten.
     *
     * @param pEntity       BaseEntity of the trigger.
     * @param type          Type of the zone.
     * @param track         Track index. (0 = main, track >= 1 = bonus with index track)
     * @param cpnum         Index of checkpoint. Only needed and used if type is ZoneType::CHECKPOINT.
     * @return              True on succes.
     */
    bool RegisterZone(CBaseEntity *pEntity, ZoneType type, int track, int cpnum = 0);

    /**
     * Registers a mod_zone_ start zone based on its classname. (https://github.com/3331/fly)
     *
     * @param pEntity       BaseEntity of the trigger.
     * @param classname     Classname of the trigger.
     * @return              True on succes.
     */
    bool RegisterZone(CBaseEntity *pEntity, const char* classname);

    void TeleportEntityToZone(CBaseEntity *pEntity, ZoneType type, int track, int cpnum = 0);

    unsigned int GetTrackCount();

    unsigned int GetCPCount(int track);
private:
    bool RegisterZone(CBaseEntity *pEntity, const char* identifier, int track);
    CBaseZone *GetZone(ZoneType type, int track, int cpnum = 0);

private:
    ke::Vector<CStartZone *> m_vStartZones;
    ke::Vector<CEndZone *> m_vEndZones;
    ke::Vector<ke::Vector<CCheckpointZone *>> m_vCheckpointZones;
};

extern CMapZones *mapzones;

#endif // !TIMER_CMAPZONES_H
