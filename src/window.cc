#include "window.hh"
#include "debug.hh"

namespace tGui {
	Window::Window(SDL_Surface *surf) : SContainer() {
		this->surf = surf;

	}

	void Window::configure(void) {
		dpush("Window::configure()");
		if (child == NULL) {
			dpop();
			return;
		}
		const ShapeRequest *sr = child->get_preferred_dimensions();
		SDL_Rect cb;
		if ((sr->maxw > 0) && (sr->maxw < surf->w)) {
			cb.w = sr->maxw;
		}
		else {
			cb.w = surf->w;
		}
		switch (sr->hGrav) {
			case LEFT:
				cb.x = 0;
				break;
			case HCENTER:
				cb.x = (surf->w-cb.w)/2;
				break;
			case RIGHT:
				cb.x = surf->w-cb.w;
				break;
		}
		if ((sr->maxh > 0) && (sr->maxh < surf->h)) {
			cb.h = sr->maxh;
		}
		else {
			cb.h = surf->h;

		}
		switch (sr->vGrav) {
			case TOP:
				cb.y = 0;
				break;
			case VCENTER:
				cb.y = (surf->h-cb.h)/2;
				break;
			case BOTTOM:
				cb.y = surf->h-cb.h;
				break;
		}
		child->set_surface(surf);
		child->place(&cb);
		dpop();
	}

	void Window::draw(void) {
		if (child != NULL) {
			child->draw();
			SDL_Flip(surf);
		}
	}

	bool Window::react_on_reaction(int reaction) {
		if (reaction > 0) {
			SDL_Flip(surf);
		}
		else if (reaction < 0) {
			return true;
		}
		return false;
	}

	int Window::handle_event(SDL_Event *e) {
		if (child != NULL) {
			return child->handle_event(e);
		}
		return 0;
	}

	
	bool Window::KeysymComparator::operator()(const SDL_keysym &a, const SDL_keysym &b) const {
		if (a.sym != b.sym)
			return a.sym<b.sym;
		return a.mod<b.mod;
	}




}
