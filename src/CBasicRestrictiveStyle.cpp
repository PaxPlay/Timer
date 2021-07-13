#include "extension.h"
#include "CBasicRestrictiveStyle.h"

#include "CTimerClients.h"

#include <shareddefs.h>
#include <usercmd.h>
#include <const.h>
#include <in_buttons.h>
#include <KeyValues.h>

CBasicRestrictiveStyle::CBasicRestrictiveStyle()
	: m_bSaveReplays(true), m_bSaveSegmentedReplays(true), m_bAllowTas(true),
	m_bSaveTasReplays(true), m_bAutoHop(true), m_bStamina(false), m_bBlockOnGround(false), m_fBlockedKeys(0)
{
}

CBasicRestrictiveStyle::CBasicRestrictiveStyle(const std::string& name, const std::string& short_name)
	: CBasicRestrictiveStyle()
{
	m_sName = name;
	m_sShortName = short_name;
}

CBasicRestrictiveStyle::~CBasicRestrictiveStyle()
{
}

bool CBasicRestrictiveStyle::SetOption(const char* option, const char* value)
{
	if (std::string("name") == option)
	{
		m_sName = value;
		return true;
	}

	if (std::string("short_name") == option)
	{
		m_sShortName = value;
		return true;
	}

	struct s_bool_option
	{
		bool* option;
		const char* name;
	};

	const s_bool_option bool_options[] =
	{
		{ &m_bSaveReplays,			"save_replays" },
		{ &m_bSaveSegmentedReplays,	"save_segmented_replays" },
		{ &m_bAllowTas,				"allow_tas" },
		{ &m_bSaveTasReplays,			"save_tas_replays"},
		{ &m_bAutoHop,				"auto_hop" },
		{ &m_bStamina,				"stamina" }
	};

	std::string soption{ option };
	for (auto& d : bool_options)
	{
		if (soption == d.name)
		{
			*d.option = static_cast<bool>(std::stoi(value));

			return true;
		}
	}
	
	return false;
}

bool CBasicRestrictiveStyle::SetSubOption(const char* option, const char* sub_option, const char* value)
{
	if (std::string("restrictions") == option)
	{
		if (std::string("block_on_ground") == sub_option)
		{
			m_bBlockOnGround = static_cast<bool>(std::stoi(value));
			return true;
		}
		
		struct s_key_restriction
		{
			const char* name;
			const int key_flag;
		};

		static const s_key_restriction key_restrictions[] =
		{
			{"w", IN_FORWARD},
			{"a", IN_MOVELEFT},
			{"s", IN_BACK},
			{"d", IN_MOVERIGHT}
		};
		
		std::string key{ sub_option };
		for (auto &d : key_restrictions)
		{
			if (key == d.name)
			{
				if (std::stoi(value))
					m_fBlockedKeys |= d.key_flag;
				
				return true;
			}
		}
	}
	return false;
}

std::string CBasicRestrictiveStyle::GetName() const
{
	return m_sName;
}

std::string CBasicRestrictiveStyle::GetShortName() const
{
	return m_sShortName;
}

bool CBasicRestrictiveStyle::ShouldSaveReplays() const
{
	return m_bSaveReplays;
}

bool CBasicRestrictiveStyle::ShouldSaveSegmentedReplays() const
{
	return m_bSaveSegmentedReplays;
}

bool CBasicRestrictiveStyle::ShouldAllowTas() const
{
	return m_bAllowTas;
}

bool CBasicRestrictiveStyle::ShouldSaveTasReplays() const
{
	return m_bSaveTasReplays;
}

void CBasicRestrictiveStyle::PlayerRunCmd(CTimerClient* client, CUserCmd* pCmd, IMoveHelper* movehelper)
{
	if (m_bAutoHop && pCmd->buttons & IN_JUMP && !(client->GetFlags() & FL_ONGROUND))
		pCmd->buttons &= ~IN_JUMP;

	if (!m_bStamina)
		client->SetStamina(0.0f);

	// block movement keys if desired
	if (m_bBlockOnGround || client->GetTicksOnGround() < 10)
	{
		if ((m_fBlockedKeys & IN_FORWARD && (pCmd->buttons & IN_FORWARD || pCmd->forwardmove > 0))
			|| (m_fBlockedKeys & IN_BACK && (pCmd->buttons & IN_BACK || pCmd->forwardmove < 0)))
		{
			pCmd->forwardmove = 0;
		}
		else if ((m_fBlockedKeys & IN_MOVELEFT && (pCmd->buttons & IN_MOVELEFT || pCmd->sidemove > 0))
			|| (m_fBlockedKeys & IN_MOVERIGHT && (pCmd->buttons & IN_MOVERIGHT || pCmd->sidemove < 0)))
		{
			pCmd->sidemove = 0;
		}

		pCmd->buttons &= ~m_fBlockedKeys;
	}
}
