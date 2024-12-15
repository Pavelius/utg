#include "draw.h"
#include "draw_strategy.h"
#include "game.h"
#include "planet.h"
#include "pushvalue.h"
#include "math.h"
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

static void textcnx(const char* format) {
	auto push_caret = caret;
	caret.x += (width - textw(format)) / 2;
	text(format);
	caret = push_caret;
	auto dy = texth() - 1;
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
	textcnx(format);
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
	auto pn = getnme(ids(id, "Short"));
	if(pn)
		return pn;
	return getnm(id);
}

static void panel(const void* object, const char* header_text, const char* format) {
	auto push_caret = caret;
	auto push_height = height;
	ishilite(object);
	headersm(header_text);
	separatorh();
	texth3(format);
	height = push_height;
	caret = push_caret;
	caret.x += width;
	separatorv();
}

static void panel(const void* object, const char* header_text, const char* format, int w) {
	pushvalue push_width(width, w);
	panel(object, header_text, format);
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

static void req_panel(const void* object, int width, const char* header_text, int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	panel(object, header_text, temp, width);
}

static void req_panel(const void* object, int width, const char* header_text, int v1, int v2, int v3) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i/%2i/%3i", v1, v2, v3);
	panel(object, header_text, temp, width);
}

static void showform() {
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
}

static void req_panel(modulen id) {
	auto p = bsdata<modulei>::elements + id;
	req_panel(p, 32, getnmsh(p->getid()), player->modules[id]);
}

void status_info(void) {
	auto push_caret = caret;
	auto push_height = height;
	auto player = game.getship();
	height = 32;
	showform();
	data_panel();
	req_panel(0, 64, getnm("Credits"), player->credits);
	req_panel(0, 64, getnmsh("ShardCannons"), player->get(ShardCannons), player->get(ShardCannonsII), player->get(ShardCannonsIII));
	req_panel(0, 64, getnmsh("LaserBeams"), player->get(LaserBeams), player->get(LaserBeamsII), player->get(LaserBeamsIII));
	req_panel(0, 64, getnmsh("RocketLaunchers"), player->get(RocketLaunchers), player->get(RocketLaunchersII), player->get(RocketLaunchersIII));
	req_panel(Insight);
	req_panel(Problem);
	caret = push_caret;
	caret.y += height;
	height = push_height;
}

static void show_marker(int size) {
	auto push_caret = caret;
	caret.x -= size;
	line(caret.x + size * 2, caret.y);
	caret.x = push_caret.x;
	caret.y -= size;
	line(caret.x, caret.y + size * 2);
	caret = push_caret;
}

static void show_circle(int size) {
	circle(size);
}

static void show_rect(int size) {
	auto push_caret = caret;
	caret.x -= size;
	caret.y -= size;
	line(caret.x + size * 2, caret.y);
	line(caret.x, caret.y + size * 2);
	line(caret.x - size * 2, caret.y);
	line(caret.x, caret.y - size * 2);
	caret = push_caret;
}

void planeti::paint() const {
	const auto size = 4;
	circlef(size);
	circle(size);
	caret.y += size;
	textcx(getnm(id));
}

static void paint_space_background() {
	auto p = gres("bg01", "art/space");
	if(!p)
		return;
	auto push_caret = caret;
	caret.x -= 1000;
	caret.y -= 1000;
	image(p, 0, 0);
	caret = push_caret;
}

void systemi::paint() const {
	paint_space_background();
	auto push_fore = fore;
	fore = colors::yellow;
	circlef(32);
	fore = colors::red.mix(fore, 128);
	circle(32);
	fore = push_fore;
}

static void show_sensors(int size) {
	auto push_fore = fore;
	auto push_alpha = alpha;
	alpha = 64;
	fore = colors::blue;
	circle(size);
	alpha = push_alpha;
	fore = push_fore;
}

void ship::paint() const {
	auto push_fore = fore;
	fore = colors::green;
	auto& ei = geti();
	auto size = getpixelsize();
	switch(ei.kind) {
	case Carrier: show_rect(size); break;
	default: show_marker(size); break;
	}
	if(game.getship() == this)
		show_sensors(modules[Sensors]);
	ishilite(8, this);
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

static void main_finish() {
	drawable::input_camera();
}

static void add_system() {
	if(last_system)
		last_system->addobject();
}

static void add_planets() {
	auto rc = drawable::getscreen(-32);
	auto system_id = getbsi(last_system);
	for(auto& e : bsdata<planeti>()) {
		if(e.system != system_id)
			continue;
		if(!e.position.in(rc))
			continue;
		e.addobject();
	}
}

static void add_system_ships() {
	auto rc = drawable::getscreen(-16);
	auto system_id = getbsi(last_system);
	for(auto& e : bsdata<ship>()) {
		if(e.system != system_id)
			continue;
		if(e.state != ShipOnOrbit)
			continue;
		if(!e.position.in(rc))
			continue;
		e.addobject();
	}
}

static void preparing() {
	add_system();
	add_planets();
	add_system_ships();
}

void initialize_interface() {
	drawable::painting = painting;
	drawable::preparing = preparing;
	pbackground = main_background;
	pfinish = main_finish;
}