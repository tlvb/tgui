#include "container.hh"

namespace tgui {


	EventReaction Container::apply_mouseevent_to_child(ChildInfo *ci, Uint16 x, Uint16 y, Uint8 b, SDL_Event *e) {
		if ((x >= ci->bounds.nm.x) && (x < ci->bounds.nm.x+ci->bounds.nm.w) &&
				(y >= ci->bounds.nm.y) && (y < ci->bounds.nm.y+ci->bounds.nm.h)) {
			if (!ci->hasMouse) {
				MouseState m = {
					.x = x,
					.y = y,
					.b = b
				};
				ci->hasMouse = true;
				SDL_Event e2;
				e2.type = SDL_USEREVENT;
				e2.user.code = mouseenter;
				e2.user.data1 = &m;
				return ci->w->handle_event(&e2);
			}
			else {
				return ci->w->handle_event(e);
			}
		}
		else {
			if (ci->hasMouse) {
				MouseState m = {
					.x = x,
					.y = y,
					.b = b
				};
				ci->hasMouse = false;
				SDL_Event e2;
				e2.type = SDL_USEREVENT;
				e2.user.code = mouseexit;
				e2.user.data1 = &m;
				return ci->w->handle_event(&e2);
			}
		}
		return 0;
	}




}
