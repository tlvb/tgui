#ifndef __TGUI_WIDGET_HH__
#define __TGUI_WIDGET_HH__

#include <SDL/SDL.h>
#include "defs.hh"
#include "abstractcontainers.hh"

namespace tGui {
	class Container;
	class Widget {

		protected:
			SDL_Rect bounds;
			ShapeRequest prefDims;
			Container *parent;
			SDL_Surface *surf;

		public:
			void set_parent(Container *pc);
			void set_surface(SDL_Surface *s);
			const ShapeRequest *get_preferred_dimensions(void);
			virtual void place(SDL_Rect *b);
			virtual void draw(void) = 0;
			virtual int handle_event(SDL_Event *e) = 0;

	};
}
#endif
