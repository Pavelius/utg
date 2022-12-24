#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "game.h"
#include "planet.h"

using namespace draw;

static void separatorv() {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = colors::border;
	line(caret.x, caret.y + height);
	caret.y = push_caret.y;
	caret.x += 1;
	fore = push_fore;
}

static void separatorh() {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = colors::border;
	line(caret.x + width, caret.y);
	caret.x = push_caret.x;
	caret.y += 1;
	fore = push_fore;
}

static void gradient() {

}

static void panel(const char* format) {
	texta(format, AlignCenterCenter);
	caret.x += width;
	separatorv();
}

static void textcn(const char* format) {
	auto push_caret = caret;
	caret.x += (width - textw(format)) / 2;
	text(format);
	caret = push_caret;
	auto dy = texth();
	caret.y += dy;
	height -= dy;
}

static void headersm(const char* format) {
	auto push_fore = fore;
	auto push_font = font;
	fore = colors::h3;
	font = metrics::small;
	textcn(format);
	font = push_font;
	fore = push_fore;
}

static void texth3(const char* format) {
	auto push_font = font;
	font = metrics::h3;
	texta(format, AlignCenterCenter);
	font = push_font;
}

static void panel(const abilityi* pi, const char* format) {
	auto push_caret = caret;
	auto push_height = height;
	ishilite(pi);
	headersm(getnmsh(pi->id));
	separatorh();
	texth3(format);
	height = push_height;
	caret = push_caret;
	caret.x += width;
	separatorv();
}

static void panel(const abilityi* pi, const char* format, int w) {
	auto push_width = width;
	width = w;
	panel(pi, format);
	width = push_width;
}

static void panel(const char* format, int panel_width) {
	auto push_width = width;
	width = panel_width;
	panel(format);
	width = push_width;
}

static const char* getmonsthname(int i) {
	static const char* names[] = {
		"January", "February",
		"March", "April", "May",
		"June", "July", "August",
		"September", "October", "November",
		"December"
	};
	return getnmof(maptbl(names, i - 1));
}

static void data_panel() {
	char temp[64]; stringbuilder sb(temp);
	sb.add("%1i %2 %3i", game.getmonthday(), getmonsthname(game.getmonth()), game.getyear());
	panel(temp, 120);
}

static void req_panel(const abilityi* pi, int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	panel(pi, temp, 32);
}

static void showform() {
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
}

void status_info(void) {
	auto push_caret = caret;
	auto push_height = height;
	height = 32;
	showform();
	data_panel();
	for(auto& e : bsdata<abilityi>())
		req_panel(&e, 2);
	caret = push_caret;
	caret.y += height;
	height = push_height;
}

void planeti::paint() const {
	circlef(4);
	circle(4);
}

void systemi::paint() const {
	auto push_fore = fore;
	fore = colors::yellow;
	circlef(32);
	fore = colors::red.mix(fore, 128);
	circle(32);
	fore = push_fore;
}

static void painting(const object* po) {
	if(bsdata<planeti>::have(po->data))
		((planeti*)po->data)->paint();
	if(bsdata<systemi>::have(po->data))
		((systemi*)po->data)->paint();
}

void initialize_interface() {
	object::painting = painting;
}