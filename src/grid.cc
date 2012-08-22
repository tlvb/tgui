#include "container.hh"
#include "debug.hh"

namespace tgui {

	Grid::Grid(unsigned columns, unsigned rows) :
			chgrid(std::vector<std::vector<ChildInfo>>(columns, std::vector<ChildInfo>(rows, ChildInfo()))),
			columns(columns),
			rows(rows),
			hp(5), vp(5), hs(5), vs(5){
		shape.nm.hgrav = left;
		shape.nm.vgrav = top;
	}

	void Grid::set_padding(int hpadding, int vpadding, bool doConfigure) {
		hp = hpadding;
		vp = vpadding;
		if (doConfigure && parent != NULL)
			parent->configure();
	}

	void Grid::set_spacing(int hspacing, int vspacing, bool doConfigure) {
		hs = hspacing;
		vs = vspacing;
		if (doConfigure && parent != NULL)
			parent->configure();
	}


	void Grid::append_column(void) {
		chgrid.push_back(std::vector<ChildInfo>(rows, ChildInfo()));
		++columns;
	}

	void Grid::insert_column(unsigned pos) {
		chgrid.insert(chgrid.begin()+pos, std::vector<ChildInfo>(rows, ChildInfo()));
		++columns;
	}


	void Grid::append_row(void) {
		for (std::vector<std::vector<ChildInfo>>::iterator i = chgrid.begin(); i != chgrid.end(); ++i)
			i->push_back(ChildInfo());
		++rows;
	}

	void Grid::insert_row(unsigned pos) {
		for (std::vector<std::vector<ChildInfo>>::iterator i = chgrid.begin(); i != chgrid.end(); ++i)
			i->insert(i->begin()+pos, ChildInfo());
		++rows;
	}


	Widget *Grid::set_child(Widget *c, unsigned column, unsigned row, bool doConfigure) {
		Widget *tmp = chgrid[column][row].w;
		chgrid[column][row].w = c;
		
		c->set_parent(this);
		c->set_canvas(canvas);
		if (doConfigure)
			configure();

		if (tmp != NULL) {
			tmp->set_parent(NULL);
			tmp->set_canvas(NULL);
		}

		return tmp;
	}

	Widget *Grid::remove_child(unsigned column, unsigned row, bool doConfigure) {
		return set_child(NULL, column, row, doConfigure);
	}


	void Grid::configure() {
		dpush("Grid::configure()");
		shape.nm.minw = 0;
		shape.nm.minh = 0;
		shape.nm.maxw = 0;
		shape.nm.maxh = 0;

		std::vector<Shape> shapelims(columns>rows?columns:rows, Shape());

		for (unsigned row=0; row<rows; ++row) {
			for (unsigned column=0; column<columns; ++column) {
				ChildInfo &c = chgrid[column][row];
				if (c.w != NULL) {
					c.shp = c.w->get_preferred_shape();

					if (shapelims[row].nm.minh < c.shp->nm.minh)
						shapelims[row].nm.minh = c.shp->nm.minh;
					if (shapelims[row].nm.maxh < c.shp->nm.maxh)
						shapelims[row].nm.maxh = c.shp->nm.maxh;
					if (shapelims[column].nm.minw < c.shp->nm.minw)
						shapelims[column].nm.minw = c.shp->nm.minw;
					if (shapelims[column].nm.maxw < c.shp->nm.maxw)
						shapelims[column].nm.maxw = c.shp->nm.maxw;
				}
			}
		}

/*
		for (unsigned column=0; column<columns; ++column) {
			shape.nm.minw += shapelims[column].nm.minw;
			shape.nm.maxw += shapelims[column].nm.maxw;
		}

		for (unsigned row=0; row<rows; ++row) {
			shape.nm.minh += shapelims[row].nm.minh;
			shape.nm.maxh += shapelims[row].nm.maxh;
		}
*/
		for (unsigned column=0; column<columns; ++column) {
			d("column "<<column<<" requires >= "<<shapelims[column].nm.minw);
		}

		for (unsigned row=0; row<rows; ++row) {
			d("row "<<row<<" requires >= "<<shapelims[row].nm.minh);
		}


		shape.nm.minw = hp*2 + (columns>1 ? hs*(columns-1) : 0);
		shape.nm.maxw = hp*2 + (columns>1 ? hs*(columns-1) : 0);
		shape.nm.minh = vp*2 + (rows>1 ? vs*(rows-1) : 0);
		shape.nm.maxh = vp*2 + (rows>1 ? vs*(rows-1) : 0);


		for (unsigned column=0; column<columns; ++column) {
			shape.nm.minw += shapelims[column].nm.minw;
			shape.nm.maxw += shapelims[column].nm.maxw;
		}
		for (unsigned row=0; row<rows; ++row) {
			shape.nm.minh += shapelims[row].nm.minh;
			shape.nm.maxh += shapelims[row].nm.maxh;
		}
		d("we need "<<shape.nm.minw<<"x"<<shape.nm.minh<<" in total");

		if (parent != NULL) {
			parent->configure();
		}

		dpop();

	}

