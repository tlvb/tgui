#ifndef __TGUI_EVENT_HH__
#define __TGUI_EVENT_HH__

#include <map>
#include <utility>
#include <SDL/SDL.h>
#include "base.hh"

namespace tgui {



	
	// eventarbiter.cc
	class EventArbiter {

		public:


		private:

			class KeysymComparator {
				public:
					bool operator()(const SDL_keysym &a, const SDL_keysym &b) const;
			};

			typedef std::multimap<SDL_keysym, KeyboardCallback, KeysymComparator> CBMap;
			typedef std::map<int,CBMap> CBMapMap;
			CBMapMap binds;
			CBMapMap::iterator curBindIter;
			CBMapMap::iterator defBindIter;

			GrabID gid;
			int grabContext;

			ReactionTranslator translator;
			EventCallback default_handler;
			EventCallback exclusivekeyboard;
			EventCallback exclusivemouse;

		public:
			const GrabID invalidGrabID = 0;
			EventArbiter();

			void set_reaction_translator(ReactionTranslator t);
			void set_default_event_handler(EventCallback ec);

			bool add_grab_context(int context);
			bool set_grab_context(int context);

			GrabID grab_key(SDL_keysym s, EventCallback ec);
			GrabID grab_key(SDL_keysym s, int context, EventCallback ec);
			bool ungrab_key(GrabID id);

			void grab_keyboard_exclusive(EventCallback ec);
			void ungrab_keyboard_exclusive(void);

			void grab_mouse_exclusive(EventCallback ec);
			void ungrab_mouse_exclusive(void);

			void event_loop(void);

		protected:
			bool update_iterators(int newContext);
			GrabID grab_key(SDL_keysym s, int context, KeyboardCallback kcb);
//			EventReaction call_grab_callback(KeyboardCallback gcb, SDL_Event *e);
	};




}

#endif
