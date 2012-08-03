#include "examplewidget.hh"
#include "container.hh"
#include "event.hh"
#include "debug.hh"

namespace tgui {

	ExampleWidget::ExampleWidget(void) {
		shape.nm = {0, 0, 0, 0, 0, 0};
		bg = 0;
		fg = 0xffffffff;
		hasMouse = false;
	}

	ExampleWidget::ExampleWidget(Uint16 minw, Uint16 minh,
			Uint16 maxw, Uint16 maxh,
			Uint8 hgrav, Uint8 vgrav, Uint32 bg, Uint32 fg) {

		shape.nm = {minw, minh, maxw, maxh, hgrav, vgrav};
		this->bg = bg;
		this->fg = fg;
		hasMouse = false;

	}

	ExampleWidget::ExampleWidget(Uint32 bg, Uint32 fg) {
		shape.nm = {0, 0, 0, 0, 0, 0};
		this->bg = bg;
		this->fg = fg;
		hasMouse = false;
	}


	void ExampleWidget::draw() {
		dpush("ExampleWidget::draw()");
		if ((canvas != NULL) && (bounds.nm.w > 0) && (bounds.nm.h > 0)) {
			fill_rect(canvas, &bounds.nm, bg);
			empty_rect(canvas, &bounds.nm, fg);
			if (hasMouse) {
				int x,y;
				SDL_GetMouseState(&x, &y);
				if ((x>=bounds.nm.x) && (x<bounds.nm.x+bounds.nm.w) && (y>=bounds.nm.y) && (y<bounds.nm.y+bounds.nm.h)) {
					horizontal_line(canvas, bounds.nm.x, y, bounds.nm.x+bounds.nm.w-1, fg);
					vertical_line(canvas, x, bounds.nm.y, bounds.nm.y+bounds.nm.h-1, fg);
				}
			}

//			bersenham_line(canvas, bounds.nm.x, bounds.nm.y, bounds.nm.x+bounds.nm.w-1, bounds.nm.y+bounds.nm.h-1, fg);
//			bersenham_line(canvas, bounds.nm.x, bounds.nm.y+bounds.nm.h-1, bounds.nm.x+bounds.nm.w-1, bounds.nm.y, fg);
		}
		d("<10>");
		dpop();
		d("<11>");
	}


	EventReaction ExampleWidget::handle_event(SDL_Event *e) {
		dpush("ExampleWidget::handle_event()");
		switch (e->type) {
			case Container::TGUI_MOUSEENTER:
				d("mouseenter");
				hasMouse = true;
				draw();
				dpop();
				return UPDATE_SCREEN;
				break;
			case Container::TGUI_MOUSEEXIT:
				d("mouseexit");
				hasMouse = false;
				draw();
				dpop();
				return UPDATE_SCREEN;
				break;
			case SDL_MOUSEMOTION:
				draw();
				dpop();
				return UPDATE_SCREEN;
				break;
		}
		dpop();
		return 0;
	}







}
