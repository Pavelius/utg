#include "draw.h"
#include "draw_strategy.h"
#include "game.h"
#include "planet.h"
#include "pushvalue.h"
#include "ship.h"

#define FOSC(T, D) ((unsigned)(D*)((T*)((void*)0x0000FFFF)) - 0x0000FFFF)

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

static void textcx(const char* format) {
	auto push_caret = caret;
	caret.x -= textw(format) / 2;
	text(format);
	caret = push_caret;
	caret.y += texth();
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

static const char* getnmsh(const char* id) {
	auto pn = getnme(str("%1Short", id));
	if(pn)
		return pn;
	return getnm(id);
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
	pushvalue push_width(width, w);
	panel(pi, format);
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
	static char temp[32];
	static stringbuilder sb(temp);
	sb.clear();
	sb.addof(getnm(maptbl(names, i - 1)));
	sb.lower();
	return temp;
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
	for(auto& e : bsdata<abilityi>()) {
		if(e.is(Ability))
			req_panel(&e, 2);
	}
	caret = push_caret;
	caret.y += height;
	height = push_height;
}

static void marker(int size) {
	auto push_caret = caret;
	caret.x -= size;
	line(caret.x + size * 2, caret.y);
	caret.x = push_caret.x;
	caret.y -= size;
	line(caret.x, caret.y + size * 2);
	caret = push_caret;
}

void planeti::paint() const {
	const auto size = 4;
	circlef(size);
	circle(size);
	caret.y += size;
	textcx(getnm(id));
}

void systemi::paint() const {
	auto push_fore = fore;
	fore = colors::yellow;
	circlef(32);
	fore = colors::red.mix(fore, 128);
	circle(32);
	fore = push_fore;
}

void ship::paint() const {
	auto push_fore = fore;
	fore = colors::green;
	marker(4);
	fore = push_fore;
}

static void painting(const drawable* po) {
	if(bsdata<planeti>::have(po))
		((planeti*)po)->paint();
	else if(bsdata<systemi>::have(po))
		((systemi*)po)->paint();
	else if(bsdata<ship>::have(po))
		((ship*)po)->paint();
}

static void main_background() {
	strategy_background();
	drawable::paintall();
}

void initialize_interface() {
	drawable::painting = painting;
	pbackground = main_background;
}