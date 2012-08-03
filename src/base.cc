#include "base.hh"
#define abs(a) (((a)>=0)?(a):(-(a)))

namespace tgui {

	void horizontal_line(SDL_Surface *s, int x0, int y, int x1, Uint32 c) {
		if (x0 < x1) {
			for (int x=x0; x<=x1; ++x) {
				((Uint32*)s->pixels)[x+s->w*y] = c;
			}
		}
		else {
			for (int x=x1; x<=x0; ++x) {
				((Uint32*)s->pixels)[x+s->w*y] = c;
			}
		}
	}

	void vertical_line(SDL_Surface *s, int x, int y0, int y1, Uint32 c) {
		if (y0 < y1) {
			for (int y=y0; y<=y1; ++y) {
				((Uint32*)s->pixels)[x+s->w*y] = c;
			}
		}
		else {
			for (int y=y1; y<=y0; ++y) {
				((Uint32*)s->pixels)[x+s->w*y] = c;
			}
		}
	}

	void bersenham_line(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 c) {
		if (x1-x0 == 0) {
			vertical_line(s, x0, y0, y1, c);
		}
		else if (y1-y0 == 0) {
			horizontal_line(s, x0, y0, x1, c);
		}
		else if (abs(x1-x0) < abs(y1-y0)) {
			if (y0 < y1) {
				for (int y=y0; y<=y1; ++y) {
					int x = x0 + ((x1-x0)*(y-y0))/(y1-y0);
					((Uint32*)s->pixels)[x+s->w*y] = c;
				}
			}
			else {
				for (int y=y1; y<=y0; ++y) {
					int x = x1 + ((x0-x1)*(y-y1))/(y0-y1);
					((Uint32*)s->pixels)[x+s->w*y] = c;
				}
			}
		}
		else {
			if (x0 < x1) {
				for (int x=x0; x<=x1; ++x) {
					int y = y0 + ((y1-y0)*(x-x0))/(x1-x0);
					((Uint32*)s->pixels)[x+s->w*y] = c;
				}
			}
			else {
				for (int x=x1; x<=x0; ++x) {
					int y = y1 + ((y0-y1)*(x-x1))/(x0-x1);
					((Uint32*)s->pixels)[x+s->w*y] = c;
				}
			}
		}
	}

	void empty_rect(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		horizontal_line(s, r->x, r->y, r->x+r->w-1, c);
		horizontal_line(s, r->x, r->y+r->h-1, r->x+r->w-1, c);
		vertical_line(s, r->x, r->y, r->y+r->h-1, c);
		vertical_line(s, r->x+r->w-1, r->y, r->y+r->h-1, c);
	}

	void fill_rect(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		SDL_FillRect(s, r, c);
	}

	void fill_uppertri(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		for (Uint16 i=0; i<r->h; ++i) {
			vertical_line(s, r->x+i, r->y, r->y+r->h-1-i, c);
		}
	}

	void fill_lowertri(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		for (Uint16 i=1; i<r->h; ++i) {
			vertical_line(s, r->x+i, r->y+r->h-i, r->y+r->h-1, c);
		}
	}

	void empty_semirect(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		horizontal_line(s, r->x, r->y, r->x+r->w-1, c);
		horizontal_line(s, r->x, r->y+r->h-1, r->x+r->w-sr_bevel, c);
		vertical_line(s, r->x, r->y, r->y+r->h-1, c);
		vertical_line(s, r->x+r->w-1, r->y, r->y+r->h-sr_bevel, c);
		bersenham_line(s, r->x+r->w-sr_bevel, r->y+r->h-1, r->x+r->w-1, r->y+r->h-sr_bevel, c);
	}

	void fill_semirect(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		SDL_Rect r1 = {
			.x = r->x,
			.y = r->y,
			.w = r->w,
			.h = (Uint16)((int)r->h-sr_bevel)
		};
		SDL_Rect r2 = {
			.x = r->x,
			.y = r->y,
			.w = (Uint16)((int)r->w-sr_bevel),
			.h = r->h
		};
		SDL_Rect r3 = {
			.x = (Sint16)((int)r->x+r->w-sr_bevel),
			.y = (Sint16)((int)r->y+r->h-sr_bevel),
			.w = sr_bevel,
			.h = sr_bevel
		};
		SDL_FillRect(s, &r1, c);
		SDL_FillRect(s, &r2, c);
		fill_uppertri(s, &r3, c);
	}

	void fill_antisemirect(SDL_Surface *s, SDL_Rect *r, Uint32 c) {
		SDL_Rect t = {
			.x = (Sint16)((int)r->x+r->w-sr_bevel),
			.y = (Sint16)((int)r->y+r->h-sr_bevel),
			.w = sr_bevel,
			.h = sr_bevel
		};
		fill_lowertri(s, &t, c);
	}

}
