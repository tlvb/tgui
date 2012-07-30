#include "debug.hh"
#include "abstractcontainers.hh"

namespace tGui {
	SContainer::SContainer(void) {
		child = NULL;
	}

	bool SContainer::set_child(Widget *c, bool force) {
		dpush("SContainer::set_child()");
		if (child == NULL || force) {
			bool wasNull = child == NULL;
			child = c;
			c->set_parent(this);
			if (child != NULL)
				configure();
			dpop();
			return wasNull;
		}
		else {
			dpop();
			return false;
		}
	}

	bool MContainer::append_child(Widget *c, bool doConfigure) {
		dpush("MContainer::append_child()");
		children.push_back(c);
		c->set_parent(this);
		if (doConfigure)
			configure();
		dpop();
		return true;
	}

	bool MContainer::insert_child(Widget *c, unsigned int pos, bool doConfigure) {
		dpush("MContainer::insert_child()");
		children.insert(children.begin()+pos, c);
		c->set_parent(this);
		if (doConfigure)
			configure();
		dpop();
		return true;
	}
}
