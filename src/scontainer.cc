#include "container.hh"
#include "debug.hh"

namespace tgui {

	SContainer::SContainer(void) {
		pad[0] = 5;
		pad[1] = 5;
		child = ChildInfo(NULL);
	}

	void SContainer::set_padding(int hpadding, int vpadding) {
		pad[0] = hpadding;
		pad[1] = vpadding;
		if (parent != NULL) {
			parent->configure();
		}
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
		if (canvas == NULL) {
			d("null canvas");
		}
		child = ChildInfo(c);
		c->set_parent(this);
		c->set_canvas(canvas);
		configure();
		dpop();
		return true;
	}

	Widget *SContainer::remove_child(void) {
		Widget *w = child.w;
		child.w = NULL;
		return w;
	}

	void SContainer::set_canvas(SDL_Surface *c) {
		dpush("SContainer::set_canvas()");
		canvas = c;
		if (child.w != NULL) {
			child.w->set_canvas(canvas);
		}
		dpop();
	}

	void SContainer::configure(void) {
		dpush("SContainer::configure()");
		if (child.w == NULL) {
			d("no child");
			dpop();
			return;
		}
		child.shp = child.w->get_preferred_shape();
		shape = *child.shp;
		shape.nm.minw += pad[0]*2;
		shape.nm.minh += pad[1]*2;
		shape.nm.maxw += pad[0]*2;
		shape.nm.maxh += pad[1]*2;
		if (parent == NULL) {
			d("no parent");
			dpop();
			return;
		}
		parent->configure();
		dpop();
	}

	EventReaction SContainer::handle_event(SDL_Event *e) {
		if (child.w != NULL) {
			switch (e->type) {
				case SDL_MOUSEMOTION:
					return apply_mouseevent_to_child(&child, e->motion.x, e->motion.y, e->motion.state, e);
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					return apply_mouseevent_to_child(&child, e->button.x, e->button.y, SDL_BUTTON(e->button.button), e);
					break;
				default:
					return child.w->handle_event(e);
			}
		}
		return 0;
	}

	void SContainer::draw(void) {
		dpush("SContainer::draw()");
		if (canvas != NULL && bounds.nm.w>0 && bounds.nm.h>0) {
			if (theme != NULL) {
#if 0
				fill_rect(canvas, &bounds.nm, th->nc);
				empty_rect(canvas, &bounds.nm, th->bg);
#endif
			}
			else {
				fill_rect(canvas, &bounds.nm, default_bg);
				empty_rect(canvas, &bounds.nm, default_fg);
			}
			if (child.w != NULL) {
				child.w->draw();
			}
		}
		dpop();
	}
	
	void SContainer::place(SDL_Rect *b, bool doDraw) {
		dpush("SContainer::place()");
		Widget::place(b, false);
		d(bounds.nm.x<<";"<<bounds.nm.y<<" "<<bounds.nm.w<<"x"<<bounds.nm.h);

		int csz[2] = {bounds.nm.w-pad[0]*2, bounds.nm.h-pad[1]*2};

		for (int i=0; i<2; ++i) {
			dpush("dimension "<<(i==0?'x':'y'));
			d("available: "<<csz[i]);
			if ((child.shp->ix.grav[i] != expand) && (child.shp->ix.max[i] < csz[i])) {
				child.bounds.ix.sz[i] = child.shp->ix.max[i];
				switch (child.shp->ix.grav[i]) {
					case 0: // left/top
						child.bounds.ix.pos[i] = 0;
						break;
					case 1: // center
						child.bounds.ix.pos[i] = (csz[i]-child.bounds.ix.sz[i])/2;
						break;
					case 2:
						child.bounds.ix.pos[i] = csz[i]-child.bounds.ix.sz[i];
						d("left/top "<<child.bounds.ix.pos[i]);
						break;
				}
			}
			else {
				child.bounds.ix.pos[i] = 0;
				child.bounds.ix.sz[i] = csz[i];
				d("eternally growing "<<child.bounds.ix.sz[i]);
			}
			d(child.bounds.ix.pos[i]);
			child.bounds.ix.pos[i] += bounds.ix.pos[i] + pad[i];
			d(child.bounds.ix.pos[i] << " = " << bounds.ix.pos[i] << " + " << pad[i]);
			dpop();
		}
		child.w->place(&child.bounds.nm, false);
		if (doDraw)
			draw();
		dpop();
	}


}
