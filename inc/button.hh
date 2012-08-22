#ifndef __TGUI_BUTTON_HH__
#define __TGUI_BUTTON_HH__

#include <SDL/SDL.h>
#include <utility>
#include "container.hh"
#include <functional>
#include "label.hh"
#include "base.hh"

namespace tgui {

	class ButtonBase : public SContainer {
		protected:
			std::function<EventReaction(Uint8)>	presscb;
			std::function<EventReaction(Uint8)>	releasecb;
			std::function<EventReaction(Uint8)> clickcb;
			bool mhover;
			bool mpress;
			bool kpress;
			Uint8 buttonstate;
			Uint8 pressedhere;
			virtual EventReaction handle_press(Uint8 button);
			virtual EventReaction handle_release(Uint8 button);
			virtual EventReaction handle_click(Uint8 button);
			
		public:
			ButtonBase(void);
			virtual void register_callback(std::function<EventReaction(Uint8)> cb, MouseAction action);
			virtual void unregister_callback(MouseAction action);

			virtual EventReaction handle_event(SDL_Event *e);
	};


	class Button : public ButtonBase {
		protected:
			Label label;

		public:
			Button(std::string text);
			void set_text(std::string text);
			virtual void draw(void);
	};

	class ToggleButton : public Button {
		protected:
			bool state;
			virtual EventReaction handle_click(Uint8 button);
		public:
			ToggleButton(std::string text);
			virtual void draw(void);
	};




}




#endif
