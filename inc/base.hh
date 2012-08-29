#ifndef __TGUI_BASE_HH__
#define __TGUI_BASE_HH__
#include <SDL/SDL.h>
#include <functional>


namespace tgui {

	typedef Uint8 MouseAction;
	const MouseAction postreaction = 0;
	const MouseAction mousepress = 1;
	const MouseAction mouserelease = 2;
	const MouseAction mouseclick = 3;
	const MouseAction mouseenter = 4;
	const MouseAction mouseexit = 5;

	const Uint8 void_void_callback = 6;

	typedef Uint8 Gravity;
	const Gravity left = 0;
	const Gravity top = 0;
	const Gravity center = 1;
	const Gravity right = 2;
	const Gravity bottom = 2;
	const Gravity expand = 4;

	struct Theme {
		int foo;
	};

	typedef unsigned int EventReaction;
	typedef unsigned int GrabID;
	typedef unsigned int GrabContext; 

	class EventConsumer {
		public:
			virtual EventReaction handle_event(SDL_Event *e) = 0;
	};

	const EventReaction QUIT = 1;
	const EventReaction UPDATE_SCREEN = 2;
	
	typedef std::function<EventReaction(SDL_Event*)> EventCallback;
	typedef std::function<bool(EventReaction)> ReactionTranslator;

	struct MouseState {
		Uint16 x, y;
		Uint8 b;
	};

	union Rect {
		Rect(SDL_Rect r) : nm(r) {};
		Rect(SDL_Rect *r) : nm(*r) {};
		Rect() {nm = {0,0,0,0};};
		SDL_Rect nm;
		struct {
			Sint16 pos[2];
			Uint16 sz[2];
		} ix;
	};

	union Shape {
		struct {
			Uint16 minw, minh;
			Uint16 maxw, maxh;
			Gravity hgrav, vgrav;
		} nm;
		struct {
			Uint16 min[2];
			Uint16 max[2];
			Gravity grav[2];
		} ix;
	};

	class Widget;
	struct ChildInfo {
		ChildInfo() : bounds(Rect()), w(NULL), shp(NULL), hasMouse(false) {};
		ChildInfo(Widget *w) : bounds(Rect()), w(w), shp(NULL), hasMouse(false) {};
		Rect bounds;
		Widget *w;
		const Shape *shp;
		bool hasMouse;
	};

	const int sr_bevel = 6;
	void horizontal_line(SDL_Surface *s, int x0, int y, int x1, Uint32 c);
	void vertical_line(SDL_Surface *s, int x, int y0, int y1, Uint32 c);
	void line(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 c);

	void empty_rect(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_rect(SDL_Surface *s, SDL_Rect *r, Uint32 c);

	void fill_uppertri(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_lowertri(SDL_Surface *s, SDL_Rect *r, Uint32 c);

	void empty_semirect(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_semirect(SDL_Surface *s, SDL_Rect *r, Uint32 c);
	void fill_antisemirect(SDL_Surface *s, SDL_Rect *r, Uint32 c);



}

#endif
