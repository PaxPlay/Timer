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
        PrintOption(iterator->key.chars(), iterator->value->at(0).GetDescription());

        // If there's more than one handler listening to the same command, print all of them.
        for (size_t i = 1; i < iterator->value->length(); i++)
        {
            PrintOption(iterator->key.chars(), iterator->value->at(i).GetDescription());
        }
    }
}

static CClientCommands _clientcommands;
CClientCommands *clientcommands = &_clientcommands;
