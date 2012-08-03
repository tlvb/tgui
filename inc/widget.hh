#ifndef __TGUI_WIDGET_HH__
#define __TGUI_WIDGET_HH__
#include "event.hh"
#include "base.hh"

namespace tgui {




	class Container;
	class Widget : public EventConsumer {

		protected:
			Rect bounds;
			Shape shape;
			Container *parent;
			SDL_Surface *canvas;

		public:
			Widget(void);
			bool has_parent(void) const;
			void set_parent(Container *pc);
			void set_canvas(SDL_Surface *c);
			void set_preferred_shape(Shape *sh, bool doConfigure=true);
			const Shape *get_preferred_shape(void) const;
			virtual void place(SDL_Rect *b, bool draw=true);
			virtual void draw(void) = 0;

	};




}
#endif
