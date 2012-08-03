#include "event.hh"
#include "debug.hh"

namespace tgui{

	EventArbiter::EventArbiter() {
		dpush("EventArbiter::EventArbiter()");
		add_grab_context(0);
		set_grab_context(0);
		default_handler.id = 0;
		exclusivekeyboard.id = 0;
		exclusivemouse.id = 0;
		gid = 2;
		dpop();
	}

	bool EventArbiter::KeysymComparator::operator()(const SDL_keysym &a, const SDL_keysym &b) const {
		if (a.sym != b.sym)
			return a.sym<b.sym;
		return a.mod<b.mod;
	}

	void EventArbiter::set_reaction_translator(ReactionTranslator *t) {
		translator = t;
	}

	void EventArbiter::set_default_event_handler(EventConsumer *ec) {
		default_handler.id = 2; // internal use only, not correlated to the grab map
		default_handler.consumer.ec = ec;
	}

	void EventArbiter::set_default_event_handler(SDL_keysym s, EventReaction (*func)(SDL_Event*, void*), void* data) {
		default_handler.id = 3; // ditto
		default_handler.func.f = func;
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

	GrabID EventArbiter::grab_key(SDL_keysym s, EventConsumer *h) {
		return grab_key(s, grabContext, h);
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, int context, EventConsumer* h) {
		GrabCallback gcb = {
			.consumer = {
				.id = gid,
				.ec = h
			}	
		};
		return grab_key(s, context, gcb);
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, EventReaction (*func)(SDL_Event*, void*), void* data) {
		return grab_key(s, grabContext, func, data);
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, int context, EventReaction (*func)(SDL_Event*, void*), void* data) {
		GrabCallback gcb = {
			.func = {
				.id = gid+1,
				.f = func,
				.data = data
			}
		};

		return grab_key(s, context, gcb);
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


	void EventArbiter::grab_keyboard_exclusive(EventConsumer *ec) {
		exclusivekeyboard.id = 2; // ditto
		exclusivekeyboard.consumer.ec = ec;
	}

	void EventArbiter::grab_keyboard_exclusive(EventReaction (*func)(SDL_Event*, void*), void* data) {
		exclusivekeyboard.id = 3; // ditto
		exclusivekeyboard.func.f = func;
	}

	void EventArbiter::ungrab_keyboard_exclusive(void) {
		exclusivekeyboard.id = 0;
	}


	void EventArbiter::grab_mouse_exclusive(EventConsumer *ec) {
		exclusivemouse.id = 2; // ditto
		exclusivemouse.consumer.ec = ec;
	}

	void EventArbiter::grab_mouse_exclusive(EventReaction (*func)(SDL_Event*, void*), void* data) {
		exclusivemouse.id = 3; // ditto
		exclusivemouse.func.f = func;
	}

	void EventArbiter::ungrab_mouse_exclusive(void) {
		exclusivemouse.id = 0;
	}

	GrabID EventArbiter::grab_key(SDL_keysym s, int context, GrabCallback gcb) {
		dpush("EventArbiter::grab_key (protected)");
		CBMapMap::iterator it = binds.find(context);

		if (it != binds.end()) {
			d("found context");
			CBMap::iterator ret;
			ret = it->second.insert(std::pair<SDL_keysym, GrabCallback>(s, gcb));
			dpop();
			return gcb.id;
		}
		else {
			d("did not find context");
			dpop();
			return false;
		}
	}

	void EventArbiter::event_loop(void) {
		dpush("EventArbiter::event_loop()");
		SDL_Event e;
		for (;;) {
			SDL_WaitEvent(&e);
			EventReaction reaction = 0;
			bool handled = false;
			switch (e.type) {
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					if (exclusivekeyboard.id > 1) {
						reaction |= call_grab_callback(exclusivekeyboard, &e);
						handled = true;
					}
					else {
						std::pair<CBMap::iterator,CBMap::iterator> it = curBindIter->second.equal_range(e.key.keysym);
						bool found = (it.first != it.second);
						if (!found) {
							d("did not find bind, searching defaults");
							it = defBindIter->second.equal_range(e.key.keysym);
							found = (it.first != it.second);
						}
						if (found) {
							d("found bind");
							d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" grabbed");
							for (CBMap::iterator ju=it.first; ju!=it.second; ++ju) {
								reaction |= call_grab_callback(ju->second, &e);
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
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					if (exclusivemouse.id > 0) {
						reaction |= call_grab_callback(exclusivemouse, &e);
					}
					handled = true;
					break;
				}
			}
			if (!handled && default_handler.id > 1) {
				reaction = call_grab_callback(default_handler, &e);
				handled = true;
			}

			if (handled && translator->translate(reaction)) {
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
	
	EventReaction EventArbiter::call_grab_callback(GrabCallback gcb, SDL_Event *e) {
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

}	
