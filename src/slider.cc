#include "slider.hh"
#include "debug.hh"

namespace tgui {

	BaseSlider::BaseSlider(bool vertical, Uint16 length, int min, int max, int delta) {
		if (vertical) {
			pri = 1;
			sec = 0;
		}
		else {
			pri = 0;
			sec = 1;
		}
		this->min = min;
		this->max = max;
		this->delta = delta;
		this->vertical = vertical;
		value = min;
		mhover = false;
		grabbing = false;

		shape.ix.min[pri] = length+13;
		shape.ix.min[sec] = 20;
		shape.ix.max[pri] = length+13;
		shape.ix.max[sec] = 20;
		shape.ix.grav[pri] = 0;
		shape.ix.grav[sec] = center;

		arbiter = NULL;
	}

	void BaseSlider::draw(void) {
		dpush("BaseSlider::draw");
		if (canvas != NULL) {
			if (theme != NULL) {
			}
			else {
				fill_rect(canvas, &bounds.nm, default_bg);
				empty_rect(canvas, &bounds.nm, default_fg);
				Sint16 barpos = value2pos(value);
				if (mhover) {
					if (vertical) {
						horizontal_line(canvas, bounds.nm.x+6, barpos-2, bounds.nm.x+bounds.nm.w-7, default_fg);
						horizontal_line(canvas, bounds.nm.x+4, barpos, bounds.nm.x+bounds.nm.w-5, default_fg);
						horizontal_line(canvas, bounds.nm.x+6, barpos+2, bounds.nm.x+bounds.nm.w-7, default_fg);
					}
				}
				if(vertical) {
					SDL_Rect knobr = {
						.x = (Sint16)(bounds.nm.x+2),
						.y = (Sint16)(barpos-4),
						.w = (Uint16)(bounds.nm.w-4),
						.h = 9
					};
					empty_rect(canvas, &knobr, default_fg);
				}
				else {
					vertical_line(canvas, barpos, bounds.nm.y, bounds.nm.y+bounds.nm.h-1, default_fg);
				}
			}
		}
		dpop();
	}
	

	EventReaction BaseSlider::handle_event(SDL_Event *e) {
		dpush("BaseSlider::handle_event");
		EventReaction reaction = 0;
		int oldvalue = value;
		switch (e->type) {
			case SDL_MOUSEBUTTONDOWN: {
				Uint16 pos[2] = {e->button.x, e->button.y};
				switch (e->button.button) {
					case SDL_BUTTON_RIGHT:
						// absolute positioning
						value = pos2value(pos[pri]);
						movoffs = 0;
						if (arbiter != NULL && grabbing == 0) {
							grabbing = e->button.button;
							arbiter->grab_mouse_exclusive(std::bind(&Widget::handle_event, this, std::placeholders::_1));
						}
						break;
					case SDL_BUTTON_LEFT:
						// relative positioning
						movoffs = value2pos(value) - pos[pri];
						if (arbiter != NULL && grabbing == 0) {
							grabbing = e->button.button;
							arbiter->grab_mouse_exclusive(std::bind(&Widget::handle_event, this, std::placeholders::_1));
						}
						break;
				}
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				Uint16 pos[2] = {e->button.x, e->button.y};
				switch (e->button.button) {
					case SDL_BUTTON_LEFT:
					case SDL_BUTTON_RIGHT:
						if (arbiter != NULL && grabbing == e->button.button) {
							grabbing = 0;
							movoffs = 0;
							arbiter->ungrab_mouse_exclusive();
							if (e->button.x < bounds.nm.x || e->button.x >= bounds.nm.x + bounds.nm.w ||
									e->button.y < bounds.nm.y || e->button.y >= bounds.nm.y + bounds.nm.h) {
								mhover = false;
								draw();
								reaction |= UPDATE_SCREEN;
							}
						}
						break;
					case SDL_BUTTON_WHEELUP:
						value += delta;
						movoffs = value2pos(value) - pos[pri];
						break;
					case SDL_BUTTON_WHEELDOWN:
						value -= delta;
						movoffs = value2pos(value) - pos[pri];
						break;
				}
				break;
			}
			case SDL_MOUSEMOTION: {
				if (grabbing) {
					Uint16 pos[2] = {e->motion.x, e->motion.y};
					value = pos2value(pos[pri] + movoffs);
					reaction |= UPDATE_SCREEN;
				}
				break;
			}
			case SDL_USEREVENT:
				switch (e->user.code) {
					case mouseenter:
						d("mouseenter");
						mhover = true;
						draw();
						reaction |= UPDATE_SCREEN;
						break;
					case mouseexit:
						d("mouseexit");
						mhover = false;
						draw();
						reaction |= UPDATE_SCREEN;
						break;
				}
				break;
		}
		if (value != oldvalue) {
			value = value>min?value:min;
			value = value<max?value:max;
			draw();
			if (changecb)
				reaction |= changecb(oldvalue, value);
			reaction |= UPDATE_SCREEN;
		}

		dpop();
		return reaction;
	}

	void BaseSlider::register_callback(std::function<EventReaction(int, int)> cb) {
		changecb = cb;
	}

	void BaseSlider::set_eventarbiter(EventArbiter *ea) {
		arbiter = ea;
	}

	void BaseSlider::unregister_callback(void) {
		changecb = std::function<EventReaction(int, int)>();
	}

	int BaseSlider::pos2value(int pos) {
		int value = min + ((max-min)*((bounds.ix.sz[pri]-13)-(pos-(bounds.ix.pos[pri]+6)))+((bounds.ix.sz[pri]-13)/2))/(bounds.ix.sz[pri]-13);

		return value;
	}

	int BaseSlider::value2pos(int value) {
		int pos = bounds.ix.pos[pri]+6 + (((bounds.ix.sz[pri]-13)*((max-min)-(value-min))+(max-min)/2))/(max-min);
		return pos;
	}

}
