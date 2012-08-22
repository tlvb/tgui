#ifndef __TGUI_SLIDER_HH__
#define __TGUI_SLIDER_HH__

#include "widget.hh"

	namespace tgui {
		

		class BaseSlider : public Widget {

			protected:
				std::function<EventReaction(int, int)> changecb;
				int pri, sec;
				EventArbiter *arbiter;
				int min, max, value, delta;
				int movoffs;
				bool vertical;
				bool mhover;
				Uint8 grabbing;

			public:
				BaseSlider(bool vertical, Uint16 length, int min, int max, int delta=1);
				virtual void register_callback(std::function<EventReaction(int, int)> cb);
				virtual void unregister_callback(void);
				virtual void set_eventarbiter(EventArbiter *ea);

				virtual EventReaction handle_event(SDL_Event *e);
				virtual void draw(void);

			protected:
				int pos2value(int pos);
				int value2pos(int value);



		};

	}

#endif
