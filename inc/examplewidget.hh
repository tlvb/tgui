#ifndef __TGUI_EXAMPLE_WIDGET__
#define __TGUI_EXAMPLE_WIDGET__
#include "widget.hh"
#include "base.hh"

namespace tgui {
	class ExampleWidget : public Widget {
	
		protected:
			Uint32 bg, fg;
			bool hasMouse;

		public:
			ExampleWidget(void);
			ExampleWidget(Uint16 minw, Uint16 minh,
					Uint16 maxw, Uint16 maxh,
					Uint8 hgrav, Uint8 vgrav);
			virtual void draw(void);
			void set_preferred_shape(Shape *sh, bool doConfigure=true);
			virtual EventReaction handle_event(SDL_Event *e);
	};

}
#endif
