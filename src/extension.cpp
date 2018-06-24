#include "extension.h"

/**
 * @file extension.cpp
 * @brief Implement core Timer extension class here.
 */

TimerExtension g_TimerExtension;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_TimerExtension);
