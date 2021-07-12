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

void CStyleManager::parseStyles()
{
	m_iDefaultStyle = 0;
	if (m_vStyles.empty())
	{
		rootconsole->ConsolePrint("No styles were found, adding a default style.");
		m_vStyles.push_back(new CBasicRestrictiveStyle("Normal", "N"));
	}
}

size_t CStyleManager::getNumStyles() const
{
	return m_vStyles.size();
}

IStyle* CStyleManager::getStyle(size_t n) const
{
	return m_vStyles[n];
}

IStyle* CStyleManager::getDefaultStyle() const
{
	return m_vStyles[m_iDefaultStyle];
}

static CStyleManager _styles;
CStyleManager* styles = &_styles;
