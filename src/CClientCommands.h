#ifndef TIMER_CCLIENTCOMMANDS_H
#define TIMER_CLIENTCOMMANDS_H

#include <amtl/am-vector.h>
#include <sm_stringhashmap.h>

class CTimerClient;
class CCommand;

class CClientCommand
{
public:
    typedef void (*COMMAND_CALLBACK)(CTimerClient *client, const CCommand &args);

    explicit CClientCommand(const char *cmd, COMMAND_CALLBACK callback);

    void operator()(CTimerClient *client, const CCommand &args);
private:
    COMMAND_CALLBACK m_Callback;
};

class CClientCommands
{
public:
    CClientCommands();
    ~CClientCommands();

    void RegisterCommand(const char *cmd, CClientCommand callback);

    /**
     *
     * @param client
     * @param args
     * @return
     */
    bool ProcessCommand(CTimerClient *client, const CCommand &args);

    void ListCommands();
private:
    SourceMod::StringHashMap<ke::Vector<CClientCommand> *> *m_Commands;
};

extern CClientCommands *clientcommands;

/**
 * Register a client command.
 *
 * Example:
 *
 * CLIENT_COMMAND(tm_test)
 * {
 *     if (args.ArgC() > 1)
 *         client->PrintToChat("Hello there %s.", args[1]);
 * }
 */
#define CLIENT_COMMAND(name) \
    void _##name##_handler(CTimerClient *client, const CCommand &args); \
    CClientCommand _##name##_cmd(#name, _##name##_handler);             \
    void _##name##_handler(CTimerClient *client, const CCommand &args)

#endif // !TIMER_CCLIENTCOMMANDS_H
