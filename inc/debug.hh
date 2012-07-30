#ifndef __TGUI_DEBUG_HH__
#define __TGUI_DEBUG_HH__

#ifdef DEBUG
#include <iostream>
#include <string>

namespace tGui {
	extern int debug_indentlevel;
}

#define dpush(x) d(x); ++tGui::debug_indentlevel
#define d(x) std::cerr << std::string(tGui::debug_indentlevel*4, ' ') << x << std::endl;
#define dpop() --tGui::debug_indentlevel;

#else
#define dpush(x) {}
#define d(x) {}
#define dpop() {}
#endif

#endif
