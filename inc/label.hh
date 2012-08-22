#ifndef __TGUI_LABEL_HH__
#define __TGUI_LABEL_HH__

#include "widget.hh"
#include <string>
#include "container.hh"


namespace tgui {
	
	class Label : public Widget {

		protected:
			std::string txt;
			SDL_Surface *ts;
			TTF_Font *font;

		public:
			Label(std::string text);
			virtual void draw(void);
			virtual void set_text(std::string text, bool doConfigure=true);
		protected:
			void render_text(bool doConfigure);
	};

}

#endif
