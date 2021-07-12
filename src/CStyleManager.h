#ifndef TIMER_CSTYLEMANAGER_H
#define TIMER_CSTYLEMANAGER_H

#include <vector>

#include "IStyle.h"

class CStyleManager
{
public:
	~CStyleManager();
	void parseStyles();
	
	size_t getNumStyles() const;
	IStyle *getStyle(size_t n) const;
	IStyle* getDefaultStyle() const;
private:
	std::vector<IStyle *> m_vStyles;
	size_t m_iDefaultStyle;
};

extern CStyleManager* styles;

#endif // !TIMER_CSTYLEMANAGER_H
