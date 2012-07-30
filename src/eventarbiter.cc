#include "eventarbiter.hh"
#include "debug.hh"

namespace tGui{

	EventArbiter::EventArbiter() {
		dpush("EventArbiter::EventArbiter()");
		add_grab_context(0);
		set_grab_context(0);
		dpop();
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

	bool EventArbiter::grab_key(SDL_keysym s, Widget* w) {
		return grab_key(s, grabContext, w);
	}

	bool EventArbiter::grab_key(SDL_keysym s, int context, Widget* w) {
		KeysymCallback kscb = {
			.widgety = true,
			.u = {
				.w = w
			}
		};

		return grab_key(s, context, kscb);
	}

	bool EventArbiter::grab_key(SDL_keysym s, int (*func)(SDL_Event*, void*), void* data) {
		return grab_key(s, grabContext, func, data);
	}

	bool EventArbiter::grab_key(SDL_keysym s, int context, int (*func)(SDL_Event*, void*), void* data) {
		KeysymCallback kscb = {
			.widgety = false,
			.u = {
				.s = {
					.func = func,
					.data = data
				}
			}
		};

		return grab_key(s, context, kscb);
	}

	bool EventArbiter::grab_key(SDL_keysym s, int context, KeysymCallback kscb) {
		dpush("EventArbiter::grab_key (protected)");
		CBMapMap::iterator it = binds.find(context);

		if (it != binds.end()) {
			d("found context");
			std::pair<CBMap::iterator, bool> ret;
			ret = it->second.insert(std::pair<SDL_keysym, KeysymCallback>(s, kscb));
			dpop();
			return ret.second;
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
			int reaction = 0;
			switch (e.type) {
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					CBMap::iterator it = curBindIter->second.find(e.key.keysym);
					bool found = (it != curBindIter->second.end());
					if (!found) {
						d("did not find bind, searching defaults");
						it = defBindIter->second.find(e.key.keysym);
						found = (it != defBindIter->second.end());
					}
					if (found) {
						d("found bind");
						d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" grabbed");
						if(it->second.widgety) {
							d("is widgety");
							reaction = it->second.u.w->handle_event(&e);
						}
						else {
							d("is not widgety");
							reaction = (it->second.u.s.func)(&e, it->second.u.s.data);
						}
						break;
					}
					else {
						d("did not find bind");
						d("key "<<e.key.keysym.sym<<":"<<e.key.keysym.mod<<" not grabbed");
					}
				}
				default: {
					reaction = handle_event(&e);
					break;
				}
			}
			if (react_on_reaction(reaction)) {
				dpop();
				return;
			}
		}
	}

}	
