#ifndef __TGUI_WINDOW_HH__
#define __TGUI_WINDOW_HH__

#include "widget.hh"
#include "abstractcontainers.hh"
#include "eventarbiter.hh"
#include <SDL/SDL.h>

namespace tGui {

	class Window : public SContainer, public EventArbiter {

		protected:
			SDL_Surface *surf;

		public:
			Window(SDL_Surface *surf);
			void configure(void);
			void draw(void);
		protected:
			bool react_on_reaction(int reaction);
			int handle_event(SDL_Event *e);


	};



}

#endif
