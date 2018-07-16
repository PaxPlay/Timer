#ifndef TIMER_CGAMEEVENTMANAGER_H
#define TIMER_CGAMEEVENTMANAGER_H

#include <igameevents.h>

class CGameEventManager : public IGameEventListener2
{
public:
    void FireGameEvent(IGameEvent *event) override;

private:
    void player_jump(int userid);
};

extern CGameEventManager *eventmanager;

#endif // !TIMER_CGAMEEVENTMANAGER_H
