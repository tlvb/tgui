#include "button.hh"
#include "debug.hh"

namespace tgui {
	
	ButtonBase::ButtonBase(void) {
		presscb = std::function<EventReaction(Uint8)>();
		releasecb = std::function<EventReaction(Uint8)>();
		clickcb = std::function<EventReaction(Uint8)>();
		buttonstate = 0;
		pressedhere = 0;
		mhover = false;
		mpress = false;
		kpress = false;
	}

	EventReaction ButtonBase::handle_press(Uint8 button) {
		if (presscb)
			return presscb(button);
		return 0;
	}

	EventReaction ButtonBase::handle_release(Uint8 button) {
		if (releasecb)
			return releasecb(button);
		return 0;
	}

	EventReaction ButtonBase::handle_click(Uint8 button) {
		if (clickcb)
			return clickcb(button);
		return 0;
	}

	EventReaction ButtonBase::handle_event(SDL_Event *e) {
		EventReaction reaction = 0;
		bool oldkpress = kpress;
		bool oldmhover = mhover;
		bool oldmpress = mpress;
		switch (e->type) {
			case SDL_KEYDOWN:
				kpress = true;
				reaction |= handle_press(0);
				break;
			case SDL_MOUSEBUTTONDOWN:
				buttonstate |= SDL_BUTTON(e->button.button);
				pressedhere |= SDL_BUTTON(e->button.button);
				reaction |= handle_press(SDL_BUTTON(e->button.button));
				break;
			case SDL_KEYUP:
				kpress = false;
				reaction |= handle_release(0);
				reaction |= handle_click(0);
				break;
			case SDL_MOUSEBUTTONUP:
				reaction |= handle_release(SDL_BUTTON(e->button.button));
				if ((pressedhere & SDL_BUTTON(e->button.button)) != 0)
					reaction |= handle_click(SDL_BUTTON(e->button.button));
				buttonstate &= ~SDL_BUTTON(e->button.button);
				pressedhere &= ~SDL_BUTTON(e->button.button);
				break;
			case SDL_USEREVENT: {
					MouseState *m = (MouseState*)e->user.data1;
					switch (e->user.code) {
						case mouseenter:
							mhover = true;
							if (m->b != 0) {
								reaction |= handle_press(m->b);
								buttonstate = m->b;
								pressedhere = 0;
							}
							break;
						case mouseexit:
							mhover = false;
							if (buttonstate != 0) {
								reaction |= handle_release(buttonstate);
								buttonstate = 0;
								pressedhere = 0;
							}
							break;
					}
			}
		}
		mpress = buttonstate != 0;
		if (mhover != oldmhover || mpress != oldmpress || kpress != oldkpress) {
			draw();
			reaction |= UPDATE_SCREEN;
		}
		return reaction;
	}

	void ButtonBase::register_callback(std::function<EventReaction(Uint8)> cb, MouseAction action) {
		switch (action) {
			case mousepress:
				presscb = cb;
				break;
			case mouserelease:
				releasecb = cb;
				break;
			case mouseclick:
				clickcb = cb;
				break;
		}
	}

	void ButtonBase::unregister_callback(MouseAction action) {
		register_callback(std::function<EventReaction(Uint8)>(), action);
	}


	Button::Button(std::string text) : label(Label(text)) {
		set_padding(20,10);
		dpush("Button::Button()");
		attach_child(&label);
		const Shape *sp = child.w->get_preferred_shape();
		d(sp->nm.minw << "-" << sp->nm.maxw << " x " << sp->nm.minh << "-" << sp->nm.maxh);
		dpop();
	}

	void Button::draw(void) {
		dpush("Button::draw()");
		if (canvas != NULL && bounds.nm.w > 0 && bounds.nm.h > 0 ) {
			if (theme != NULL) {
			}
			else {
				fill_rect(canvas, &bounds.nm, default_bg);
				empty_semirect(canvas, &bounds.nm, default_fg);
				if (mhover || kpress) {
					if (bounds.nm.w >= 4 && bounds.nm.h >= 4) {
						SDL_Rect r = {
							.x = (Sint16) (bounds.nm.x + 2),
							.y = (Sint16) (bounds.nm.y + 2),
							.w = (Uint16) (bounds.nm.w - 4),
							.h = (Uint16) (bounds.nm.h - 4)
						};
						if (mpress || kpress) {
							if (bounds.nm.w >= 8 && bounds.nm.h >= 8) {
								r.x += 2;
								r.y += 2;
								r.w -= 4;
								r.h -= 4;
							}
						}
						empty_semirect(canvas, &r, default_fg);
					}
				}
			}
		}
		label.draw();
		dpop();
	}


	void Button::set_text(std::string text) {
		configure();
	}

	ToggleButton::ToggleButton(std::string text) : Button(text) {
		state = false;
	}

	void ToggleButton::draw(void) {
		dpush("ToggleButton::draw()");
		Button::draw();
		if (canvas != NULL && bounds.nm.w > 0 && bounds.nm.h > 0 ) {
			if (theme != NULL) {
			}
			else {
				if(state) {
					fill_antisemirect(canvas, &bounds.nm, default_fg);
				}
				else {
					empty_rect(canvas, &bounds.nm, default_fg);
				}
			}
		}
		d("/TB::draw()");
		dpop();
	}

	EventReaction ToggleButton::handle_click(Uint8 button) {
		EventReaction reaction = Button::handle_click(button);
		reaction |= UPDATE_SCREEN;
		state = !state;
		return reaction;
	}

}
