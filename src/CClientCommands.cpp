#include "CClientCommands.h"
#include "extension.h"

CClientCommand::CClientCommand(const char *cmd, CClientCommand::COMMAND_CALLBACK callback) : m_Callback(callback)
{
    clientcommands->RegisterCommand(cmd, *this);
}

void CClientCommand::operator()(CTimerClient *client, const CCommand &args)
{
    m_Callback(client, args);
}

CClientCommands::CClientCommands()
{
    if (!m_Commands)
        m_Commands = new SourceMod::StringHashMap<ke::Vector<CClientCommand> *>;
}

CClientCommands::~CClientCommands()
{
    for (auto iterator = m_Commands->iter(); !iterator.empty(); iterator.next())
    {
        delete iterator->value;
        iterator.erase();
    }

    delete m_Commands;
}

void CClientCommands::RegisterCommand(const char *cmd, CClientCommand callback)
{
    if (!m_Commands) // initialize if this got called BEFORE the constructor
        m_Commands = new SourceMod::StringHashMap<ke::Vector<CClientCommand> *>;

    ke::Vector<CClientCommand> *cmdCallbacks;
    if (m_Commands->retrieve(cmd, &cmdCallbacks))
    {
        cmdCallbacks->append(callback);
    }

    cmdCallbacks = new ke::Vector<CClientCommand>;
    cmdCallbacks->append(callback);
    m_Commands->insert(cmd, cmdCallbacks);
}

bool CClientCommands::ProcessCommand(CTimerClient *client, const CCommand &args)
{
    ke::Vector<CClientCommand> *cmdCallbacks;
    if (m_Commands->retrieve(args[0], &cmdCallbacks))
    {
        for (size_t i = 0; i < cmdCallbacks->length(); i++)
        {
            (*cmdCallbacks)[i](client, args);
        }
        return true;
    }
    return false;
}

void CClientCommands::ListCommands()
{
    rootconsole->ConsolePrint("Displaying client commands:");
    for (auto iterator = m_Commands->iter(); !iterator.empty(); iterator.next())
    {
        rootconsole->DrawGenericOption(iterator->key.chars(), "No description provided.");
    }
}

static CClientCommands _clientcommands;
CClientCommands *clientcommands = &_clientcommands;
