#ifndef __TGUI_BASE_HH__
#define __TGUI_BASE_HH__
#include <SDL/SDL.h>


namespace tGui {
	const int sr_bevel = 12;
	void horizontal_line(SDL_Surface *s, int x0, int y, int x1, Uint32 c);
	void vertical_line(SDL_Surface *s, int x, int y0, int y1, Uint32 c);
	void bersenham_line(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 c);

	void empty_rect(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_rect(SDL_Surface *s, SDL_Rect *r, Uint32 c);

	void fill_uppertri(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_lowertri(SDL_Surface *s, SDL_Rect *r, Uint32 c);

	void empty_semirect(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_semirect(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_antisemirect(SDL_Surface *s, SDL_Rect *r, Uint32 c);



}

#endif