	void Grid::place(SDL_Rect *b, bool doDraw) {
		dpush("Grid::place()");
		bounds = *b;

		d("we received "<<bounds.nm.x<<","<<bounds.nm.y<<" "<<bounds.nm.w<<"x"<<bounds.nm.h);
		d("of which we wanted at least "<<shape.nm.minw<<"x"<<shape.nm.minh);
		d("of which "<<(bounds.nm.w - hp*2 - (columns>1 ? hs*(columns-1) : 0))<<"x"<<(bounds.nm.h - vp*2 - (rows>1 ? vs*(rows-1) : 0))
				<<" is available to children");

		int distributable;
		int maxused;
		int ncompetitors;
		std::vector<Rect> boundlims(columns>rows?columns:rows, Rect());

		// minimum widths
		distributable = bounds.nm.w - hp*2 - (columns>1 ? hs*(columns-1) : 0);
		for (unsigned column=0; column<columns; ++column) {
			maxused = 0;
			for (unsigned row=0; row<rows; ++row) {
				ChildInfo &c = chgrid[column][row];
				c.bounds.nm.x = 0; // will be used for flags later
				c.bounds.nm.y = 0;
				c.bounds.nm.w = c.shp->nm.minw < distributable ? c.shp->nm.minw : distributable;
				maxused = c.shp->nm.minw > maxused ? c.shp->nm.minw : maxused;
			}
			distributable -= maxused;
		}

		// distribute remaining width evenly
		ncompetitors = columns;
		d(distributable<<" width points remains to be distributed among "<<ncompetitors<<" columns");
		while (distributable > 0 && ncompetitors > 0) {
			int splitshare = (distributable+1)/ncompetitors;
			if (splitshare == 0)
				splitshare = 1;
			ncompetitors = 0;
			for (unsigned column=0; column<columns; ++column) {
				bool atleastone = false;
				maxused = 0;
				for (unsigned row=0; row<rows; ++row) {
					int assignable = distributable < splitshare ? distributable : splitshare;
					ChildInfo &c = chgrid[column][row];
					int used = 0;
					if (c.bounds.nm.x == 0) {
						if (c.shp->nm.hgrav == expand || c.shp->nm.maxw - c.bounds.nm.w > assignable) {
							// we want more than is currently offered, so we are taking it all
							// and staying interested for next iteration
							c.bounds.nm.w += assignable;
							used = assignable;
							atleastone = true;
						}
						else {
							// what is offered is enough or more than we want, so we take as much
							// as we want and won't take more in any further iteration
							used = c.shp->nm.maxw - c.bounds.nm.w;
							c.bounds.nm.w = c.shp->nm.maxw;
							c.bounds.nm.x = 1;
						}
					}
					maxused = used > maxused ? used : maxused;
				}
				if (atleastone)
					++ncompetitors;
				distributable -= maxused;
			}
		}
		
		for (unsigned column=0; column<columns; ++column) {
			for (unsigned row=0; row<rows; ++row) {
				ChildInfo &c = chgrid[column][row];
				if (boundlims[column].nm.w < c.bounds.nm.w)
					boundlims[column].nm.w = c.bounds.nm.w;
			}
		}

		// set x position
		int x = bounds.nm.x + hp;
		for (unsigned column=0; column<columns; ++column) {
			for (unsigned row=0; row<rows; ++row) {
				ChildInfo &c = chgrid[column][row];
				switch (c.shp->nm.hgrav) {
					case left:
						c.bounds.nm.x = x;
						break;
					case center:
						c.bounds.nm.x = x + (boundlims[column].nm.w - c.bounds.nm.w)/2;
						break;
					case right:
						c.bounds.nm.x = x + (boundlims[column].nm.w - c.bounds.nm.w);
						break;
				}
			}
			x += boundlims[column].nm.w + hs;
		}

		//minimum heights
		distributable = bounds.nm.h - vp*2 - (rows>1 ? vs*(rows-1) : 0);
		for (unsigned row=0; row<rows; ++row) {
			maxused = 0;
			for (unsigned column=0; column<columns; ++column) {
				ChildInfo &c = chgrid[column][row];
				c.bounds.nm.h = c.shp->nm.minh < distributable ? c.shp->nm.minh : distributable;
				maxused = c.shp->nm.minh > maxused ? c.shp->nm.minh : maxused;
			}
			distributable -= maxused;
		}

		// distribute remaining height evenly
		ncompetitors = rows;
		while (distributable > 0 && ncompetitors > 0) {
			int splitshare = (distributable+1)/ncompetitors;
			if (splitshare == 0)
				splitshare = 1;
			ncompetitors = 0;
			for (unsigned row=0; row<rows; ++row) {
				bool atleastone = false;
				maxused = 0;
				for (unsigned column=0; column<columns; ++column) {
					int assignable = distributable < splitshare ? distributable : splitshare;
					ChildInfo &c = chgrid[column][row];
					int used = 0;
					if (c.bounds.nm.y == 0) {
						if (c.shp->nm.vgrav == expand || c.shp->nm.maxh - c.bounds.nm.h > assignable) {
							// we want more than is currently offered, so we are taking it all
							// and staying interested for next iteration
							c.bounds.nm.h += assignable;
							used = assignable;
							atleastone = true;
						}
						else {
							// what is offered is enough or more than we want, so we take as much
							// as we want and won't take more in any further iteration
							used = c.shp->nm.maxh - c.bounds.nm.h;
							c.bounds.nm.h = c.shp->nm.maxh;
							c.bounds.nm.y = 1;
						}
					}
					maxused = used > maxused ? used : maxused;
				}
				if (atleastone)
					++ncompetitors;
				distributable -= maxused;
			}
		}

		for (unsigned row=0; row<rows; ++row) {
			for (unsigned column=0; column<columns; ++column) {
				ChildInfo &c = chgrid[column][row];
				if (boundlims[row].nm.h < c.bounds.nm.h)
					boundlims[row].nm.h = c.bounds.nm.h;
			}
		}

		// set y position
		int y = bounds.nm.y + vp;
		for (unsigned row=0; row<rows; ++row) {
			for (unsigned column=0; column<columns; ++column) {
				ChildInfo &c = chgrid[column][row];
				switch (c.shp->nm.vgrav) {
					case top:
						c.bounds.nm.y = y;
						break;
					case center:
						c.bounds.nm.y = y + (boundlims[column].nm.h - c.bounds.nm.h)/2;
						break;
					case right:
						c.bounds.nm.y = y + (boundlims[column].nm.h - c.bounds.nm.h);
						break;
				}
			}
			y += boundlims[row].nm.h + vs;
		}

		for (unsigned row=0; row<rows; ++row) {
			for (unsigned column=0; column<columns; ++column) {
				ChildInfo &c = chgrid[column][row];
				if (c.w != NULL) {
					d("giving child "<<column<<","<<row<<" values "<<c.bounds.nm.x<<","<<c.bounds.nm.y<<" "<<c.bounds.nm.w<<"x"<<c.bounds.nm.h);
					c.w->place(&c.bounds.nm);
				}
			}
		}
		if (doDraw)
			draw();
		dpop();
	}

