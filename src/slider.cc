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
		value = min + 0.3*(max-min);
		mhover = false;
		grabbing = false;

		shape.ix.min[pri] = length+4;
		shape.ix.min[sec] = 16;
		shape.ix.max[pri] = length+4;
		shape.ix.max[sec] = 16;

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
				int barpos = value2pos(value);
				if (mhover) {
					if(vertical) {
						if (barpos > bounds.nm.y)
							horizontal_line(canvas, bounds.nm.x, barpos-1, bounds.nm.x+bounds.nm.w-1, default_fg);
						if (barpos < bounds.nm.y+bounds.nm.h-1)
							horizontal_line(canvas, bounds.nm.x, barpos+1, bounds.nm.x+bounds.nm.w-1, default_fg);
					}
					else {
						vertical_line(canvas, barpos, bounds.nm.y, bounds.nm.y+bounds.nm.h-1, default_fg);
					}
				}
				if(vertical) {
					horizontal_line(canvas, bounds.nm.x, barpos, bounds.nm.x+bounds.nm.w-1, default_fg);
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
					case SDL_BUTTON_LEFT:
						if (arbiter != NULL && grabbing == 0) {
							grabbing = e->button.button;
							arbiter->grab_mouse_exclusive(std::bind(&Widget::handle_event, this, std::placeholders::_1));
						}
						break;
				}
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				switch (e->button.button) {
					case SDL_BUTTON_LEFT:
					case SDL_BUTTON_RIGHT:
						if (arbiter != NULL && grabbing == e->button.button) {
							grabbing = 0;
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
						break;
					case SDL_BUTTON_WHEELDOWN:
						value -= delta;
						break;
				}
				break;
			}
			case SDL_MOUSEMOTION: {
				if (grabbing) {
					Sint16 relpos[2] = {e->motion.xrel, e->motion.yrel};
					value = pos2value(value2pos(value)+relpos[pri]);
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
			reaction |= UPDATE_SCREEN;
		}

		dpop();
		return reaction;
	}

	void BaseSlider::register_callback(std::function<EventReaction(int)> cb) {
	}

	void BaseSlider::set_eventarbiter(EventArbiter *ea) {
		arbiter = ea;
	}

	void BaseSlider::unregister_callback(void) {
	}

	int BaseSlider::pos2value(int pos) {
		return (((bounds.ix.pos[pri]+2+bounds.ix.sz[pri]-1-4)-pos)*(max-min))/(bounds.ix.sz[pri]-1-4);
	}

	int BaseSlider::value2pos(int value) {
		return bounds.ix.pos[pri]+2+bounds.ix.sz[pri]-1-4 - (value*(bounds.ix.sz[pri]-1-4))/(max-min);
	}



}
