#include "vcontainer.hh"
#include "debug.hh"

namespace tGui {

	VContainer::VContainer(int hpadding, int vpadding, int vspacing) {
		hp = hpadding;
		vp = vpadding;
		vs = vspacing;
		surf = NULL;
		parent = NULL;
	}

	bool VContainer::insert_child(Widget *c, unsigned int pos, bool doConfigure) {
		c->set_surface(surf);
		MContainer::insert_child(c, pos, doConfigure);
		return true;
	}

	bool VContainer::append_child(Widget *c, bool doConfigure) {
		c->set_surface(surf);
		MContainer::append_child(c, doConfigure);
		return true;
	}

	void VContainer::configure() {
		dpush("VContainer::configure()");

		std::vector<Widget*>::iterator cit;
		prefDims.minw = 0;
		prefDims.maxw = 0;
		prefDims.minh = 0;
		prefDims.maxh = 0;


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
		
		const ShapeRequest **csrs = new const ShapeRequest*[nch];
		SDL_Rect *candidates = new SDL_Rect[nch];

		int i = 0;
		for (cit = children.begin(); cit < children.end(); ++cit) {
			csrs[i] = (*cit)->get_preferred_dimensions();
			d("child has minimum requirements of "<<csrs[i]->minw<<"x"<<csrs[i]->minh);
			candidates[i] = {
				.x = 0, .y = 0,
				.w = csrs[i]->maxw, .h = 0
			};
			if (prefDims.minw < csrs[i]->minw)
				prefDims.minw = csrs[i]->minw;
			prefDims.minh += csrs[i]->minh;
			d("yielding child space requirements of "<<prefDims.minw<<"x"<<prefDims.minh);
			++i;
		}

		prefDims.minw += hp*2;
		prefDims.minh += 2*vp + ((nch>1)?nch-1:0)*vs;


		d("requiring totally "<<prefDims.minw<<"x"<<prefDims.minh<<" for whole container");

		d("requesting configuration from parent");
		d("parent is "<<(void*)parent);
		parent->configure();
		d("returned therefrom");

		d("we got "<<bounds.w<<"x"<<bounds.h);

		int availh = bounds.h - 2*vp - ((nch>1)?nch-1:0)*vs;
		int availw = bounds.w - 2*hp;

		//
		// set x and width for children
		//
		for (i=0; i < nch; ++i) {
			dpush("setting x+w for child #"<<i);
			candidates[i].x = bounds.x + hp;
			if ((csrs[i]->maxw == 0) || (candidates[i].w > availw)) {
				d("want("<<csrs[i]->maxw<<") >= avail("<<availw<<") - set to available");
				candidates[i].w = availw;
			}
			else {
				d("want("<<csrs[i]->maxw<<") < avail("<<availw<<") - set to wanted");
				if (csrs[i]->hGrav == HCENTER) {
					d("child is centered, adding half of extra space to x: "<<(candidates[i].w-availw)/2);
					candidates[i].x += (candidates[i].w - availw)/2;
				}
				else if (csrs[i]->hGrav == RIGHT) {
					d("child is right justified, adding extra space to x: "<<(candidates[i].w-availw));
					candidates[i].x += candidates[i].w - availw;
				}
			}
			dpop();
		}
	
		//
		// set height for children
		//
		int nactive = nch;
		int distrh = availh;
		
		// set minheight
		for (i=0; i<nch; ++i) {
			candidates[i].h = distrh < csrs[i]->minh? distrh : csrs[i]->minh;
			d("child #"<<i<<" is initially given height "<<candidates[i].h<<" ("<<csrs[i]->minh<<") required")
			distrh -= candidates[i].h;
		}
		d(distrh<<" remains to be distributed evenly");

		// distribute any height still available and wanted
		while ((nactive > 0) && (distrh > 0)) {
			int splith = (distrh+1)/(nactive);
			if (splith == 0)
				splith = 1;
			dpush("distributing "<<distrh<<" to "<<nactive<<" children");
			nactive = 0;
			for (i=0; i<nch; ++i) {
				int assignh = distrh<splith?distrh:splith;
				dpush(assignh<<" is available");
				if (candidates[i].y == 0) {
					if ((csrs[i]->maxh == 0) || (csrs[i]->maxh - candidates[i].h > assignh)) {
						d("taking all");
						candidates[i].h += assignh;
						distrh -= assignh;
						++nactive;
					}
					else {
						d("content with taking "<<(csrs[i]->maxh-candidates[i].h));
						d((assignh-(csrs[i]->maxh - candidates[i].h))<<" is returned");
						d("will not take anything in future iterations");
						candidates[i].y = 1;
						distrh -= (csrs[i]->maxh - candidates[i].h);
						candidates[i].h = csrs[i]->maxh;
					}
				}
				dpop();
			}
			dpop();
		}
		
		//
		// set y for children
		//
		int y = bounds.y + vp;
		VGravity last = TOP;
		for (i=0; i<nch; ++i) {
			dpush("placing child #"<<i<<" with "<<y<<" as starting point");
			int dy = 0;
			if (distrh > 0) {
				if (last == TOP && csrs[i]->vGrav == VCENTER) {
					d("adding some space");
					dy = (distrh+1)/2;
					distrh -= dy;
					last = csrs[i]->vGrav;
				}
				else if (last < csrs[i]->vGrav) {
					d("adding all remaining space");
					dy = distrh;
					distrh = 0;
				}
			}
			candidates[i].y = y+dy;
			y = candidates[i].y + candidates[i].h + vs;
			dpop();
		}
		
		i = 0;
		for (cit = children.begin(); cit < children.end(); ++cit) {
			(*cit)->place(&candidates[i++]);
		}

		delete[] csrs;
		delete[] candidates;
	}

	void VContainer::draw() {
		dpush("VContainer::draw()");
		std::vector<Widget*>::iterator cit;
		ColorTheme *ct = get_color_theme();
		fill_rect(surf, &bounds, ct->passiveBackground[focs]);
		d("drawing children");
		for (cit = children.begin(); cit < children.end(); ++cit) {
			(*cit)->draw();
		}
		dpop();
	}

	int VContainer::handle_event(SDL_Event *e) {
		return 0;
	}


}
