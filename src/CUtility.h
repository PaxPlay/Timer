#ifndef TIMER_CUTILITY_H
#define TIMER_CUTILITY_H

#include "extension.h"

#include <cstdarg>

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
    static int Format(char *buffer, const size_t maxlength, const char *format, const int argc = 0, ...);

    /**
     * Formats a string. This function wraps IPhraseCollection::FormatString and is thus needed for formatting translations.
     * @param buffer        Buffer to write to.
     * @param maxlength     Maximum amount of bytes to write.
     * @param argc          Amount of formatting arguments provided to the function.
     * @param format        Format specifier string.
     * @param va_args       Format argument list. For non string types pass a pointer to the object.
     * @return              Number of bytes written.
     */
    static int FormatVA(char *buffer, const size_t maxlength, const char *format, const int argc = 0, va_list va_args = va_list());

    /**
     * Prints to the chat. Wraps FormatVA and thus IPhraseCollection::FormatString.
     * @param client        Client index.
     * @param argc          Number of formatting arguments specified.
     * @param format        Format specifier string.
     * @param ...           Format arguments. For non string types pass a pointer to the object.
     */
    static void PrintToChat(int client, const char *format, const int argc = 0, ...);

    /**
     * Prints to the chat. Wraps FormatVA and thus IPhraseCollection::FormatString.
     * @param client        Client index.
     * @param argc          Number of formatting arguments specified.
     * @param format        Format specifier string.
     * @param va_args       Format argument list. For non string types pass a pointer to the object.
     */
    static void PrintToChatVA(int client, const char *format, const int argc = 0, va_list va_args = va_list());

    /**
     * Prints to the chat. Wraps FormatVA and thus IPhraseCollection::FormatString.
     * @param client        Client index.
     * @param argc          Number of formatting arguments specified.
     * @param format        Format specifier string.
     * @param ...           Format arguments. For non string types pass a pointer to the object.
     */
    static void PrintToConsole(int client, const char *format, const int argc = 0, ...);

    /**
     * Prints to the console. Wraps FormatVA and thus IPhraseCollection::FormatString.
     * @param client        Client index.
     * @param argc          Number of formatting arguments specified.
     * @param format        Format specifier string.
     * @param va_args       Format argument list. For non string types pass a pointer to the object.
     */
    static void PrintToConsoleVA(int client, const char *format, const int argc = 0, va_list va_args = va_list());

    static void PrintGenericOptionToConsole(int client, const char *cmd, const char *text);

    /**
     * Receives a pointer to an entities prop data field.
     * @tparam T            Field type. (this isn't checked)
     * @param pEntity       Pointer to the entity.
     * @param prop          Prop name.
     * @return              Pointer to the data field.
     */
    template <typename T>
    static T *EntPropData(CBaseEntity *pEntity, const char* prop)
    {
        int offset = EntPropDataOffset(pEntity, prop);
        if (!offset)
            return nullptr;

        return (T*)((uintptr_t)pEntity + offset);
    }

    /**
     * Receives a pointer to an entities prop send field.
     * @tparam T            Field type. (this isn't checked)
     * @param pEntity       Pointer to the entity.
     * @param classname     Classname for the entity (i.e. CCSPlayer for CSS Player).
     * @param prop          Prop name.
     * @return              Pointer to the send field.
     */
    template <typename T>
    static T *EntPropSend(CBaseEntity *pEntity, const char* classname, const char* prop) {
        int offset = EntPropSendOffset(classname, prop);
        if (!offset)
            return nullptr;

        return (T*)((uintptr_t)pEntity + offset);
    }

    /**
     *
     * @param buffer
     * @param maxlen
     * @param track
     */
    static void GetTrackName(char *buffer, int maxlen, int track);

    /**
     * Formats the time into [hours:][minutes:]seconds.
     * @param buffer        Buffer to write to.
     * @param maxlen        Maximum amount of bytes to write.
     * @param time          Time in seconds.
     * @param precision     Precision.
     */
    static void FormatTime(char *buffer, int maxlen, float time, int precision = 3);

private:
    static int EntPropDataOffset(CBaseEntity *pEntity, const char* prop);
    static int EntPropSendOffset(const char* classname, const char* prop);
};

#endif // !TIMER_CUTILITY_H
