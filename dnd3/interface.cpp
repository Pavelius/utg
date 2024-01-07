#include "collection.h"
#include "crt.h"
#include "creature.h"
#include "draw.h"
#include "draw_figure.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "list.h"
#include "monster.h"
#include "panel.h"
#include "room.h"
#include "widget.h"

using namespace draw;

static void* current_object;
static collectiona opened_pages;

void initialize_png();
void circle_image(int xm, int ym, const sprite* e, int id, int r);

bool menurd(const char* title);

const int timer_seg = 1000;

static void horizontal_border() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x -= metrics::border;
	caret.y += metrics::padding;
	line(caret.x + width + metrics::border * 2, caret.y);
	caret = push_caret;
	caret.y += metrics::padding * 2;
	fore = push_fore;
}

static void title_text(const char* string) {
	auto push_font = font;
	font = metrics::h2;
	texta(string, AlignCenter);
	caret.y += texth();
	font = push_font;
}

static void copy(point& p1, const point& p2) {
	p1 = p2;
}

void roomi::paint() const {
	image(gres(avatar, "maps"), 0, flags);
}

void creature::paint() const {
	auto pa = gres(avatar, "art/avatars");
	circle_image(caret.x, caret.y, pa, 0, 32);
	circle(32);
	if(ishilite(32, last_object))
		hot.cursor = cursor::Hand;
}

void monsteri::paint() const {
	auto pa = gres(id, "art/avatars");
	circle_image(caret.x, caret.y, pa, 0, 32);
	//circle(32);
}

static void paint_selected_border() {
	auto push_fore = fore;
	fore = colors::button;
	circle(34);
	circle(33);
	circle(32);
	fore = push_fore;
}

static void paint_hilite_border() {
	auto push_fore = fore;
	fore = colors::active;
	circle(32);
	fore = push_fore;
}

static void paint_hilite_object(const void* pv) {
	if(current_object == pv)
		paint_selected_border();
	if(ishilite(width / 2)) {
		hilite_object = pv;
		paint_hilite_border();
		if(hot.key == MouseLeft && hot.pressed)
			execute(cbsetptr, (long)pv, 0, &current_object);
	}
}

static void paint_drag_target() {
	auto pv = getdragactive();
	if(dragactive(pv)) {
		auto push_caret = caret;
		caret = hot.mouse;
		//object_painting_data(pv);
		caret = push_caret;
	}
}

static void show_panel(int dx, int dy) {
	rectpush push;
	height = dy; width = dx;
	caret.y += metrics::padding;
	caret.x += metrics::padding;
	caret.x += dx / 2;
	caret.y += dy / 2;
	for(auto& e : bsdata<monsteri>()) {
		e.paint();
		paint_hilite_object(&e);
		caret.x += width + metrics::padding;
	}
}

static bool menurd(const char* title) {
	if(!title)
		return false;
	auto push_caret = caret;
	auto push_height = height;
	textfs(title);
	auto result = swindow(true);
	textf(title);
	height = push_height;
	width += metrics::border * 2;
	caret = push_caret;
	return result;
}

static void show_panels() {
	rectpush push;
	caret.x += metrics::border + metrics::padding;
	caret.y += metrics::border + metrics::padding;
	for(auto& e : bsdata<paneli>()) {
		auto w = e.getname();
		if(button(w, 0, menurd, false)) {
			if(last_panel == &e)
				execute(cbsetptr, 0, 0, &last_panel);
			else
				execute(cbsetptr, (long)&e, 0, &last_panel);
		}
	}
}

static void set_opened() {
	if(hot.param)
		opened_pages.add((void*)hot.object);
	else
		opened_pages.remove((void*)hot.object);
}

static void paint_bullet(listi* list, bool opened) {
	auto w = textw("aa");
	auto hilite = ishilite({caret.x, caret.y, caret.x + w, caret.y + texth()});
	if(opened) {
		text("-");
		if(hot.key == MouseLeft && hot.pressed && hilite)
			execute(set_opened, 0, 0, list);
	} else {
		text("+");
		if(hot.key == MouseLeft && hot.pressed && hilite)
			execute(set_opened, 1, 0, list);
	}
	caret.x += w;
}

static void paint_closed_panel(listi* list) {
	if(!list)
		return;
	auto push_caret = caret;
	auto push_height = height;
	height = texth();
	swindow(false);
	paint_bullet(list, false);
	text(getnm(list->id));
	caret.x = push_caret.x;
	caret.y += height + metrics::border * 2;
	height = push_height;
}

static void paint_open_panel(listi* list) {
	if(!list)
		return;
	auto push_caret = caret;
	auto push_height = height;
	height = 200;
	swindow(false);
	paint_bullet(list, true);
	text(getnm(list->id));
	caret.x = push_caret.x;
	caret.y += height + metrics::border * 2;
	height = push_height;
}

static bool is_opened(const listi* list) {
	return opened_pages.find((void*)list) != -1;
}

static void paint_panel_page(listi* list) {
	if(is_opened(list))
		paint_open_panel(list);
	else
		paint_closed_panel(list);
}

static void paint_panel() {
	if(!last_panel)
		return;
	rectpush push;
	caret.x += metrics::padding + metrics::border;
	caret.y += metrics::padding + metrics::border;
	height = last_panel->getheight();
	width = last_panel->getwidth();
	for(auto v : last_panel->elements) {
		if(v.iskind<listi>()) {
			paint_panel_page(bsdata<listi>::elements + v.value);
			caret.y += metrics::padding;
		}
	}
	//swindow(false);
	//auto pn = getnme(stw(last_panel->id, "Title"));
	//if(pn) {
	//	//caret.y -= metrics::border;
	//	title_text(pn);
	//	horizontal_border();
	//}
}

void status_info() {
	auto push_height = height;
	height = metrics::padding * 2 + metrics::border * 2 + texth();
	gradv(colors::border.lighten().lighten(), colors::border.darken().darken());
	show_panels();
	caret.y += height;
	height = push_height;
}

static void ui_background() {
	strategy_background();
	paint_objects();
	paint_panel();
	paint_drag_target();
}

static void add_current_object() {
}

static void put_selected_object() {
	if(!current_object)
		return;
	if(mouseinobjects() && hot.key == KeySpace)
		execute(add_current_object);
}

static void drag_selected_object() {
	if(hilite_object) {
	}
}

static void ui_finish() {
	put_selected_object();
	input_camera();
}

void initialize_ui() {
	initialize_png();
	pbackground = ui_background;
	pfinish = ui_finish;
	metrics::padding = 2;
}

static draworder* modify(object* po) {
	return po->addorder(1000);
}

static void update_object(void* object, point pt, fnevent paint, int priority) {
	auto pd = findobject(object);
	if(!pd) {
		pd = addobject(pt, object, paint, 0, priority, 0);
		auto po = modify(pd);
		po->alpha = 255;
	} else if(pt != pd->position) {
		auto po = modify(pd);
		po->position = pt;
	}
}

static void update_rooms() {
	for(auto& e : bsdata<roomi>())
		update_object(&e, e.getscreen(), ftpaint<roomi>, 21);
}

static void update_creatures() {
	for(auto& e : bsdata<creature>())
		update_object(&e, e.getscreenc(), ftpaint<creature>, 30);
}

void update_ui() {
	update_rooms();
	update_creatures();
	wait_all();
}