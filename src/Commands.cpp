#include "extension.h"
#include "CClientCommands.h"
#include "CTimerClients.h"

CLIENT_COMMAND(tm_help, "Shows available commands.")
{
    client->PrintToChat("Open you console to see available commands.");
    clientcommands->ListCommands(client);
}

CLIENT_COMMAND_ALIAS(tm_cmds, tm_help);