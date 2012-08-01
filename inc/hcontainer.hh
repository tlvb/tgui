#ifndef __TGUI_HCONTAINER_HH__
#define __TGUI_HCONTAINER_HH__
#include "abstractcontainers.hh"
#include "themablewidget.hh"

namespace tGui {

	class HContainer : public MContainer, public ThemableWidget {
		protected:
			int hp, vp, hs;
		public:
			HContainer(int hpadding=5, int vpadding=5, int hspacing=3);
			virtual bool insert_child(Widget *c, unsigned int pos = 0, bool doConfigure = true);
			virtual bool append_child(Widget *c, bool doConfigure = true);
			virtual void configure();
			virtual int handle_event(SDL_Event *e);
			virtual void draw();

	};




}

#endif
