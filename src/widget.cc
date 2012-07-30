#include "widget.hh"
#include "debug.hh"

namespace tGui {

	const ShapeRequest *Widget::get_preferred_dimensions(void) {
		return &prefDims;
	}

	int Widget::handle_event(SDL_Event *e) {
		return 0;
	}
	
	void Widget::set_parent(Container *pc) {
		parent = pc;
	}

	void Widget::place(SDL_Rect *b) {
		dpush("Widget::place");
		bounds = *b;
		d("("<<bounds.x<<";"<<bounds.y<<", "<<bounds.w<<"x"<<bounds.h<<")");
		dpop();
	}

	void Widget::set_surface(SDL_Surface *s) {
		dpush("Widget::set_surface()");
		surf = s;
		d("surface = "<<(void*)surf);
		dpop();
	}

}
