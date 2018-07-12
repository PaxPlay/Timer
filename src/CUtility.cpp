#include <cstdarg>

#include "CUtility.h"

int CUtility::Format(char *buffer, const size_t maxlength, const int argc, const char *format, ...)
{
    void *params[argc];
    size_t written;
    const char *err;

    va_list list;

    va_start(list, format);

    for (int i = 0; i < argc; i++) {
        params[i] = va_arg(list, void*);
    }

    va_end(list);

    if (phrases->FormatString(buffer, maxlength, format, params, argc, &written, &err))
        return written;

    if (err)
        smutils->LogError(myself, "Couldn't find phrase: %s.", err);
    else
        smutils->LogError(myself, "Unknown error while translating a Timer phrase.");

    return 0;
}

int CUtility::EntPropDataOffset(CBaseEntity *pEntity, const char *prop)
{
    if (!pEntity || !prop || !prop[0])
        return 0;

    datamap_t *pMap = gamehelpers->GetDataMap(pEntity);

    if (!pMap)
        return 0;

    typedescription_t *pDesc = gamehelpers->FindInDataMap(pMap, prop);

    if (!pDesc)
        return 0;

    return pDesc->fieldOffset[TD_OFFSET_NORMAL];
}

static CUtility _util;
CUtility *util = &_util;
