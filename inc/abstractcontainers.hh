#ifndef __TGUI_ABSTRACTCONTAINERS_HH__
#define __TGUI_ABSTRACTCONTAINERS_HH__

#include "widget.hh"
#include <vector>

namespace tGui {

	class Widget;
	class Container {
		public:
			virtual void configure() = 0;
	};

	class SContainer : public Container {
		protected:
			Widget *child;
		public:
			SContainer(void);
			virtual bool set_child(Widget *c, bool force=false);
	};

	class MContainer : public Container {
		protected:
			std::vector<Widget*> children;
		public:
			virtual bool insert_child(Widget *c, unsigned int pos = 0, bool doConfigure = true);
			virtual bool append_child(Widget *c, bool doConfigure = true);
	};

}

#endif

