#include "container.hh"

namespace tgui {


	EventReaction Container::apply_mouseevent_to_child(ChildInfo *ci, int x, int y, SDL_Event *e) {
		if ((x >= ci->bounds.nm.x) && (x < ci->bounds.nm.x+ci->bounds.nm.w) &&
				(y >= ci->bounds.nm.y) && (y < ci->bounds.nm.y+ci->bounds.nm.h)) {
			if (!ci->hasMouse) {
				ci->hasMouse = true;
				SDL_Event e2;
				e2.type = SDL_USEREVENT;
				e2.user.code = mouseenter;
				e2.user.data1 = e;
				return ci->w->handle_event(&e2);
			}
			else {
				return ci->w->handle_event(e);
			}
		}
		else {
			if (ci->hasMouse) {
				ci->hasMouse = false;
				SDL_Event e2;
				e2.type = SDL_USEREVENT;
				e2.user.code = mouseexit;
				return ci->w->handle_event(&e2);
			}
		}
		return 0;
	}




}
