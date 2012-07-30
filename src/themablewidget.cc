#include "themablewidget.hh"
#include "debug.hh"

namespace tGui {

	ThemableWidget::ThemableWidget() {
		surf = NULL;
		bounds = {0};
		prefDims = {
			.minw = 0, .minh = 0,
			.maxw = 0, .maxh = 0,
			.hGrav = LEFT,
			.vGrav = TOP
		};
		colors = NULL;
		enabled = true;
		focs = NORMAL;

	}

	void ThemableWidget::draw(void) {
		dpush("ThemableWidget::draw()");
		if ((surf!=NULL) &&	(bounds.w>0) &&	(bounds.h>0)) {
			d("has surface and bounds");
			if (enabled) {
				d("is enabled");
				fill_semirect(surf, &bounds, colors->activeBackground[NORMAL]);
				fill_antisemirect(surf, &bounds, colors->passiveBackground[NORMAL]);
				empty_semirect(surf, &bounds, colors->foreground[NORMAL]);
			}
			else {
				d("is not enabled");
				fill_semirect(surf, &bounds, colors->activeBackground[DISABLED]);
				fill_antisemirect(surf, &bounds, colors->passiveBackground[DISABLED]);
				empty_semirect(surf, &bounds, colors->foreground[DISABLED]);
			}
		}
		dpop();
	}

	ColorTheme *ThemableWidget::get_color_theme(void) {
		return colors;
	}

	int ThemableWidget::handle_event(SDL_Event *e) {
		/* TODO */
		return 0;
	}

	bool ThemableWidget::is_enabled(void) {
		return enabled;
	}

	void ThemableWidget::set_color_theme(ColorTheme *t) {
		colors = t;
	}

	void ThemableWidget::set_enabled(bool e) {
		enabled = e;
	}

}
