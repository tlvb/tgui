#ifndef __TGUI_VCONTAINER_HH__
#define __TGUI_VCONTAINER_HH__
#include "abstractcontainers.hh"
#include "themablewidget.hh"

namespace tGui {

	class VContainer : public MContainer, public ThemableWidget {
		protected:
			std::vector<SDL_Rect> compartments;
			int hp, vp, vs;
		public:
			VContainer(int hpadding=5, int vpadding=5, int vspacing=3);
			virtual bool insert_child(Widget *c, unsigned int pos = 0, bool doConfigure = true);
			virtual bool append_child(Widget *c, bool doConfigure = true);
			virtual void configure();
			virtual int handle_event(SDL_Event *e);
			virtual void draw();

	};




}

#endif
