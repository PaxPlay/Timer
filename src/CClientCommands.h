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

    explicit CClientCommand(const char *cmd, const char *description, COMMAND_CALLBACK callback);

    const char *GetDescription() const;

    void operator()(CTimerClient *client, const CCommand &args);
private:
    const char *m_sDescription;
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

    void ListCommands(CTimerClient *client = nullptr);
private:
    SourceMod::StringHashMap<ke::Vector<CClientCommand> *> *m_Commands;
};

extern CClientCommands *clientcommands;

/**
 * Register a client command. If the command already exists, it will listen to the command.
 *
 * Example:
 *
 * CLIENT_COMMAND(tm_test, "Test command")
 * {
 *     if (args.ArgC() > 1)
 *         client->PrintToChat("Hello there %s.", args[1]);
 * }
 */
#define CLIENT_COMMAND(name, description) \
    static void _##name##_handler(CTimerClient *client, const CCommand &args);  \
    static CClientCommand _##name##_cmd(#name, description, _##name##_handler); \
    static void _##name##_handler(CTimerClient *client, const CCommand &args)

/**
 * Register a client command alias.
 * This has to be used AFTER the client command handler was defined with CLIENT_COMMAND. (and in the same file)
 *
 * Example:
 *
 * CLIENT_COMMAND_ALIAS(sm_test, tm_test);
 */
#define CLIENT_COMMAND_ALIAS(alias, command_name) \
    static CClientCommand _##alias##_cmd(#alias, _##command_name##_cmd.GetDescription(), _##command_name##_handler)

#endif // !TIMER_CCLIENTCOMMANDS_H
