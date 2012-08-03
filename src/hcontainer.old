#include "hcontainer.hh"
#include "debug.hh"

namespace tGui {

	HContainer::HContainer(int hpadding, int vpadding, int hspacing) {
		hp = hpadding;
		vp = vpadding;
		hs = hspacing;
		surf = NULL;
		parent = NULL;
	}

	bool HContainer::insert_child(Widget *c, unsigned int pos, bool doConfigure) {
		c->set_surface(surf);
		MContainer::insert_child(c, pos, doConfigure);
		return true;
	}

	bool HContainer::append_child(Widget *c, bool doConfigure) {
		c->set_surface(surf);
		MContainer::append_child(c, doConfigure);
		return true;
	}

	void HContainer::configure() {
		dpush("HContainer::configure()");

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
				.w = 0, .h = csrs[i]->maxh
			};
			if (prefDims.minh < csrs[i]->minh)
				prefDims.minh = csrs[i]->minh;
			prefDims.minw += csrs[i]->minw;
			d("yielding child space requirements of "<<prefDims.minw<<"x"<<prefDims.minh);
			++i;
		}

		prefDims.minh += vp*2;
		prefDims.minw += 2*hp + ((nch>1)?nch-1:0)*hs;


		d("requiring totally "<<prefDims.minw<<"x"<<prefDims.minh<<" for whole container");

		d("requesting configuration from parent");
		d("parent is "<<(void*)parent);
		parent->configure();
		d("returned therefrom");

		d("we got "<<bounds.w<<"x"<<bounds.h);

		int availh = bounds.h - 2*vp;
		int availw = bounds.w - 2*hp - ((nch>1)?nch-1:0)*hs;

		//
		// set x and width for children
		//
		for (i=0; i < nch; ++i) {
			dpush("setting y+h for child #"<<i);
			candidates[i].y = bounds.y + vp;
			if ((csrs[i]->maxh == 0) || (candidates[i].h > availh)) {
				d("want("<<csrs[i]->maxh<<") >= avail("<<availh<<") - set to available");
				candidates[i].h = availh;
			}
			else {
				d("want("<<csrs[i]->maxh<<") < avail("<<availh<<") - set to wanted");
				if (csrs[i]->vGrav == VCENTER) {
					d("child is centered, adding half of extra space to y: "<<(candidates[i].h-availh)/2);
					candidates[i].y += (candidates[i].h - availh)/2;
				}
				else if (csrs[i]->vGrav == BOTTOM) {
					d("child is bottom justified, adding extra space to y: "<<(candidates[i].y-availh));
					candidates[i].y += candidates[i].h - availh;
				}
			}
			dpop();
		}
	
		//
		// set width for children
		//
		int nactive = nch;
		int distrw = availw;
		
		// set minwidth
		for (i=0; i<nch; ++i) {
			candidates[i].w = distrw < csrs[i]->minw? distrw : csrs[i]->minw;
			d("child #"<<i<<" is initially given width "<<candidates[i].w<<" ("<<csrs[i]->minw<<") required")
			distrw -= candidates[i].w;
		}
		d(distrw<<" remains to be distributed evenly");

		// distribute any width still available and wanted
		while ((nactive > 0) && (distrw > 0)) {
			int splitw = (distrw+1)/(nactive);
			if (splitw == 0)
				splitw = 1;
			dpush("distributing "<<distrw<<" to "<<nactive<<" children");
			nactive = 0;
			for (i=0; i<nch; ++i) {
				int assignw = distrw<splitw?distrw:splitw;
				dpush(assignw<<" is available");
				if (candidates[i].x == 0) {
					if ((csrs[i]->maxw == 0) || (csrs[i]->maxw - candidates[i].w > assignw)) {
						d("taking all");
						candidates[i].w += assignw;
						distrw -= assignw;
						++nactive;
					}
					else {
						d("content with taking "<<(csrs[i]->maxw-candidates[i].w));
						d((assignw-(csrs[i]->maxw - candidates[i].w))<<" is returned");
						d("will not take anything in future iterations");
						candidates[i].x = 1;
						distrw -= (csrs[i]->maxw - candidates[i].w);
						candidates[i].w = csrs[i]->maxw;
					}
				}
				dpop();
			}
			dpop();
		}
		
		//
		// set x for children
		//
		int x = bounds.x + hp;
		HGravity last = LEFT;
		for (i=0; i<nch; ++i) {
			dpush("placing child #"<<i<<" with "<<x<<" as starting point");
			int dx = 0;
			if (distrw > 0) {
				if (last == LEFT && csrs[i]->hGrav == HCENTER) {
					d("adding some space");
					dx = (distrw+1)/2;
					distrw -= dx;
					last = csrs[i]->hGrav;
				}
				else if (last < csrs[i]->hGrav) {
					d("adding all remaining space");
					dx = distrw;
					distrw = 0;
				}
			}
			candidates[i].x = x+dx;
			x = candidates[i].x + candidates[i].w + hs;
			dpop();
		}
		
		i = 0;
		for (cit = children.begin(); cit < children.end(); ++cit) {
			(*cit)->place(&candidates[i++]);
		}

		delete[] csrs;
		delete[] candidates;
	}

	void HContainer::draw() {
		dpush("HContainer::draw()");
		std::vector<Widget*>::iterator cit;
		ColorTheme *ct = get_color_theme();
		fill_rect(surf, &bounds, ct->passiveBackground[focs]);
		d("drawing children");
		for (cit = children.begin(); cit < children.end(); ++cit) {
			(*cit)->draw();
		}
		dpop();
	}

	int HContainer::handle_event(SDL_Event *e) {
		return 0;
	}


}

