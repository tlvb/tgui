#ifndef __TGUI_THEMABLE_WIDGET_HH__
#define __TGUI_THEMABLE_WIDGET_HH__
#include "base.hh"
#include "defs.hh"
#include "widget.hh"
#include <SDL/SDL.h>

namespace tGui {

	class ThemableWidget : public Widget {

		protected:
			ColorTheme *colors;

			bool enabled;
			WFocusState focs;

		public:
			ThemableWidget();
			ColorTheme *get_color_theme(void);
			void set_color_theme(ColorTheme *t);

			bool is_enabled(void);
			void set_enabled(bool e);
			virtual void draw(void);
			int handle_event(SDL_Event *e);


	};

}
#endif
