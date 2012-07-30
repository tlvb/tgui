#ifndef __TGUI_DEFS_HH__
#define __TGUI_DEFS_HH__

namespace tGui {
	enum WFocusState {
		NORMAL,
		FOCUS,
		HOVER,
		PRESSED,
		DISABLED
	};

	const int wFocusStates = 5;

	enum HGravity {
		LEFT, HCENTER, RIGHT
	};
	
	enum VGravity {
		TOP, VCENTER, BOTTOM
	};
		 
	struct ShapeRequest {
		Uint16 minw, minh;
		Uint16 maxw, maxh;
		HGravity hGrav;
		VGravity vGrav;
		Uint16 priority;
	};

	struct ColorTheme {
		Uint32 foreground[wFocusStates];
		Uint32 passiveBackground[wFocusStates];
		Uint32 activeBackground[wFocusStates];
		Uint32 text[wFocusStates];
	};

	struct EventMask {
		Uint16 mouse;
		Uint16 key;
	};
}


#endif
