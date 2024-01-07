#include "panel.h"

paneli* last_panel;

int paneli::getwidth() const {
	if(width)
		return width;
	return 400;
}

int paneli::getheight() const {
	if(height)
		return height;
	return 400;
}