#include "event.hh"
#include "debug.hh"

#include <iostream>

namespace tgui{

	EventArbiter::EventArbiter() {
		dpush("EventArbiter::EventArbiter()");
		nextContext = invalidContext + 1;
		currentContext = add_grab_context();
		update_iterators(defaultContext);
		set_grab_context(defaultContext);
		translator = ReactionTranslator();
		default_handler = EventCallback();
		exclusivekeyboard = EventCallback();
		exclusivemouse = EventCallback();
		nextGID = invalidGID + 1;
		mbinds = std::vector<EventCallback>(TGUI_EVENTARBITER_NMB);
		dpop();
	}

	bool EventArbiter::KeysymComparator::operator()(const SDL_keysym &a, const SDL_keysym &b) const {
		if (a.sym != b.sym)
			return a.sym<b.sym;
		return a.mod<b.mod;
	}

	void EventArbiter::set_reaction_translator(ReactionTranslator t) {
		translator = t;
	}

	void EventArbiter::set_default_event_handler(EventCallback ec) {
		d("EventArbiter::set_default_event_handler() EC");
		default_handler = ec;
	}

	GrabContext EventArbiter::add_grab_context(void) {
		dpush("EventArbiter::add_grab_context()");
		GrabContext gc = nextContext++;
		std::pair<KCBMapMap::iterator, bool> ret;
		ret = kbinds.insert(std::pair<int, KCBMap>(gc, KCBMap()));
		d("insert returned "<<ret.second);
		update_iterators(currentContext);
		dpop();
		if (ret.second) {
			return gc;
		}
		else {
			return invalidContext;
		}
	}

	bool EventArbiter::set_grab_context(GrabContext gc) {
		return update_iterators(gc);
	}

	GrabID EventArbiter::grab_key(SDLKey sym, SDLMod mod, EventCallback ec) {
		return grab_key(sym, mod, currentContext, ec);
	}

	GrabID EventArbiter::grab_key(SDLKey sym, SDLMod mod, GrabContext gc, EventCallback ec) {
		SDL_keysym key = {
			.scancode = 0,
			.sym = sym,
			.mod = mod,
			.unicode = 0
		};
		EventCallbackEntry kcb = {
			.id = nextGID++,
			.cb = ec
		};
		return grab_key(key, gc, kcb);
	}

	bool EventArbiter::ungrab_key(GrabID id) {
		for(KCBMapMap::iterator outer=kbinds.begin(); outer!=kbinds.end(); ++outer) {
			for (KCBMap::iterator inner=outer->second.begin(); inner!=outer->second.end(); ++inner) {
				if (inner->second.id == id) {
					outer->second.erase(inner);
					update_iterators(currentContext);
					return true;
				}
			}
		}
		return false;
	}


	bool EventArbiter::grab_button(int button, EventCallback ec) {
		if (button <= TGUI_EVENTARBITER_NMB) {
			if (mbinds[button-1] == NULL) {
				mbinds[button-1] = ec;
				return true;
			}
		}
		return false;
	}

	bool EventArbiter::ungrab_button(int button) {
		if (button <= TGUI_EVENTARBITER_NMB) {
			if (!mbinds[button-1]) {
				mbinds[button-1] = EventCallback();
				return true;
			}
		}
		return false;
	}

	void EventArbiter::grab_keyboard_exclusive(EventCallback ec) {
		exclusivekeyboard = ec;
	}

	void EventArbiter::ungrab_keyboard_exclusive(void) {
		exclusivekeyboard = EventCallback();
	}


	void EventArbiter::grab_mouse_exclusive(EventCallback ec) {
		exclusivemouse = ec;
	}

	void EventArbiter::ungrab_mouse_exclusive(void) {
		exclusivemouse = EventCallback();
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, GrabContext gc, EventCallbackEntry kcb) {
		dpush("EventArbiter::grab_key (protected)");
		KCBMapMap::iterator it = kbinds.find(gc);

		if (it != kbinds.end()) {
			d("found context");
			KCBMap::iterator ret;
			ret = it->second.insert(std::pair<SDL_keysym, EventCallbackEntry>(s, kcb));
			dpop();
			return kcb.id;
		}
		else {
			d("did not find context");
			dpop();
			return invalidGID;
		}
	}

	void EventArbiter::event_loop(void) {
		dpush("EventArbiter::event_loop()");
		if (!translator) {
			d("no translator set");
			dpop();
			return;
		}
		SDL_Event e;
		for (;;) {
			SDL_WaitEvent(&e);
			EventReaction reaction = 0;
			bool handled = false;
			switch (e.type) {
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					if (exclusivekeyboard) {
						reaction |= exclusivekeyboard(&e);
						handled = true;
					}
					else {
						std::pair<KCBMap::iterator,KCBMap::iterator> it = curKBindIter->second.equal_range(e.key.keysym);
						bool found = (it.first != it.second);
						if (!found) {
							d("did not find bind, searching defaults");
							it = defKBindIter->second.equal_range(e.key.keysym);
							found = (it.first != it.second);
						}
						if (found) {
							d("found bind");
							d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" grabbed");
							for (KCBMap::iterator ju=it.first; ju!=it.second; ++ju) {
								reaction |= ju->second.cb(&e);
							}
							handled = true;
						}
						else {
							d("did not find bind");
							d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" not grabbed");
						}
					}
					break;
				}
				case SDL_MOUSEMOTION:
					if (exclusivemouse) {
						d("exclusive lock")
						reaction |= exclusivemouse(&e);
						handled = true;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					d("something mousey happened");
					if (exclusivemouse) {
						d("exclusive lock")
						reaction |= exclusivemouse(&e);
						handled = true;
					}
					else if (e.button.button <= TGUI_EVENTARBITER_NMB) {
						if (mbinds[e.button.button-1]) {
							reaction |= mbinds[e.button.button-1](&e);
							handled = true;
						}
					}
					break;
				case SDL_USEREVENT:
					if (e.user.code == void_void_callback) {
						std::cout<<"[[EVENTARBITER VOID VOID CALLBACK RUNNER]]"<<std::endl;
						(*((std::function<void(void)>*)(e.user.data1)))();
						std::cout<<"[[EVENTARBITER VOID VOID CALLBACK RUNNER]] -- deleting functor"<<std::endl;
						delete ((std::function<void(void)>*)(e.user.data1));
						std::cout<<"[[EVENTARBITER VOID VOID CALLBACK RUNNER]] -- functor deleted"<<std::endl;
					}
				}
			}
			if (!handled && default_handler) {
				d("not handled, calling default handler");
				reaction = default_handler(&e);
				handled = true;
			}

			if (handled && translator(reaction)) {
				dpop();
				return;
			}
		}
	}

	bool EventArbiter::update_iterators(int newContext) {
		defKBindIter = kbinds.find(defaultContext);
		KCBMapMap::iterator tmpi = kbinds.find(newContext);
		if (tmpi != kbinds.end()) {
			currentContext = newContext;
			curKBindIter = tmpi;
			return true;
		}
		return false;
	}
	
}	
