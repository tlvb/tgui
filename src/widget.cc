#include "widget.hh"
#include "container.hh"
#include "debug.hh"

namespace tgui {

	Widget::Widget(void) {
		bounds = Rect();
		shape = Shape();
		parent = NULL;
		canvas = NULL;
	}

	void Widget::set_parent(Container *pc) {
		parent = pc;
	}

	void Widget::set_canvas(SDL_Surface *c) {
		canvas = c;
	}

	bool Widget::has_parent(void) const {
		return parent != NULL;
	}

	void Widget::set_preferred_shape(Shape *sh, bool doConfigure) {
		shape = *sh;
		if (doConfigure && (parent != NULL)) {
			parent->configure();
		}
	}

	const Shape *Widget::get_preferred_shape(void) const {
		return &shape;
	}

	void Widget::place(SDL_Rect *b, bool doDraw) {
		dpush("Widget::place()");
		bounds = *b;
		if (doDraw && (canvas != NULL)) {
			draw();
		}
		dpop();
	}

}
