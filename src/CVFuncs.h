#ifndef TIMER_VFUNCS_H
#define TIMER_VFUNCS_H

#include "extension.h"

class CVFuncs
{
public:
    void TeleportEntity(CBaseEntity *pEntity, const Vector *pos, const Vector *ang, const Vector *vel);

private:
    template < typename T1, typename ... T2>
    T1 CallVFunc(void *pObj, int vTableOffset, T2 ... args);
};

extern CVFuncs *vfuncs;

#endif // !TIMER_VFUNCS_H
