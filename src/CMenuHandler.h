#ifndef TIMER_CMENUHANDLER_H
#define TIMER_CMENUHANDLER_H

#include <functional>

#include <IMenuManager.h>

#include "extension.h"
#include "CTimerClients.h"

class CMenuHandler : public SourceMod::IMenuHandler
{
public:
    using CallbackFunction = void (*)(IBaseMenu*, CTimerClient*, unsigned int);
private:
    CallbackFunction callback;
public:
	CMenuHandler(CallbackFunction callback)
	{
        this->callback = callback;
	}
	
    ~CMenuHandler() = default;
	
    void OnMenuSelect(IBaseMenu* menu, int client, unsigned int item) override
    {
        CTimerClient* pClient = timerclients->GetClient(client);
        if (!pClient)
        {
            menus->CancelMenu(menu);
            return;
        }

        callback(menu, pClient, item);
    }

    void OnMenuEnd(IBaseMenu* menu, MenuEndReason reason) override
    {
        // probably not the best solution, but just calling menu->Destroy crashed my server...

        auto frameaction = [](void* userp) -> void {
            reinterpret_cast<IBaseMenu*>(userp)->Destroy();
        };
        smutils->AddFrameAction(frameaction, menu);
    }
};

#endif // !TIMER_CMENUHANDLER_H
