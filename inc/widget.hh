#ifndef __TGUI_WIDGET_HH__
#define __TGUI_WIDGET_HH__
#include "event.hh"
#include "base.hh"
#include "font.hh"

namespace tgui {




	class Container;
	class Widget : public EventConsumer {

		protected:
			Rect bounds;
			Shape shape;
			Container *parent;
			SDL_Surface *canvas;
			Theme* theme;
			static Uint32 default_bg;
			static Uint32 default_fg;
			static TTF_Font *default_font;

		public:
			static void set_defaults(Uint32 bg, Uint32 fg, TTF_Font *font);
			Widget(void);
			bool has_parent(void) const;
			void set_parent(Container *pc);
			virtual void set_canvas(SDL_Surface *c);
			void set_preferred_shape(Shape *sh, bool doConfigure=true);
			const Shape *get_preferred_shape(void) const;
			virtual void place(SDL_Rect *b, bool doDraw=true);
			virtual void draw(void) = 0;
			virtual EventReaction handle_event(SDL_Event *e) {return 0;};

	};




}
#endif
