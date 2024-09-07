#include "draw_gui.h"
#include "slice.h"

using namespace draw;

guii draw::gui;

void guii::clear() {
	memset(this, 0, sizeof(*this));
}