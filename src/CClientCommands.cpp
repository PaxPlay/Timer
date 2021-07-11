#include "CClientCommands.h"
#include "CUtility.h"
#include "CTimerClients.h"
#include "extension.h"

CClientCommand::CClientCommand(const char *cmd, const char *description, COMMAND_CALLBACK callback)
        : m_sDescription(description),
          m_Callback(callback)
{
    clientcommands->RegisterCommand(cmd, *this);
}

const char *CClientCommand::GetDescription() const
{
    return m_sDescription;
}

void CClientCommand::operator()(CTimerClient *client, const CCommand &args)
{
    m_Callback(client, args);
}

CClientCommands::CClientCommands()
{
    if (!m_Commands)
        m_Commands = new SourceMod::StringHashMap<std::vector<CClientCommand> *>;
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
        m_Commands = new SourceMod::StringHashMap<std::vector<CClientCommand> *>;

    std::vector<CClientCommand> *cmdCallbacks;
    if (m_Commands->retrieve(cmd, &cmdCallbacks))
    {
        cmdCallbacks->push_back(callback);
    }

    cmdCallbacks = new std::vector<CClientCommand>;
    cmdCallbacks->push_back(callback);
    m_Commands->insert(cmd, cmdCallbacks);
}

bool CClientCommands::ProcessConsoleCommand(CTimerClient *client, const CCommand &args)
{
    const char *cmd = args[0];
    if (!(strncmp(cmd, "t_", 2) == 0 && cmd[2]))
        return false;

    cmd += 2;

    std::vector<CClientCommand> *cmdCallbacks;
    if (m_Commands->retrieve(cmd, &cmdCallbacks))
    {
        for (size_t i = 0; i < cmdCallbacks->size(); i++)
        {
            (*cmdCallbacks)[i](client, args);
        }
        return true;
    }
    return false;
}

bool CClientCommands::ProcessChatCommand(CTimerClient *client, const char *cmd)
{
    if ((cmd[0] != '!' && cmd[0] != '/') || !cmd[1])
        return false;

    cmd++;

    if (m_Commands->contains(cmd))
        engine->ClientCommand(client->GetEdict(), "t_%s", cmd);

    return true;
}

void CClientCommands::ListCommands(CTimerClient *client)
{
    auto PrintOption = [&](const char *cmd, const char *text)
    {
        if (client)
            util->PrintGenericOptionToConsole(client->GetIndex(), cmd, text);
        else
            rootconsole->DrawGenericOption(cmd, text);
    };


    if (client)
        client->PrintToConsole("Displaying available commands:");
    else
        rootconsole->ConsolePrint("Displaying client commands:");

    for (auto iterator = m_Commands->iter(); !iterator.empty(); iterator.next())
    {
        PrintOption(iterator->key.c_str(), iterator->value->at(0).GetDescription());

        // If there's more than one handler listening to the same command, print all of them.
        for (size_t i = 1; i < iterator->value->size(); i++)
        {
            PrintOption(iterator->key.c_str(), iterator->value->at(i).GetDescription());
        }
    }
}

static CClientCommands _clientcommands;
CClientCommands *clientcommands = &_clientcommands;
