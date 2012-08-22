#include "label.hh"
#include "debug.hh"

namespace tgui {


	Label::Label(std::string text) {
		theme = NULL;
		ts = NULL;
		txt = text;
		font = NULL;
		render_text(false);
	}

	void Label::draw(void) {
		dpush("Label::draw()");
		if (canvas != NULL) {
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
			d(bounds.nm.x<<";"<<bounds.nm.y<<" "<<bounds.nm.w<<"x"<<bounds.nm.h);
			SDL_Rect boundscopy = bounds.nm;
			if (ts != NULL) {
				SDL_BlitSurface(ts, &r, canvas, &boundscopy);
			}
		}
		else {
			d("null canvas");
		}
		dpop();
	}

	void Label::set_text(std::string text, bool doConfigure) {
		txt = text;
		render_text(doConfigure);
	}

	void Label::render_text(bool doConfigure) {
		dpush("Label::render_text()");
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
		if (ts != NULL) {
			shape.nm.minw = ts->w;
			shape.nm.minh = ts->h;
			shape.nm.maxw = ts->w;
			shape.nm.maxh = ts->h;
		}
		else {
			shape.nm.minw = 0;
			shape.nm.minh = 0;
			shape.nm.maxw = 0;
			shape.nm.maxh = 0;
		}
				
		if (parent != NULL && doConfigure) {
			parent->configure();
		}
		dpop();
	}

}
