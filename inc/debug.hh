#ifndef __TGUI_DEBUG_HH__
#define __TGUI_DEBUG_HH__

#ifdef DEBUG
#include <iostream>
#include <string>

namespace tgui {
	extern int debug_indentlevel;
}

#define dpush(x) d(x); ++tgui::debug_indentlevel
#define d(x) std::cerr << std::string(tgui::debug_indentlevel*4, ' ') << x << std::endl;
#define dpop() tgui::debug_indentlevel = tgui::debug_indentlevel>0?tgui::debug_indentlevel-1:0;

#else
#define dpush(x) {}
#define d(x) {}
#define dpop() {}
#endif

#endif
