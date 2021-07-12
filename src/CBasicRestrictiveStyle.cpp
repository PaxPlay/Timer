#include "extension.h"
#include "CBasicRestrictiveStyle.h"

#include "CTimerClients.h"

#include <shareddefs.h>
#include <usercmd.h>
#include <const.h>
#include <in_buttons.h>

CBasicRestrictiveStyle::CBasicRestrictiveStyle(const std::string& name, const std::string& short_name)
	: m_sName(name), m_sShortName(short_name), m_bSaveReplays(true), m_bSaveSegmentedReplays(true), m_bAllowTas(true),
	  m_bSaveTasReplays(true), m_bAutoHop(true), m_bStamina(false)
{
}

CBasicRestrictiveStyle::~CBasicRestrictiveStyle()
{
}

const char* CBasicRestrictiveStyle::getName() const
{
	return m_sName.c_str();
}

const char* CBasicRestrictiveStyle::getShortName() const
{
	return m_sShortName.c_str();
}

bool CBasicRestrictiveStyle::shouldSaveReplays() const
{
	return m_bSaveReplays;
}

bool CBasicRestrictiveStyle::shouldSaveSegmentedReplays() const
{
	return m_bSaveSegmentedReplays;
}

bool CBasicRestrictiveStyle::shouldAllowTas() const
{
	return m_bAllowTas;
}

bool CBasicRestrictiveStyle::shouldSaveTasReplays() const
{
	return m_bSaveTasReplays;
}

void CBasicRestrictiveStyle::PlayerRunCmd(CTimerClient* client, CUserCmd* pCmd, IMoveHelper* movehelper)
{
	if (m_bAutoHop && pCmd->buttons & IN_JUMP && !(client->GetFlags() & FL_ONGROUND))
		pCmd->buttons &= ~IN_JUMP;

	if (!m_bStamina)
		client->SetStamina(0.0f);
}
