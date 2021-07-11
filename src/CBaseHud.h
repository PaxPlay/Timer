#ifndef TIMER_CBASEHUD_H
#define TIMER_CBASEHUD_H

#include "extension.h"

class CTimerClient;

class CBaseHud
{
public:
    virtual ~CBaseHud() = default;
    virtual const char *GetName() = 0;

public:
    virtual void DrawTimerInformation(CTimerClient *client) = 0;
};

class CBasicHud : public CBaseHud
{
public:
    const char *GetName() override;
public:
    void DrawTimerInformation(CTimerClient *client) override;
};

#endif // !TIMER_CBASEHUD_H
