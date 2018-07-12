#ifndef TIMER_CROOTCONSOLECMDS_H
#define TIMER_CROOTCONSOLECMDS_H

#include "extension.h"

class CRootConsoleCmds : public IRootConsoleCommand {
public:
    void OnRootConsoleCommand(const char *cmdname, const ICommandArgs *args) override;
};

#endif // !TIMER_CROOTCONSOLECMDS_H
