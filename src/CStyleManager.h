#ifndef TIMER_CSTYLEMANAGER_H
#define TIMER_CSTYLEMANAGER_H

#include <vector>
#include <string>
#include <stack>

#include <ITextParsers.h>

#include "IStyle.h"

class CStyleManager
{
public:
	~CStyleManager();
	void ParseStyles();
	
	size_t GetNumStyles() const;
	IStyle *GetStyle(size_t n) const;
	IStyle* GetDefaultStyle() const;
private:
	class CSMCTextParser : public SourceMod::ITextListener_SMC
	{
	public:
		CSMCTextParser(CStyleManager *style_manager)
			: m_pStyleManager(style_manager), m_pCurrentStyle(nullptr), m_ParseStack()
		{
		}
		~CSMCTextParser() = default;

		SourceMod::SMCResult ReadSMC_NewSection(const SourceMod::SMCStates* states, const char* name) override;
		SourceMod::SMCResult ReadSMC_KeyValue(const SourceMod::SMCStates* states, const char* key, const char* value) override;
		SourceMod::SMCResult ReadSMC_LeavingSection(const SourceMod::SMCStates* states) override;
	private:
		CStyleManager* m_pStyleManager;
		IStyle* m_pCurrentStyle;
		std::stack<std::string> m_ParseStack;
	};
	
	std::vector<IStyle *> m_vStyles;
	size_t m_iDefaultStyle;
};

extern CStyleManager* styles;

#endif // !TIMER_CSTYLEMANAGER_H
