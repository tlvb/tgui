#include "label.hh"

namespace tgui {


	Label::Label(std::string text) {
		theme = NULL;
		ts = NULL;
		txt = text;
		font = NULL;
		render_text();
	}

	void Label::draw(void) {
		SDL_Rect r = {
			.x = 0,
			.y = 0,
			.w = bounds.nm.w,
			.h = bounds.nm.h
		};
		if (theme != NULL) {
#if 0
			fill_rect(canvas, &bounds.nm, th->nc);
#endif
		}
		else {
			fill_rect(canvas, &bounds.nm, default_bg);
		}

		SDL_Rect boundscopy = bounds.nm;
		SDL_BlitSurface(ts, &r, canvas, &boundscopy);
	}

	void Label::set_text(std::string text) {
		txt = text;
		render_text();
	}

	void Label::render_text(void) {
		if (ts != NULL) {
			SDL_FreeSurface(ts);
		}
		if (theme != NULL) {
#if 0
			SDL_Color tc = {
				.r=(Uint8)((th->tc>>16)&0xff),
				.g=(Uint8)((th->tc>>8)&0xff),
				.b=(Uint8)((th->tc>>0)&0xff)
			};
			ts = TTF_RenderText_Blended(th->font, txt.c_str(), tc);
#endif
		}
		else {
			SDL_Color tc = {
				.r=(Uint8)((default_fg>>16)&0xff),
				.g=(Uint8)((default_fg>>8)&0xff),
				.b=(Uint8)((default_fg>>0)&0xff)
			};
			ts = TTF_RenderText_Blended(default_font, txt.c_str(), tc);
		}
		shape.nm.minw = ts->w;
		shape.nm.minh = ts->h;
		shape.nm.maxw = ts->w;
		shape.nm.maxh = ts->h;
		shape.nm.hgrav = left;
		shape.nm.vgrav = center;
		if (parent != NULL) {
			parent->configure();
		}
	}

}