	void Grid::draw() {
		if (canvas != NULL && bounds.nm.w > 0 && bounds.nm.h > 0) {
			if (theme != NULL) {
				// TODO
			}
			else {
				fill_rect(canvas, &bounds.nm, default_bg);
				empty_rect(canvas, &bounds.nm, default_fg);
			}
			for (unsigned row=0; row<rows; ++row) {
				for (unsigned column=0; column<columns; ++column) {
					Widget *w = chgrid[column][row].w;
					if (w != NULL)
						w->draw();
				}
			}
		}
	}

	EventReaction Grid::handle_event(SDL_Event *e) {

		EventReaction ret = 0;
		switch (e->type) {
			case SDL_MOUSEMOTION:
				for (unsigned row=0; row<rows; ++row) {
					for (unsigned column=0; column<columns; ++column) {
						ret |= apply_mouseevent_to_child(&chgrid[column][row], e->motion.x, e->motion.y, e->motion.state, e);
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				for (unsigned row=0; row<rows; ++row) {
					for (unsigned column=0; column<columns; ++column) {
						ret |= apply_mouseevent_to_child(&chgrid[column][row], e->button.x, e->button.y, SDL_BUTTON(e->button.button), e);
					}
				}
				break;
			case SDL_USEREVENT:
				switch (e->user.code) {
					case mouseenter:
						for (unsigned row=0; row<rows; ++row) {
							for (unsigned column=0; column<columns; ++column) {
								handle_event((SDL_Event*)e->user.data1); // recursing instead of doing a whole new select
							}
						}
						break;
					case mouseexit:
						for (unsigned row=0; row<rows; ++row) {
							for (unsigned column=0; column<columns; ++column) {
								ret |= chgrid[column][row].w->handle_event(e);
							}
						}
						break;
				}
				break;
		}
		return ret;
	}
	
	void Grid::set_canvas(SDL_Surface *c) {
		canvas = c;
		for (unsigned row=0; row<rows; ++row) {
			for (unsigned column=0; column<columns; ++column) {
				Widget *w = chgrid[column][row].w;
				if (w != NULL)
					w->set_canvas(c);
			}
		}
	}



}
