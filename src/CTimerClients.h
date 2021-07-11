#ifndef TIMER_CTIMERCLIENTS_H
#define TIMER_CTIMERCLIENTS_H

#include "extension.h"
#include "CBaseHud.h"

#include <shareddefs.h>

#include <amtl/am-vector.h>

class CUserCmd;
class IMoveHelper;
class CTakeDamageInfo;

class CTimerClient
{
public:
    explicit CTimerClient(int index);
    ~CTimerClient();

public:
    int GetIndex();
    IGamePlayer *GetGamePlayer();
    CBaseEntity *GetBaseEntity();
    edict_t *GetEdict();

public:
    bool IsInGame();

public:
    // This functions just wrap their equivalents in CUtility
    void PrintToChat(const char *format, int argc = 0, ...);
    void PrintToConsole(const char *format, int argc = 0, ...);

public: // Timer stuff
    bool ToggleNoclip();

    /**
     * Starts a client's timer on the specified track. If a timer is already running, it will be ignored.
     * @param track         Track index.
     */
    void StartTimer(int track);

    // TODO: this stuff could need some refactoring

    void CheckpointReached(int track, int index, float offset = 0.0f);
    void StopTimer(bool finish, int track, float offset = 0.0f);

    bool IsRunning();
    int GetCurrentTrack();
    int GetCurrentCP();
    float GetCurrentTime();

    int GetSelectedHud();

    void BlockBhop(bool block);
private:
    void ReachCheckpoint(float time);
    void Finish();

public: // Hooks
    void OnClientPutInServer();
    void PlayerRunCmd(CUserCmd *pCmd, IMoveHelper *movehelper);
    int OnTakeDamage(const CTakeDamageInfo &info);
    void Jump();

    void GameFrame();

private:
    int m_iIndex;
    IGamePlayer *m_pGamePlayer;
    edict_t *m_pEdict;
    CBaseEntity *m_pEntity;

    // timer
    bool m_bRunning;
    int m_iTrack;
    int m_iCurrentCP;
    float m_flTime;

    // settings
    int m_iHudIndex;

    bool m_bBhopBlocked;

    unsigned int m_iTicksOnGround;

public:
    // get some ent props
    Vector GetVelocity();
    int GetFlags();

private:
    // ent props
    MoveType_t *m_MoveType;
    Vector *m_vecVelocity;
    float *m_flStamina;
    int *m_fFlags;
};

class CTimerClients : public ITimedEvent
{
public:
    ~CTimerClients();

    /**
     * Creates a client object for the specified index. Supposed to be called on OnClientConnected.
     * @param index         Client index.
     * @return              True on success. False if player already exists.
     */
    bool CreateClient(int index);

    /**
     * Removes a client object for the specified index. Supposed to be called on OnClientDisconnected.
     * @param index         Client index.
     * @return              True on success. False if the player doesn't exist.
     */
    bool RemoveClient(int index);

    /**
     * Get the client of an index.
     * @param index         Client index.
     * @return              Pointer to the CTimerClient object.
     */
    CTimerClient *GetClient(int index);

    /**
     * Get the client of an entity.
     * @param index         Client BaseEntity.
     * @return              Pointer to the CTimerClient object.
     */
    CTimerClient *GetClient(CBaseEntity *pEntity);

    bool RegisterHud(CBaseHud *hud);

    bool RemoveHud(const char *name);

    void ReconfigureHooks();

public:
    ResultType OnTimer(ITimer *pTimer, void *pData) override;
    void OnTimerEnd(ITimer *pTimer, void *pData) override;
private:
    std::vector<CBaseHud *> m_vHuds;
    CTimerClient *m_Clients[MAX_PLAYERS - 1];
};

extern CTimerClients *timerclients;

#endif // !TIMER_CTIMERCLIENTS_H
