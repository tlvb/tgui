#include "container.hh"
#include "debug.hh"

namespace tgui {

	MContainer::MContainer(bool vertical, int hpadding, int vpadding, int chspacing) {
		if (vertical) {
			pri = 1;
			sec = 0;
		}
		else {
			pri = 0;
			sec = 1;
		}
		pad[0] = hpadding;
		pad[1] = vpadding;
		chsp = chspacing;
	}

	bool MContainer::append_child(Widget *c, bool doConfigure) {
		dpush("MContainer::append_child()");
		if (c->has_parent()) {
			d("child prospect has parent already");
			dpop();
			return false;
		}
		children.push_back(ChildInfo(c));
		c->set_parent(this);
		c->set_canvas(canvas);
		if (doConfigure)
			configure();
		dpop();
		return true;
	}

	bool MContainer::insert_child(Widget *c, unsigned int pos, bool doConfigure) {
		dpush("MContainer::insert_child()");
		if (c->has_parent()) {
			d("child prospect has parent already");
			dpop();
			return false;
		}
		children.insert(children.begin()+pos, ChildInfo(c));
		c->set_parent(this);
		c->set_canvas(canvas);
		if (doConfigure)
			configure();
		dpop();
		return true;
	}

	Widget *MContainer::remove_child(int pos) {
		dpush("MContainer::remove_child()");
		ChildIterator cit = children.begin()+pos;
		Widget *w = NULL;
		if (cit < children.end()) {
			w = cit->w;
			children.erase(cit);
			w->set_parent(NULL);
			w->set_canvas(NULL);
		}
		else {
			d("no child with index" << pos);
		}
		dpop();
		return w;
	};

