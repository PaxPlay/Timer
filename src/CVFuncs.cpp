#include <cstdint>
#include "CVFuncs.h"

class VFuncClass {};

void CVFuncs::TeleportEntity(CBaseEntity *pEntity, const Vector *pos, const QAngle *ang, const Vector *vel)
{
    int offset;
    if (!gameconf[GAMECONF_SDKTOOLS]->GetOffset("Teleport", &offset))
        smutils->LogError(myself, "Couldn't get Teleport offset.");
    CallVFunc<void, const Vector *, const QAngle *, const Vector *>(pEntity, offset, pos, ang, vel);
	
}

QAngle CVFuncs::EyeAngles(CBaseEntity* pEntity)
{
    int offset;
    if (!gameconf[GAMECONF_SDKTOOLS]->GetOffset("EyeAngles", &offset))
        smutils->LogError(myself, "Couldn't get EyeAngles offset.");
    return CallVFunc<QAngle>(pEntity, offset);
}

template < typename T1, typename ... T2>
T1 CVFuncs::CallVFunc(void *pObj, int vTableOffset, T2 ... args)
{	
    auto vptr = reinterpret_cast<void***>(pObj);

    union
    {
        T1 (VFuncClass::*mfp)(T2 ...);
        struct
        {
            void *addr;
#ifdef __linux__ // Set this pointer adjustor on linux (some vFuncs won't work without this)
        	uintptr_t adjustor;
#endif
        }
        s;
    } u;
    u.s.addr = (*vptr)[vTableOffset];
#ifdef __linux__
    u.s.adjustor = 0;
#endif

    return (reinterpret_cast<VFuncClass *>(pObj)->*u.mfp)(args ...);
}

static CVFuncs _vfuncs;
CVFuncs *vfuncs = &_vfuncs;
