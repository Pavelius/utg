#include "ability.h"
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
static int column_width;
static const char* value_format = "%1i";

void initialize_png();
void circle_image(int xm, int ym, const sprite* e, int id, int r);

int calculate(statable* p, variants source);

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

static void paint_hilite() {
	rectpush push;
	auto push_fore = fore;
	auto push_alpha = alpha;
	fore = colors::active;
	alpha = 128;
	caret.x -= 4; width += 4;
	caret.y -= 1; height += 2;
	rectf();
	alpha = push_alpha;
	fore = push_fore;
}

static void paint_block(const char* id, void* object, const char* value) {
	auto push_height = height;
	auto push_caret = caret;
	height = texth();
	auto hilite = ishilite();
	if(hilite)
		paint_hilite();
	text(getnm(id));
	caret.x += column_width;
	text(value);
	caret.y += height + 1;
	caret.x = push_caret.x;
	height = push_height;
}

static void paint_block(const char* id, void* object, int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(value_format, value);
	paint_block(id, object, temp);
}

static void paint_values(const variants& elements) {
	int values[6] = {};
	size_t list_count = elements.size();
	abilityi* object = 0;
	for(size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
		if(i >= list_count)
			values[i] = 0;
		else {
			auto v = elements.begin()[i];
			if(v.iskind<abilityi>()) {
				if(!object)
					object = bsdata<abilityi>::elements + v.value;
				values[i] = player->abilities[v.value];
			} else if(v.iskind<listi>())
				values[i] = calculate(player, bsdata<listi>::elements[v.value].elements);
		}
	}
	if(!object)
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.add(value_format, values[0], values[1], values[2], values[3], values[4], values[5]);
	paint_block(object->id, object, temp);
}

static void paint_block(const char* id, const variants& source) {
	auto push_format = value_format;
	auto pn = getnme(stw(id, "Format"));
	if(pn)
		value_format = pn;
	for(auto v : source) {
		if(v.iskind<paneli>()) {
			auto p = bsdata<paneli>::elements + v.value;
			paint_block(p->id, p->elements);
		}
		else if(v.iskind<listi>()) {
			auto p = bsdata<listi>::elements + v.value;
			paint_values(p->elements);
		}
		else if(v.iskind<abilityi>()) {
			auto p = bsdata<abilityi>::elements + v.value;
			paint_block(p->id, p, player->abilities[v.value]);
		} else if(v.iskind<consumablei>()) {
			auto p = bsdata<consumablei>::elements + v.value;
			paint_block(p->id, p, player->consumables[v.value]);
		}
	}
	value_format = push_format;
}

static void paint_bullet(listi* list, bool opened) {
	auto push_caret = caret;
	auto push_fore = fore;
	auto hilite = ishilite({caret.x, caret.y, caret.x + width, caret.y + texth()});
	fore = colors::h3;
	if(hilite) {
		hot.cursor = cursor::Hand;
		fore = colors::active;
	}
	if(opened) {
		text("-");
		if(hot.key == MouseLeft && hot.pressed && hilite)
			execute(set_opened, 0, 0, list);
	} else {
		text("+");
		if(hot.key == MouseLeft && hot.pressed && hilite)
			execute(set_opened, 1, 0, list);
	}
	caret.x += textw("0") * 2;
	text(getnm(list->id));
	fore = push_fore;
	caret = push_caret;
}

static void paint_closed_panel(listi* list) {
	if(!list)
		return;
	auto push_height = height;
	height = texth();
	swindow(false);
	paint_bullet(list, false);
	caret.y += height + metrics::border * 2;
	height = push_height;
}

static int paint_block_height(const char* id, const variants& source) {
	auto push_clip = clipping;
	auto push_caret = caret;
	auto push_height = height;
	caret.x = 0; caret.y = 0;
	clipping.set(0, 0, width, 0);
	paint_block(id, source);
	auto result = caret.y;
	caret = push_caret;
	clipping = push_clip;
	return result;
}

static void paint_open_panel(listi* list) {
	if(!list)
		return;
	auto push_caret = caret;
	auto push_height = height;
	auto push_width = width;
	height = paint_block_height(list->id, list->elements) + texth() + metrics::border;
	swindow(false);
	paint_bullet(list, true);
	caret.x += texth(); width -= texth();
	caret.y += texth() + metrics::border;
	paint_block(list->id, list->elements);
	caret.y += metrics::border * 2;
	caret.x = push_caret.x;
	width = push_width;
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
	auto push_column = column_width;
	caret.x += metrics::padding + metrics::border;
	caret.y += metrics::padding + metrics::border;
	height = last_panel->getheight();
	width = last_panel->getwidth();
	column_width = width - textw("0") * 6;
	if(last_panel->column)
		column_width = last_panel->column;
	for(auto v : last_panel->elements) {
		if(v.iskind<listi>()) {
			paint_panel_page(bsdata<listi>::elements + v.value);
			caret.y += metrics::padding;
		}
	}
	column_width = push_column;
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