	void MContainer::configure() {
		dpush("MContainer::configure()");

		int nch = children.size();
		
		if (nch == 0) {
			d("no children");
			dpop();
			return;
		}
		if (parent == NULL) {
			d("no parent");
			dpop();
			return;
		}

		ChildIterator cit;
		shape.ix.min[sec] = 0;
		shape.ix.max[sec] = 0;
		shape.ix.min[pri] = 0;
		shape.ix.max[pri] = 0;

		int i = 0;
		for (cit = children.begin(); cit<children.end(); ++cit) {
			cit->shp = cit->w->get_preferred_shape();
			d("child has minimum requirements of "<<cit->shp->nm.minw<<"x"<<cit->shp->nm.minh);
			cit->bounds.ix.pos[0] = 0;
			cit->bounds.ix.pos[1] = 0;
			cit->bounds.ix.sz[pri] = 0;
			cit->bounds.ix.sz[sec] = cit->shp->ix.max[sec];
			if (shape.ix.min[sec] < cit->shp->ix.min[sec])
				shape.ix.min[sec] = cit->shp->ix.min[sec];
			shape.ix.min[pri] += cit->shp->ix.min[pri];
			d("yielding child space requirements of "<<shape.nm.minw<<"x"<<shape.nm.minh);
			++i;
		}

		shape.ix.min[sec] += 2*pad[sec];
		shape.ix.min[pri] += 2*pad[pri] + ((nch>1)?nch-1:0)*chsp;


		d("requiring totally "<<shape.nm.minw<<"x"<<shape.nm.minh<<" for whole container");

		d("requesting configuration from parent");
		d("parent is "<<(void*)parent);
		parent->configure();
		d("returned therefrom");

		d("we got "<<bounds.nm.w<<"x"<<bounds.nm.h);

		int availpri = bounds.ix.sz[pri] - 2*pad[pri] - ((nch>1)?nch-1:0)*chsp;
		int availsec = bounds.ix.sz[sec] - 2*pad[sec];

		d("of which "<<availpri<<" ~ "<<availsec<<" are available to children");

		//
		// set secondary dimension size and position for children
		//
		for (cit = children.begin(); cit<children.end(); ++cit) {
			dpush("setting secondaries for child #"<<i);
			cit->bounds.ix.pos[sec] = bounds.ix.pos[sec] + pad[sec];
			d("sec pos is "<<cit->bounds.ix.pos[sec]);
			if ((cit->shp->ix.max[sec] == 0) || (cit->bounds.ix.sz[sec] > availsec)) {
				d("want("<<cit->shp->ix.max[sec]<<") >= avail("<<availsec<<") - set to available");
				cit->bounds.ix.sz[sec] = availsec;
			}
			else {
				d("want("<<cit->shp->ix.max[sec]<<") < avail("<<availsec<<") - set to wanted");
				if (cit->shp->ix.grav[sec] == 1) {
					d("child is centered, adding half of extra space to x: "<<(cit->bounds.ix.sz[sec]-availsec)/2);
					cit->bounds.ix.pos[sec] += (cit->bounds.ix.sz[sec] - availsec)/2;
				}
				else if (cit->shp->ix.grav[sec] == 2) {
					d("child is right justified, adding extra space to x: "<<(cit->bounds.ix.sz[sec]-availsec));
					cit->bounds.ix.pos[sec] += cit->bounds.ix.sz[sec] - availsec;
				}
			}
			dpop();
		}

	
		//
		// set primary dimension size for children
		//
		int nactive = nch;
		int distrpri = availpri;
		
		// set minheight
		for (cit=children.begin(); cit<children.end(); ++cit) {
			cit->bounds.ix.sz[pri] = distrpri < cit->shp->ix.min[pri]? distrpri : cit->shp->ix.min[pri];
			d("child #"<<i<<" is initially given primary dimension size "<<cit->bounds.ix.sz[pri]<<" ("<<cit->shp->ix.min[pri]<<") required")
			distrpri -= cit->bounds.ix.sz[pri];
		}
		d(distrpri<<" remains to be distributed evenly");

		cit = children.begin();
		d("a current cit->bounds = "<<cit->bounds.ix.pos[sec]<<":"<<cit->bounds.ix.pos[pri]<<" "<<cit->bounds.ix.sz[sec]<<"x"<<cit->bounds.ix.sz[pri]);
		// distribute any primary dimension size still available and wanted
		while ((nactive > 0) && (distrpri > 0)) {
			int splitpri = (distrpri+1)/(nactive);
			if (splitpri == 0)
				splitpri = 1;
			dpush("distributing "<<distrpri<<" to "<<nactive<<" children");
			nactive = 0;
			for (cit=children.begin(); cit<children.end(); ++cit) {
				int assignpri = distrpri<splitpri?distrpri:splitpri;
				dpush(assignpri<<" is available");
				if (cit->bounds.ix.pos[pri] == 0) {
					if ((cit->shp->ix.max[pri] == 0) || (cit->shp->ix.max[pri] - cit->bounds.ix.sz[pri] > assignpri)) {
						d("taking all");
						cit->bounds.ix.sz[pri] += assignpri;
						distrpri -= assignpri;
						++nactive;
					}
					else {
						d("content with taking "<<(cit->shp->ix.max[pri]-cit->bounds.ix.sz[pri]));
						d((assignpri-(cit->shp->ix.max[pri] - cit->bounds.ix.sz[pri]))<<" is returned");
						d("will not take anything in future iterations");
						cit->bounds.ix.pos[pri] = 1;
						distrpri -= (cit->shp->ix.max[pri] - cit->bounds.ix.sz[pri]);
						cit->bounds.ix.sz[pri] = cit->shp->ix.max[pri];
					}
				}
				dpop();
			}
			dpop();
		}
		
		cit = children.begin();
		d("b current cit->bounds = "<<cit->bounds.ix.pos[sec]<<":"<<cit->bounds.ix.pos[pri]<<" "<<cit->bounds.ix.sz[sec]<<"x"<<cit->bounds.ix.sz[pri]);
		//
		// set primary dimension position for children
		//
		int v = bounds.ix.pos[pri] + pad[pri];
		Uint8 last = 0;
		for (cit=children.begin(); cit<children.end(); ++cit) {
			dpush("placing child #"<<i<<" with "<<v<<" as starting point");
			int dv = 0;
			if (distrpri > 0) {
				if (last == 0 && cit->shp->ix.grav[pri] == 1) {
					d("adding some space");
					dv = (distrpri+1)/2;
					distrpri -= dv;
					last = cit->shp->ix.grav[pri];
				}
				else if (last < cit->shp->ix.grav[pri]) {
					d("adding all remaining space");
					dv = distrpri;
					distrpri = 0;
				}
			}
			cit->bounds.ix.pos[pri] = v+dv;
			v = cit->bounds.ix.pos[pri] + cit->bounds.ix.sz[pri] + chsp;
			cit->w->place(&(cit->bounds.nm));
			dpop();
		}
		dpop();
	}

	EventReaction MContainer::handle_event(SDL_Event *e) {
		ChildIterator it;
		EventReaction ret;
		switch (e->type) {
			case SDL_MOUSEMOTION:
				for (ChildIterator it=children.begin(); it<children.end(); ++it) {
					ret |= apply_mouseevent_to_child(&(*it), e->motion.x, e->motion.y, e);
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				for (ChildIterator it=children.begin(); it<children.end(); ++it) {
					ret |= apply_mouseevent_to_child(&(*it), e->button.x, e->button.y, e);
				}
				break;
		}
		return ret;
	}

	void MContainer::set_canvas(SDL_Surface *c) {
		canvas = c;
		for (ChildIterator it=children.begin(); it<children.end(); ++it) {
			it->w->set_canvas(c);
		}
	}

	void MContainer::draw() {
		dpush("MContainer::draw()");
		if (canvas != NULL) {
			empty_rect(canvas, &bounds.nm, 0);
			ChildIterator cit;
			d("drawing children");
			for (cit = children.begin(); cit < children.end(); ++cit) {
				cit->w->draw();
			}
		}
		dpop();
	}

}
