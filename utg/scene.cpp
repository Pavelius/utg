#include "draw.h"
#include "scene.h"

using namespace draw;

void scenei::background(const char* id, const char* folder) const {
	if(!folder)
		folder = "art/background";
	auto ps = gres(id, folder);
	if(ps)
		image(caret.x, caret.y, ps, 0, 0);
}

void* scenei::show() {
	auto push_caret = caret;
	auto push_width = width;
	while(ismodal()) {
		if(pbackground)
			pbackground();
		render();
		if(pwindow)
			pwindow();
		domodal();
	}
	return (void*)getresult();
}