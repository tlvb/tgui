#ifndef __TGUI_BUTTON_HH__
#define __TGUI_BUTTON_HH__

#include <SDL/SDL.h>
#include <utility>
#include "container.hh"

namespace tgui {

	class ButtonBase : protected SContainer {
		public:
			Uint8 press = 0;
			Uint8 release = 1;
			Uint8 click = 2;
		protected:
			Uint32 buttonstate;

			
		public:
			virtual void register_callback(EventReaction (*f)(ButtonBase*, Uint8, void*),
				Uint8 action, void* extra);

			virtual EventReaction handle_event(SDL_Event *e);
	};




}




#endif
