#ifndef __TGUI_EVENT_HH__
#define __TGUI_EVENT_HH__

#include <map>
#include <utility>
#include <SDL/SDL.h>

namespace tgui {

	typedef unsigned int EventReaction;
	typedef unsigned long long GrabID;

	const EventReaction QUIT = 1;
	const EventReaction UPDATE_SCREEN = 2;

	class EventConsumer {
		public:
			virtual EventReaction handle_event(SDL_Event *e) = 0;
	};

	class ReactionTranslator {
		public:
			virtual bool translate(EventReaction r) = 0;
	};



	
	// eventarbiter.cc
	class EventArbiter {

		public:


		private:
			union GrabCallback {
				GrabID id;
				struct {
					GrabID id;
					EventConsumer *ec;
				} consumer;
				struct {
					GrabID id;
					EventReaction (*f)(SDL_Event*, void*);
					void* data;
				} func;
			};

			class KeysymComparator {
				public:
					bool operator()(const SDL_keysym &a, const SDL_keysym &b) const;
			};

			typedef std::multimap<SDL_keysym, GrabCallback, KeysymComparator> CBMap;
			typedef std::map<int,CBMap> CBMapMap;
			CBMapMap binds;
			CBMapMap::iterator curBindIter;
			CBMapMap::iterator defBindIter;

			GrabID gid;
			int grabContext;

			ReactionTranslator *translator;
			GrabCallback default_handler;
			GrabCallback exclusivekeyboard;
			GrabCallback exclusivemouse;

		public:
			EventArbiter();

			void set_reaction_translator(ReactionTranslator *t);
			void set_default_event_handler(EventConsumer *ec);
			void set_default_event_handler(SDL_keysym s, EventReaction (*func)(SDL_Event*, void*), void* data);

			bool add_grab_context(int context);
			bool set_grab_context(int context);

			GrabID grab_key(SDL_keysym s, EventConsumer* h);
			GrabID grab_key(SDL_keysym s, int context, EventConsumer* h);
			GrabID grab_key(SDL_keysym s, EventReaction (*func)(SDL_Event*, void*), void* data);
			GrabID grab_key(SDL_keysym s, int context, EventReaction (*func)(SDL_Event*, void*), void* data);
			bool ungrab_key(GrabID id);

			void grab_keyboard_exclusive(EventConsumer *h);
			void grab_keyboard_exclusive(EventReaction (*func)(SDL_Event*, void*), void *data);
			void ungrab_keyboard_exclusive(void);

			void grab_mouse_exclusive(EventConsumer *h);
			void grab_mouse_exclusive(EventReaction (*func)(SDL_Event*, void*), void *data);
			void ungrab_mouse_exclusive(void);

			void event_loop(void);

		protected:
			bool update_iterators(int newContext);
			GrabID grab_key(SDL_keysym s, int context,  GrabCallback gcb);
			EventReaction call_grab_callback(GrabCallback gcb, SDL_Event *e);
	};




}

#endif
