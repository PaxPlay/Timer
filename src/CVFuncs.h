#ifndef TIMER_VFUNCS_H
#define TIMER_VFUNCS_H


class CVFuncs {
    template < typename T1, typename ... T2>
    T1 CallVFunc(void *pObj, int vTableOffset, T2 ... args);
};

#endif // !TIMER_VFUNCS_H
