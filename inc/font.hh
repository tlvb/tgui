#ifndef __TGUI_FONT_HH__
#define __TGUI_FONT_HH__

#include <SDL/SDL_ttf.h>

namespace tgui {

	TTF_Font *load_font(const char *name, const char *style, int size);
}


#endif
