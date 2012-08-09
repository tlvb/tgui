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

	EventReaction ButtonBase::handle_event(SDL_Event *e) {
		EventReaction reaction = 0;
		bool oldkpress = kpress;
		bool oldmhover = mhover;
		bool oldmpress = mpress;
		switch (e->type) {
			case SDL_KEYDOWN:
				kpress = true;
				if (presscb)
					reaction |= presscb(0);
				break;
			case SDL_MOUSEBUTTONDOWN:
				buttonstate |= SDL_BUTTON(e->button.button);
				pressedhere |= SDL_BUTTON(e->button.button);
				if (presscb)
					reaction |= presscb(SDL_BUTTON(e->button.button));
				break;
			case SDL_KEYUP:
				kpress = false;
				if (releasecb)
					reaction |= releasecb(0);
				if (clickcb)
					reaction |= clickcb(0);
			case SDL_MOUSEBUTTONUP:
				if (releasecb)
					reaction |= releasecb(SDL_BUTTON(e->button.button));
				if (((pressedhere & SDL_BUTTON(e->button.button)) != 0) && clickcb)
					reaction |= clickcb(SDL_BUTTON(e->button.button));
				buttonstate &= ~SDL_BUTTON(e->button.button);
				pressedhere &= ~SDL_BUTTON(e->button.button);
				break;
			case SDL_USEREVENT: {
					MouseState *m = (MouseState*)e->user.data1;
					switch (e->user.code) {
						case mouseenter:
							mhover = true;
							if (m->b != 0) {
								if (presscb)
									reaction |= presscb(m->b);
								buttonstate = m->b;
								pressedhere = 0;
							}
							break;
						case mouseexit:
							mhover = false;
							if (buttonstate != 0) {
								if (releasecb)
									reaction |= releasecb(buttonstate);
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
/*
	EventReaction ButtonBase::handle_event(SDL_Event *e) {
		EventReaction reaction = 0;
		bool oldmhover = mhover;
		bool oldmpress = mpress;
		switch (e->type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
				buttonstate |= SDL_BUTTON(e->button.button);
				pressedhere |= SDL_BUTTON(e->button.button);
				if (presscb) {
					reaction |= presscb(SDL_BUTTON(e->button.button));
				}
				break;
			case SDL_MOUSEBUTTONUP:
			case SDL_KEYUP:
				if (releasecb) {
					reaction |= releasecb(SDL_BUTTON(e->button.button));
				}
				if (((pressedhere & SDL_BUTTON(e->button.button)) != 0) && clickcb) {
					reaction |= clickcb(SDL_BUTTON(e->button.button));
				}
				buttonstate &= ~SDL_BUTTON(e->button.button);
				pressedhere &= ~SDL_BUTTON(e->button.button);
				break;
			case SDL_USEREVENT: {
					MouseState *m = (MouseState*)e->user.data1;
					switch (e->user.code) {
						case mouseenter:
							mhover = true;
							if (m->b != 0) {
								if (presscb) {
									reaction |= presscb(m->b);
								}
								buttonstate = m->b;
								pressedhere = 0;
							}
							break;
						case mouseexit:
							mhover = false;
							if (buttonstate != 0) {
								if (releasecb) {
									reaction |= releasecb(buttonstate);
								}
								buttonstate = 0;
								pressedhere = 0;
							}
							break;
					}
			}
		}
		mpress = buttonstate != 0;
		if (mhover != oldmhover || mpress != oldmpress) {
			draw();
			reaction |= UPDATE_SCREEN;
		}
		return reaction;
	}
*/

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


	LabelButton::LabelButton(std::string text) : label(Label(text)) {
		set_padding(20,10);
		dpush("LabelButton::LabelButton()");
		attach_child(&label);
		const Shape *sp = child.w->get_preferred_shape();
		d(sp->nm.minw << "-" << sp->nm.maxw << " x " << sp->nm.minh << "-" << sp->nm.maxh);
		dpop();
	}

	void LabelButton::draw(void) {
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
	}


	void LabelButton::set_text(std::string text) {
		configure();
	}


}
