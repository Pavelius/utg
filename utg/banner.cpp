#include "banner.h"
#include "bsdata.h"
#include "draw.h"

BSDATA(banneri) = {
	{"GrayBanner", {140, 155, 171}, {58, 69, 80}},
	{"RedBanner", {174, 42, 25}, {84, 21, 12}},
	{"YellowBanner", {226, 178, 3}, {90, 70, 1}},
	{"GreenBanner", {148, 199, 72}, {90, 70, 1}},
};

using namespace draw;

void show_bannerf(int size, int n) {
	auto push_fore = fore;
	fore = bsdata<banneri>::elements[n].fore;
	circlef(size);
	fore = push_fore;
}

void show_bannerb(int size, int n) {
	auto push_fore = fore;
	fore = bsdata<banneri>::elements[n].border;
	circle(size);
	fore = push_fore;
}

void show_banner(int size, int n) {
	show_bannerf(size, n);
	show_bannerb(size, n);
}

void show_banner_text(int size, int n, const char* format) {
	auto push_fore = fore;
	auto push_caret = caret;
	auto push_font = font;
	font = metrics::h2;
	fore = bsdata<banneri>::elements[n].border;
	caret.y -= texth() / 2;
	caret.x -= textw(format) / 2;
	text(format);
	font = push_font;
	caret = push_caret;
	fore = push_fore;
}

void show_banner(int size, int n, const char* format) {
	show_banner(size, n);
	show_banner_text(size, n, format);
}