#ifndef __TGUI_EVENT_HH__
#define __TGUI_EVENT_HH__

#include <map>
#include <utility>
#include <SDL/SDL.h>
#include "base.hh"
#include <vector>

#define TGUI_EVENTARBITER_NMB 16

namespace tgui {



	
	// eventarbiter.cc
	class EventArbiter {

		private:

			class KeysymComparator {
				public:
					bool operator()(const SDL_keysym &a, const SDL_keysym &b) const;
			};

			struct EventCallbackEntry {
				GrabID id;
				EventCallback cb;
			};

			typedef std::multimap<SDL_keysym, EventCallbackEntry, KeysymComparator> KCBMap;
			typedef std::map<int,KCBMap> KCBMapMap;


			KCBMapMap kbinds;
			KCBMapMap::iterator curKBindIter;
			KCBMapMap::iterator defKBindIter;

			std::vector<EventCallback> mbinds;

			GrabID nextGID;
			GrabContext currentContext;
			GrabContext nextContext;

			ReactionTranslator translator;
			EventCallback default_handler;
			EventCallback exclusivekeyboard;
			EventCallback exclusivemouse;

		public:
			const GrabID invalidGID = 0;
			const GrabContext invalidContext = 0;
			const GrabContext defaultContext = 1;

			EventArbiter();

			void set_reaction_translator(ReactionTranslator t);
			void set_default_event_handler(EventCallback ec);

			GrabContext add_grab_context(void);
			bool set_grab_context(GrabContext gc);

			GrabID grab_key(SDLKey sym, SDLMod mod, EventCallback ec);
			GrabID grab_key(SDLKey sym, SDLMod mod, GrabContext gc, EventCallback ec);
			bool ungrab_key(GrabID id);
			
			bool grab_button(int button, EventCallback ec);
			bool ungrab_button(int button);

			void grab_keyboard_exclusive(EventCallback ec);
			void ungrab_keyboard_exclusive(void);

			void grab_mouse_exclusive(EventCallback ec);
			void ungrab_mouse_exclusive(void);

			void event_loop(void);

		protected:
			bool update_iterators(int newContext);
			GrabID grab_key(SDL_keysym s, GrabContext gc, EventCallbackEntry kcb);
	};




}

#endif
