#include "font.hh"
#include "debug.hh"

#ifdef __LINUX__
#include <fontconfig/fontconfig.h>
namespace tGui {
	TTF_Font *load_font(const char *name, const char *style, int size) {
		FcPattern *sp = FcPatternCreate();
		FcPatternAddString(sp, FC_FAMILY, (FcChar8*)name);
		FcPatternAddString(sp, FC_STYLE, (FcChar8*)style);
		FcObjectSet *obs = FcObjectSetCreate();
		FcObjectSetAdd(obs, FC_FILE);
		FcFontSet *fs = FcFontList(NULL, sp, obs);

		if (fs == NULL) {
			d("no font");
			return NULL;
		}
		
		FcPattern *fp = fs->fonts[0];
		char* ff;
		FcPatternGetString(fp, FC_FILE, 0, (FcChar8**)&ff);
		d("font file is "<<ff);

		TTF_Font *f = TTF_OpenFont(ff, size);

		FcFontSetDestroy(fs);
		FcObjectSetDestroy(obs);
		FcPatternDestroy(sp);

		return f;
	}

}
#else
#error "no font wrapper for your system yet. want to write one?"
#endif
