#ifndef TIMER_CBASICRESTRICTIVESTYLE_H
#define TIMER_CBASICRESTRICTIVESTYLE_H

#include <string>

#include "IStyle.h"

class CBasicRestrictiveStyle : public IStyle
{
public:
	CBasicRestrictiveStyle();
	CBasicRestrictiveStyle(const std::string& name, const std::string& short_name);
	~CBasicRestrictiveStyle() override;

	bool setOption(const char* option, const char* value) override;
	bool setSubOption(const char* option, const char* sub_option, const char* value) override;
	
	std::string getName() const override;
	std::string getShortName() const override;

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
	bool m_bBlockOnGround;
	int m_fBlockedKeys;
};

#endif // !TIMER_CBASICRESTRICTIVESTYLE_H