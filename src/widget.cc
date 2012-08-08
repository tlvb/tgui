#include "widget.hh"
#include "container.hh"
#include "debug.hh"

namespace tgui {
	Uint32 Widget::default_bg = 0;
	Uint32 Widget::default_fg = 0xff;
	TTF_Font *Widget::default_font = NULL;

	void Widget::set_defaults(Uint32 bg, Uint32 fg, TTF_Font *font) {
		default_bg = bg;
		default_fg = fg;
		default_font = font;
	}

	Widget::Widget(void) {
		bounds = Rect();
		shape = Shape();
		parent = NULL;
		canvas = NULL;
		theme = NULL;
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
