#include "examplewidget.hh"
#include "container.hh"
#include "event.hh"
#include "debug.hh"

namespace tgui {

	ExampleWidget::ExampleWidget(void) {
		shape.nm = {0, 0, 1000, 1000, expand, expand};
		hasMouse = false;
	}

	ExampleWidget::ExampleWidget(Uint16 minw, Uint16 minh,
			Uint16 maxw, Uint16 maxh,
			Uint8 hgrav, Uint8 vgrav) {

		shape.nm = {minw, minh, maxw, maxh, hgrav, vgrav};
		hasMouse = false;

	}

	void ExampleWidget::draw() {
//		dpush("ExampleWidget::draw()");
		if ((canvas != NULL) && (bounds.nm.w > 0) && (bounds.nm.h > 0)) {
			if (theme != NULL) {
#if 0
				fill_rect(canvas, &bounds.nm, th->bg);
				empty_rect(canvas, &bounds.nm, th->fg);
				if (hasMouse) {
					int x,y;
					SDL_GetMouseState(&x, &y);
					if ((x>=bounds.nm.x) && (x<bounds.nm.x+bounds.nm.w) && (y>=bounds.nm.y) && (y<bounds.nm.y+bounds.nm.h)) {
						horizontal_line(canvas, bounds.nm.x, y, bounds.nm.x+bounds.nm.w-1, th->fg);
						vertical_line(canvas, x, bounds.nm.y, bounds.nm.y+bounds.nm.h-1, th->fg);
					}
				}
#endif
			}
			else {
				fill_rect(canvas, &bounds.nm, default_bg);
				empty_rect(canvas, &bounds.nm, default_fg);
				if (hasMouse) {
					int x,y;
					SDL_GetMouseState(&x, &y);
					if ((x>=bounds.nm.x) && (x<bounds.nm.x+bounds.nm.w) && (y>=bounds.nm.y) && (y<bounds.nm.y+bounds.nm.h)) {
						horizontal_line(canvas, bounds.nm.x, y, bounds.nm.x+bounds.nm.w-1, default_fg);
						vertical_line(canvas, x, bounds.nm.y, bounds.nm.y+bounds.nm.h-1, default_fg);
					}
				}
//				d("draw done");
			}
		}
//		dpop();
	}


	EventReaction ExampleWidget::handle_event(SDL_Event *e) {
	//	dpush("ExampleWidget::handle_event()");
		switch (e->type) {
			case SDL_USEREVENT:
				switch (e->user.code) {
					case mouseenter:
//						d("mouseenter");
						hasMouse = true;
						draw();
						dpop();
						return UPDATE_SCREEN;
						break;
					case mouseexit:
//						d("mouseexit");
						hasMouse = false;
						draw();
						dpop();
						return UPDATE_SCREEN;
						break;
				}
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
