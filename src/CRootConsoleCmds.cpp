#include "CRootConsoleCmds.h"
#include "CMapZones.h"
#include "CClientCommands.h"

void CRootConsoleCmds::OnRootConsoleCommand(const char *cmdname, const ICommandArgs *args)
{
    if (strcmp(cmdname, "timer") == 0)
    {

        if (args->ArgC() >= 3)
        {

            /* COMMANDS */

            if (strcmp(args->Arg(2), "debug") == 0)
            {
                if (args->ArgC() >= 4) {

                    /* DEBUG COMMANDS */

                    if (strcmp(args->Arg(3), "zones") == 0)
                    {
                        mapzones->PrintZones();
                    }

                }
                else
                {
                    rootconsole->ConsolePrint("Usage: sm timer debug <arg>.");
                    rootconsole->DrawGenericOption("zones", "Print hooked mapzones");
                }
            }
            else if (strcmp(args->Arg(2), "cmds") == 0)
            {
                clientcommands->ListCommands();
            }
        }
        else
        {
            rootconsole->ConsolePrint("Usage: sm timer <command>.");
            rootconsole->DrawGenericOption("debug", "Print debug information");
            rootconsole->DrawGenericOption("cmds", "Prints available commands");
        }

    }
}
