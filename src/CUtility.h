#ifndef TIMER_CUTILITY_H
#define TIMER_CUTILITY_H

#include "extension.h"

class CUtility
{
public:
    /**
     * Formats a string. This function wraps IPhraseCollection::FormatString and is thus needed for formatting translations.
     * @param buffer        Buffer to write to.
     * @param maxlength     Maximum amount of bytes to write.
     * @param argc          Amount of formatting arguments provided to the function.
     * @param format        Format specifier string.
     * @param ...           Format arguments. For non string types pass a pointer to the object.
     * @return              Number of bytes written.
     */
    int Format(char *buffer, const size_t maxlength, const int argc, const char *format, ...);

    /**
     * Receives a pointer to an entities prop data field.
     * @tparam T            Field type. (this isn't checked)
     * @param pEntity       Pointer to the entity.
     * @param prop          Prop name.
     * @return              Pointer to the data field.
     */
    template <typename T>
    T *EntPropData(CBaseEntity *pEntity, const char* prop) {
        int offset = EntPropDataOffset(pEntity, prop);
        if (!offset)
            return nullptr;

        return (T*)((uintptr_t)pEntity + offset);
    }

private:
    int EntPropDataOffset(CBaseEntity *pEntity, const char* prop);
};

extern CUtility *util;

#endif // !TIMER_CUTILITY_H
