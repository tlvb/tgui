#ifndef __TGUI_BUTTON_HH__
#define __TGUI_BUTTON_HH__

#include <SDL/SDL.h>
#include <utility>
#include "container.hh"
#include <functional>
#include <label.hh>
#include "base.hh"

namespace tgui {

	class ButtonBase : public SContainer {
		protected:
			Uint8 buttonstate;
			Uint8 pressedhere;
			std::function<EventReaction(Uint8)>	presscb;
			std::function<EventReaction(Uint8)>	releasecb;
			std::function<EventReaction(Uint8)> clickcb;
			bool hover;
			bool press;
			
		public:
			ButtonBase(void);
			virtual void register_callback(std::function<EventReaction(Uint8)> cb, MouseAction action);
			virtual void unregister_callback(MouseAction action);

			virtual EventReaction handle_event(SDL_Event *e);
	};


	class LabelButton : public ButtonBase {
		protected:
			Label label;

		public:
			LabelButton(std::string text);
			void set_text(std::string text);
			virtual void draw(void);









	};




}




#endif
