#include "container.hh"
#include "debug.hh"

namespace tgui {

	SContainer::SContainer(int hpadding, int vpadding) {
		pad[0] = hpadding;
		pad[1] = vpadding;
		child = ChildInfo(NULL);
	}

	bool SContainer::attach_child(Widget* c) {
		dpush("SContainer::attach_child()");
		if (child.w != NULL) {
			d("scontainer has child already");
			dpop();
			return false;
		}
		if (c->has_parent()) {
			d("child prospect has parent already");
			dpop();
			return false;
		}
		child = ChildInfo(c);
		c->set_parent(this);
		c->set_canvas(canvas);
		configure();
		return true;
	}

	Widget *SContainer::remove_child(void) {
		Widget *w = child.w;
		child.w = NULL;
		return w;
	}

	void SContainer::set_canvas(SDL_Surface *c) {
		canvas = c;
		if (child.w != NULL) {
			child.w->set_canvas(canvas);
		}
	}

	void SContainer::configure(void) {
		dpush("SContainer::configure()");
		if (child.w == NULL) {
			d("no child");
			dpop();
			return;
		}
		if (parent == NULL) {
			d("no parent");
			dpop();
			return;
		}
		child.shp = child.w->get_preferred_shape();
		shape = *child.shp;
		parent->configure();

		int csz[2] = {canvas->w-pad[0]*2, canvas->h-pad[1]*2};

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
				child.bounds.ix.pos[i] += pad[i];
			}
			else {
				child.bounds.ix.sz[i] = csz[i];
			}
			child.w->place(&child.bounds.nm, false);
			dpop();
		}
	}

	EventReaction SContainer::handle_event(SDL_Event *e) {
		dpush("Scontainer::handle_event()");
		if (child.w != NULL) {
			switch (e->type) {
				case SDL_MOUSEMOTION:
					dpop();
					return apply_mouseevent_to_child(&child, e->motion.x, e->motion.y, e);
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					dpop();
					return apply_mouseevent_to_child(&child, e->button.x, e->button.y, e);
					break;
				default:
					dpop();
					return child.w->handle_event(e);
			}
		}
		dpop();
		return 0;
	}

	void SContainer::draw(void) {
		dpush("SContainer::draw()");
		if (canvas != NULL && bounds.nm.w>0 && bounds.nm.h>0) {
			empty_rect(canvas, &bounds.nm, 0);
			if (child.w != NULL) {
				child.w->draw();
			}
		}
		dpop();
	}

}
