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
    auto *params = new void*[argc];
    size_t written;
    const char *err;

    for (size_t i = 0; i < argc; i++) {
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

void CUtility::GetTrackName(char *buffer, int maxlen, int track)
{
    if (track == 0)
        Format(buffer, maxlen, "%t", 1, "Main");
    else
        Format(buffer, maxlen, "%t", 2, "Bonus", &track);
}

void CUtility::FormatTime(char *buffer, int maxlen, float time, int precision)
{
    char sTmp[16];
    buffer[0] = 0; // force the string to be 'empty'

    if (time > 3600.0f)
    {
        int hours = (int)(time / 3600.0f);
        smutils->Format(buffer, maxlen, "%d:", hours);

        time -= hours * 3600.0f;
    }

    if (time > 60.0f || buffer[0])
    {
        int minutes = (int)(time / 60.0f);
        smutils->Format(sTmp, 16, "%02d:", minutes);

        strncat(buffer, sTmp, maxlen);

        time -= minutes * 60.0f;
    }

    smutils->Format(sTmp, 16, "%0*.*f", precision + 3, precision, time);
    strncat(buffer, sTmp, maxlen);
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

int CUtility::EntPropSendOffset(const char* classname, const char *prop)
{
    if (!classname || !classname[0] || !prop || !prop[0])
        return 0;

    sm_sendprop_info_t sendprop_info;
    
    if (!gamehelpers->FindSendPropInfo(classname, prop, &sendprop_info))
        return 0;
	
    return sendprop_info.actual_offset;
}
