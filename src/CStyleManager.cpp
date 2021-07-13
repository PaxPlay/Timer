#include "CStyleManager.h"
#include "CBasicRestrictiveStyle.h"
#include "extension.h"

CStyleManager::~CStyleManager()
{
	for (auto &d : m_vStyles)
	{
		delete d;
	}
}

void CStyleManager::ParseStyles()
{
	char path[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_SM, path, sizeof(path), "configs/timer-styles.cfg");
	CSMCTextParser parser{ this };
	SMCStates states;
	auto error = textparsers->ParseFile_SMC(path, &parser, &states);
	if (error != SMCError_Okay)
	{
		rootconsole->ConsolePrint("Failed reading styles configuration.");
		for (auto &d : m_vStyles)
		{
			delete d;
		}
		m_vStyles.clear();
	}
	
	m_iDefaultStyle = 0;
	if (m_vStyles.empty())
	{
		rootconsole->ConsolePrint("No styles were found, adding a default style.");
		m_vStyles.push_back(new CBasicRestrictiveStyle("Normal", "N"));
	}
}

size_t CStyleManager::GetNumStyles() const
{
	return m_vStyles.size();
}

IStyle* CStyleManager::GetStyle(size_t n) const
{
	return m_vStyles[n];
}

IStyle* CStyleManager::GetDefaultStyle() const
{
	return m_vStyles[m_iDefaultStyle];
}

SourceMod::SMCResult CStyleManager::CSMCTextParser::ReadSMC_NewSection(const SourceMod::SMCStates* states,
	const char* name)
{
	switch (m_ParseStack.size())
	{
	case 0:
		if (std::string("styles") != name)
			return SMCResult_HaltFail;
		break;
	case 1:
		if (m_pCurrentStyle != nullptr)
			return SMCResult_HaltFail;
		if (std::string("basic") == name)
			m_pCurrentStyle = new CBasicRestrictiveStyle();
		break;
	case 2:
		break;
	default:
		return SMCResult_HaltFail;
	}
	m_ParseStack.push(name);
	return SMCResult_Continue;
}

SourceMod::SMCResult CStyleManager::CSMCTextParser::ReadSMC_KeyValue(const SourceMod::SMCStates* states,
	const char* key, const char* value)
{
	if (m_pCurrentStyle == nullptr)
		return SMCResult_HaltFail;
	switch (m_ParseStack.size())
	{
	case 2:
		return m_pCurrentStyle->SetOption(key, value) ? SMCResult_Continue : SMCResult_HaltFail;
	case 3:
		return m_pCurrentStyle->SetSubOption(m_ParseStack.top().c_str(),key, value)
			? SMCResult_Continue : SMCResult_HaltFail;
	default:
		return SMCResult_HaltFail;
	}
}

SourceMod::SMCResult CStyleManager::CSMCTextParser::ReadSMC_LeavingSection(const SourceMod::SMCStates* states)
{
	switch (m_ParseStack.size())
	{
	case 1: case 3:
		break;
	case 2:
		if (m_pCurrentStyle == nullptr)
			return SMCResult_HaltFail;
		if (m_pCurrentStyle->GetName().empty() || m_pCurrentStyle->GetShortName().empty())
		{
			delete m_pCurrentStyle;
			m_pCurrentStyle = nullptr;
			return SMCResult_HaltFail;
		}
		m_pStyleManager->m_vStyles.push_back(m_pCurrentStyle);
		m_pCurrentStyle = nullptr;
		break;
	default: // I genuinely don't know how we got here
		return SMCResult_HaltFail;
	}
	m_ParseStack.pop();
	return SMCResult_Continue;
}

static CStyleManager _styles;
CStyleManager* styles = &_styles;
