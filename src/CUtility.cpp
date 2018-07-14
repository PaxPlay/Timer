#include <cstdarg>

#include "CUtility.h"

int CUtility::Format(char *buffer, const size_t maxlength, const char *format, const int argc, ...)
{
    va_list list;
    va_start(list, argc);

    return FormatVA(buffer, maxlength, format, argc, list);
}

int CUtility::FormatVA(char *buffer, const size_t maxlength, const char *format, const int argc, va_list va_args)
{
    void *params[argc];
    size_t written;
    const char *err;

    for (int i = 0; i < argc; i++) {
        params[i] = va_arg(va_args, void*);
    }

    va_end(va_args);

    if (phrases->FormatString(buffer, maxlength, format, params, argc, &written, &err))
        return written;

    if (err)
        smutils->LogError(myself, "Couldn't find phrase: %s.", err);
    else
        smutils->LogError(myself, "Unknown error while translating a Timer phrase.");

    return 0;
}

void CUtility::PrintToChat(int client, const char *format, const int argc, ...)
{
    va_list list;
    va_start(list, argc);

    PrintToChatVA(client, format, argc, list);
}

void CUtility::PrintToChatVA(int client, const char *format, const int argc, va_list va_args)
{
    char buffer[253];

    if (FormatVA(buffer, 253, format, argc, va_args))
    {
        if (!gamehelpers->TextMsg(client, TEXTMSG_DEST_CHAT, buffer))
            smutils->LogError(myself, "Couldn't print to chat.");
    }
    else
    {
        smutils->LogError(myself, "PrintToChat failed: couldn't format args.");
    }
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
