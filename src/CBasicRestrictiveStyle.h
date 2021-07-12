#ifndef TIMER_CBASICRESTRICTIVESTYLE_H
#define TIMER_CBASICRESTRICTIVESTYLE_H

#include <string>

#include "IStyle.h"

class CBasicRestrictiveStyle : public IStyle
{
public:
	CBasicRestrictiveStyle(const std::string& name, const std::string& short_name);
	~CBasicRestrictiveStyle() override;
	
	const char* getName() const override;
	const char* getShortName() const override;

	bool shouldSaveReplays() const override;
	bool shouldSaveSegmentedReplays() const override;

	bool shouldAllowTas() const override;
	bool shouldSaveTasReplays() const override;

	void PlayerRunCmd(CTimerClient* client, CUserCmd* pCmd, IMoveHelper* movehelper) override;
private:
	std::string m_sName;
	std::string m_sShortName;
	bool m_bSaveReplays;
	bool m_bSaveSegmentedReplays;
	bool m_bAllowTas;
	bool m_bSaveTasReplays;
	bool m_bAutoHop;
	bool m_bStamina;
};

#endif // !TIMER_CBASICRESTRICTIVESTYLE_H