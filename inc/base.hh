#ifndef __TGUI_BASE_HH__
#define __TGUI_BASE_HH__
#include <SDL/SDL.h>
#include <functional>

namespace tgui {

	const Uint8 postreaction = 0;
	const Uint8 mouseenter = 1;
	const Uint8 mouseexit = 2;
	const Uint8 mousepress = 3;
	const Uint8 mouserelease = 4;
	const Uint8 mouseclick = 5;

	const Uint8 left = 0;
	const Uint8 top = 0;
	const Uint8 center = 1;
	const Uint8 right = 2;
	const Uint8 bottom = 2;

	struct Theme {
		int foo;
	};

	typedef unsigned int EventReaction;
	typedef unsigned long long GrabID;

	class EventConsumer {
		public:
			virtual EventReaction handle_event(SDL_Event *e) = 0;
	};

	const EventReaction QUIT = 1;
	const EventReaction UPDATE_SCREEN = 2;
	
	typedef std::function<EventReaction(SDL_Event*)> EventCallback;
	typedef std::function<bool(EventReaction)> ReactionTranslator;

	struct KeyboardCallback {
		GrabID id;
		EventCallback cb;
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
			Uint8 hgrav, vgrav;
		} nm;
		struct {
			Uint16 min[2];
			Uint16 max[2];
			Uint8 grav[2];
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

	const int sr_bevel = 12;
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
