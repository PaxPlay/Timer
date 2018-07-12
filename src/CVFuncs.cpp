#include <cstdint>
#include "CVFuncs.h"

class VFuncClass {};

template < typename T1, typename ... T2>
T1 CallVFunc(void *pObj, int vTableOffset, T2 ... args) {
    auto vptr = reinterpret_cast<void***>(pObj);

#ifdef __linux__
    // Set this pointer adjustor on linux (some vFuncs won't work without this)
    union {
        T1 (VFuncClass::*mfp)(T2 ...);
        struct {
            void *addr; uintptr_t adjustor;
        } s;
    } u;
    u.s.addr = (*vptr)[vTableOffset];
    u.s.adjustor = 0;

    return (reinterpret_cast<VFuncClass *>(pObj)->*u.mfp)(args ...);
#else
    auto func = reinterpret_cast<T1 (*)(T2 ...)>((*vptr)[vTableOffset]);
    return func(args ...);
#endif
}