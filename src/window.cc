#include "window.hh"
#include "debug.hh"

namespace tgui {
	Window::Window(SDL_Surface *c) : SContainer(0, 0), EventArbiter() {
		canvas = c;
		bounds.nm.x = 0;
		bounds.nm.y = 0;
		bounds.nm.w = c->w;
		bounds.nm.h = c->h;
		set_reaction_translator(this);
		set_default_event_handler(this);
	}


	bool Window::translate(EventReaction r) {
		dpush("Window::translate r="<<r);
		if ((r & UPDATE_SCREEN) != 0) {
			d("FLIP");
			SDL_Flip(canvas);
		}
		if ((r & QUIT) != 0) {
			d("QUIT");
			dpop();
			return true;
		}
		dpop();
		return false;
	}

	void Window::configure(void) {
		dpush("Window::configure()");
		if (child.w == NULL) {
			d("no child");
			dpop();
			return;
		}
		child.shp = child.w->get_preferred_shape();

		int csz[2] = {canvas->w, canvas->h};

		for (int i=0; i<2; ++i) {
			if ((child.shp->ix.max[i] > 0) && (child.shp->ix.max[i] < csz[i])) {
				child.bounds.ix.sz[i] = child.shp->ix.max[i];
				switch (child.shp->ix.grav[i]) {
					case 0:
						child.bounds.ix.pos[i] = 0;
						break;
					case 1:
						child.bounds.ix.pos[i] = (csz[i]-child.bounds.ix.sz[i])/2;
						break;
					case 2:
						child.bounds.ix.pos[i] = csz[i]-child.bounds.ix.sz[i];
						break;
				}
			}
			else {
				child.bounds.ix.sz[i] = csz[i];
			}
		}
		child.w->place(&child.bounds.nm, false);
		dpop();
	}



}
