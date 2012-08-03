#ifndef __TGUI_WINDOW_HH__
#define __TGUI_WINDOW_HH__

#include "widget.hh"
#include "event.hh"
#include "container.hh"
#include <SDL/SDL.h>

namespace tgui {

	class Window : public SContainer, public EventArbiter, public ReactionTranslator {

		public:
			Window(SDL_Surface *surf);
			virtual bool translate(EventReaction r);
			virtual void configure(void);


	};



}

#endif
