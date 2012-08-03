#include "examplewidget.hh"
#include "debug.hh"

namespace tgui {

	ExampleWidget::ExampleWidget(void) {
		shape.nm = {0, 0, 0, 0, 0, 0};
		bg = 0;
		fg = 0xffffffff;
	}

	ExampleWidget::ExampleWidget(Uint16 minw, Uint16 minh,
			Uint16 maxw, Uint16 maxh,
			Uint8 hgrav, Uint8 vgrav, Uint32 bg, Uint32 fg) {

		shape.nm = {minw, minh, maxw, maxh, hgrav, vgrav};
		this->bg = bg;
		this->fg = fg;

	}

	void ExampleWidget::draw() {
		dpush("ExampleWidget::draw()");
		if ((canvas != NULL) && (bounds.nm.w > 0) && (bounds.nm.h > 0)) {
//			fill_rect(canvas, &bounds.nm, bg);
			empty_rect(canvas, &bounds.nm, fg);
//			bersenham_line(canvas, bounds.nm.x, bounds.nm.y, bounds.nm.x+bounds.nm.w-1, bounds.nm.y+bounds.nm.h-1, fg);
//			bersenham_line(canvas, bounds.nm.x, bounds.nm.y+bounds.nm.h-1, bounds.nm.x+bounds.nm.w-1, bounds.nm.y, fg);
		}
		dpop();
	}


	EventReaction ExampleWidget::handle_event(SDL_Event *e) {
		return 0;
	}







}
