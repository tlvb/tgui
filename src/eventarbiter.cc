#include "event.hh"
#include "debug.hh"

namespace tgui{

	EventArbiter::EventArbiter() {
		dpush("EventArbiter::EventArbiter()");
		add_grab_context(0);
		set_grab_context(0);
		translator = ReactionTranslator();
		default_handler = EventCallback();
		exclusivekeyboard = EventCallback();
		exclusivemouse = EventCallback();
		gid = invalidGrabID + 1;
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

	bool EventArbiter::add_grab_context(int context) {
		dpush("EventArbiter::add_grab_context("<<context<<")");
		std::pair<CBMapMap::iterator, bool> ret;
		ret = binds.insert(std::pair<int, CBMap>(context, CBMap()));
		d("insert returned "<<ret.second);
		update_iterators(grabContext);
		dpop();
		return ret.second;
	}

	bool EventArbiter::set_grab_context(int context) {
		return update_iterators(context);
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, EventCallback ec) {
		return grab_key(s, grabContext, ec);
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, int context, EventCallback ec) {
		KeyboardCallback kcb = {
			.id = gid++,
			.cb = ec
		};
		return grab_key(s, context, kcb);
	}

	bool EventArbiter::ungrab_key(GrabID id) {
		for(CBMapMap::iterator outer=binds.begin(); outer!=binds.end(); ++outer) {
			for (CBMap::iterator inner=outer->second.begin(); inner!=outer->second.end(); ++inner) {
				if (inner->second.id == id) {
					outer->second.erase(inner);
					update_iterators(grabContext);
					return true;
				}
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

	GrabID EventArbiter::grab_key(SDL_keysym s, int context, KeyboardCallback kcb) {
		dpush("EventArbiter::grab_key (protected)");
		CBMapMap::iterator it = binds.find(context);

		if (it != binds.end()) {
			d("found context");
			CBMap::iterator ret;
			ret = it->second.insert(std::pair<SDL_keysym, KeyboardCallback>(s, kcb));
			dpop();
			return kcb.id;
		}
		else {
			d("did not find context");
			dpop();
			return invalidGrabID;
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
						std::pair<CBMap::iterator,CBMap::iterator> it = curBindIter->second.equal_range(e.key.keysym);
						bool found = (it.first != it.second);
						if (!found) {
//							d("did not find bind, searching defaults");
							it = defBindIter->second.equal_range(e.key.keysym);
							found = (it.first != it.second);
						}
						if (found) {
							d("found bind");
							d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" grabbed");
							for (CBMap::iterator ju=it.first; ju!=it.second; ++ju) {
								reaction |= ju->second.cb(&e);
							}
							handled = true;
						}
						else {
//							d("did not find bind");
//							d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" not grabbed");
						}
					}
					break;
				}
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
//					d("something mousey happened");
					if (exclusivemouse) {
						d("exclusive lock")
						reaction |= exclusivemouse(&e);
						handled = true;
					}
					break;
				}
			}
			if (!handled && default_handler) {
//				d("not handled, calling default handler");
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
		defBindIter = binds.find(0);
		CBMapMap::iterator tmpi = binds.find(newContext);
		if (tmpi != binds.end()) {
			grabContext = newContext;
			curBindIter = tmpi;
			return true;
		}
		return false;
	}
	
	/*
	EventReaction EventArbiter::call_grab_callback(KeyboardCallback gcb, SDL_Event *e) {
		dpush("EventArbiter::call_grab_callback");
		EventReaction reaction;
		if((gcb.id & 1) == 0) {
			d("is a consumer object");
			reaction = gcb.consumer.ec->handle_event(e);
		}
		else {
			d("is a function pointer");
			reaction = gcb.func.f(e, gcb.func.data);
		}
		dpop();
		return reaction;
	}
	*/

}	
