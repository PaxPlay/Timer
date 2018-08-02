#include <cstdarg>

#include "CUtility.h"

#include <amtl/am-string.h>

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

void CUtility::PrintToConsole(int client, const char *format, const int argc, ...)
{
    va_list list;
    va_start(list, argc);

    PrintToConsoleVA(client, format, argc, list);
}

void CUtility::PrintToConsoleVA(int client, const char *format, const int argc, va_list va_args)
{
    char buffer[253];

    if (FormatVA(buffer, 253, format, argc, va_args))
    {
        if (!gamehelpers->TextMsg(client, TEXTMSG_DEST_CONSOLE, buffer))
            smutils->LogError(myself, "Couldn't print to console.");
    }
    else
    {
        smutils->LogError(myself, "PrintToConsole failed: couldn't format args.");
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

int CUtility::EntPropSendOffset(CBaseEntity *pEntity, const char *prop)
{
    if (!pEntity || !prop || !prop[0])
        return 0;

    const char* classname = gamehelpers->GetEntityClassname(pEntity);

    if (!classname)
        return 0;

    return gamehelpers->GetSendPropOffset(gamehelpers->FindInSendTable(classname, prop));
}

void CUtility::PrintGenericOptionToConsole(int client, const char *cmd, const char *text)
{
    // shamelessly copied from rootconsolemenu
    char buffer[255];
    size_t len, cmdlen = strlen(cmd);

    len = ke::SafeSprintf(buffer, sizeof(buffer), "    %s", cmd);
    if (cmdlen < 16)
    {
        size_t num = 16 - cmdlen;
        for (size_t i = 0; i < num; i++)
        {
            buffer[len++] = ' ';
        }
        len += ke::SafeSprintf(&buffer[len], sizeof(buffer) - len, " - %s", text);
        PrintToConsole(client, "%s", 1, buffer);
    }
}

static CUtility _util;
CUtility *util = &_util;
