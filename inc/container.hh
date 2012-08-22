#ifndef __TGUI_CONTAINER_HH__
#define __TGUI_CONTAINER_HH__
#include "widget.hh"
#include "base.hh"
#include <vector>
namespace tgui {



	// container.cc
	class Container : public Widget {

		public:
			virtual void configure(void) = 0;
		protected:
			EventReaction apply_mouseevent_to_child(ChildInfo *ci, Uint16 x, Uint16 y, Uint8 b, SDL_Event *e);
	};




	// scontainer.cc
	class SContainer : public Container {
		protected:
			ChildInfo child;
			int pad[2];

		public:
			SContainer(void);
			virtual void set_padding(int hpadding, int vpadding, bool doConfigure = true);
			virtual bool attach_child(Widget* c);
			virtual Widget *remove_child(void);
			virtual void configure(void);
			virtual EventReaction handle_event(SDL_Event *e);
			virtual void draw(void);
			virtual void set_canvas(SDL_Surface *c);
			virtual void place(SDL_Rect *b, bool doDraw=true);
	};




	// mcontainer.cc
	class MContainer : public Container {
		protected:
			std::vector<ChildInfo> children;
			typedef std::vector<ChildInfo>::iterator ChildIterator;

		private:
			int pri, sec;
			int pad[2];
			int chsp;

		public:
			MContainer(bool vertical);
			virtual void set_padding(int hpadding, int vpadding, int chspacing, bool doConfigure = true);
			virtual bool append_child(Widget *c, bool doConfigure = true);
			virtual bool insert_child(Widget *c, unsigned int pos = 0, bool doConfigure = true);
			virtual Widget *remove_child(int pos, bool doConfigure = true);
			virtual void configure();
			virtual void draw();
			virtual EventReaction handle_event(SDL_Event *e);	
			virtual void set_canvas(SDL_Surface *c);
			virtual void place(SDL_Rect *b, bool doDraw=true);
	};




	class HContainer : public MContainer {
		public:
			HContainer(void) : MContainer(false) {};
	};



	class VContainer : public MContainer {
		public:
			VContainer(void) : MContainer(true) {};
	};



	class Grid : public Container {
		protected:
			std::vector<std::vector<ChildInfo>> chgrid;
			unsigned columns, rows;
			int hp, vp;
			int hs, vs;
		public:
			Grid(unsigned columns, unsigned rows);
			virtual void set_padding(int hpadding, int vpadding, bool doConfigure = true);
			virtual void set_spacing(int hspacing, int vspacing, bool doConfigure = true);

			virtual void append_column(void);
			virtual void insert_column(unsigned pos);

			virtual void append_row(void);
			virtual void insert_row(unsigned pos);

			virtual Widget *set_child(Widget *c, unsigned column, unsigned row, bool doConfigure = true);
			virtual Widget *remove_child(unsigned column, unsigned row, bool doConfigure = true);

			virtual void configure();
			virtual void draw();
			virtual EventReaction handle_event(SDL_Event *e);
			virtual void set_canvas(SDL_Surface *c);
			virtual void place(SDL_Rect *b, bool doDraw = true);
	};

}

#endif

