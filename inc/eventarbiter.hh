#ifndef __TGUI_EVENT_ARBITER_HH__
#define __TGUI_EVENT_ARBITER_HH__

#include <map>
#include <utility>
#include <SDL/SDL.h>
#include "widget.hh"

namespace tGui {

	class EventArbiter {

		public:
			struct KeysymCallback {
				bool widgety;
				union {
					Widget *w;
					struct {
						int (*func)(SDL_Event*, void*);
						void* data;
					} s;
				} u;
			};

			class KeysymComparator {
				public:
					bool operator()(const SDL_keysym &a, const SDL_keysym &b) const;
			};

			typedef std::map<SDL_keysym, KeysymCallback, KeysymComparator> CBMap;
			typedef std::map<int,CBMap> CBMapMap;

			EventArbiter();
			bool add_grab_context(int context);
			bool set_grab_context(int context);
			bool grab_key(SDL_keysym s, Widget* h);
			bool grab_key(SDL_keysym s, int context, Widget* h);
			bool grab_key(SDL_keysym s, int (*func)(SDL_Event*, void*), void* data);
			bool grab_key(SDL_keysym s, int context, int (*func)(SDL_Event*, void*), void* data);
			void event_loop(void);

		protected:
			CBMapMap binds;
			CBMapMap::iterator curBindIter;
			CBMapMap::iterator defBindIter;
			int grabContext;

			bool update_iterators(int newContext);
			bool grab_key(SDL_keysym s, int context, KeysymCallback kscb);
			virtual bool react_on_reaction(int reaction) = 0;
			virtual int handle_event(SDL_Event *e) = 0;
	};
}

#endif
