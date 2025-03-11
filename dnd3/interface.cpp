#include "ability.h"
#include "collection.h"
#include "command.h"
#include "creature.h"
#include "draw.h"
#include "draw_figure.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "math.h"
#include "list.h"
#include "monster.h"
#include "panel.h"
#include "room.h"
#include "widget.h"

using namespace draw;

static bool			disable_map;
static collectiona	opened_pages;
static int			column_width;
static const char*	value_format = "%1i";

void initialize_png();
void circle_image(int xm, int ym, const sprite* e, int id, int r);
void update_ui();

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

static void show_widget_panels() {
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

static void show_command_panels() {
	for(auto& e : bsdata<commandi>()) {
		auto w = e.getname();
		if(button(w, 0, menurd, false))
			execute(e.proc);
	}
}

static void show_panels() {
	pushrect push;
	caret.x += metrics::border + metrics::padding;
	caret.y += metrics::border + metrics::padding;
	show_widget_panels();
	show_command_panels();
}

static void set_opened() {
	if(hparam)
		opened_pages.add((void*)hobject);
	else
		opened_pages.remove((void*)hobject);
}

static void paint_hilite() {
	pushrect push;
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

static void check_format(const char* id) {
	auto pn = getnme(ids(id, "Format"));
	if(pn)
		value_format = pn;
}

static void paint_block(const char* id, void* object, int value, int value2 = 0) {
	auto push_format = value_format;
	check_format(id);
	char temp[32]; stringbuilder sb(temp);
	sb.add(value_format, value, value2);
	paint_block(id, object, temp);
	value_format = push_format;
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
			} else if(v.iskind<formulai>())
				values[i] = calculate(player, bsdata<formulai>::elements[v.value].formula);
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
	check_format(id);
	for(auto v : source) {
		if(v.iskind<paneli>()) {
			auto p = bsdata<paneli>::elements + v.value;
			paint_block(p->id, p->elements);
		} else if(v.iskind<listi>()) {
			auto p = bsdata<listi>::elements + v.value;
			paint_values(p->elements);
		} else if(v.iskind<abilityi>()) {
			auto p = bsdata<abilityi>::elements + v.value;
			paint_block(p->id, p, player->abilities[v.value]);
		} else if(v.iskind<consumablei>()) {
			auto p = bsdata<consumablei>::elements + v.value;
			paint_block(p->id, p, player->consumables[v.value], player->maximum.consumables[v.value]);
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
		hcursor = cursor::Hand;
		fore = colors::active;
	}
	if(opened) {
		text("-");
		if(hkey == MouseLeft && hpressed && hilite)
			execute(set_opened, 0, 0, list);
	} else {
		text("+");
		if(hkey == MouseLeft && hpressed && hilite)
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

static void paint_widget(widget* object) {
	if(!object)
		return;
	auto push_caret = caret;
	auto push_height = height;
	auto push_width = width;
	swindow(false);
	object->proc();
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

static void paint_menu_panel() {
	height = last_panel->getheight();
	if(last_panel->width == -1)
		width = getwidth() - caret.x - metrics::padding - metrics::border;
	else
		width = last_panel->getwidth();
	column_width = width - textw("0") * 6;
	if(last_panel->column)
		column_width = last_panel->column;
	for(auto v : last_panel->elements) {
		if(v.iskind<listi>()) {
			paint_panel_page(bsdata<listi>::elements + v.value);
			caret.y += metrics::padding;
		} else if(v.iskind<widget>())
			paint_widget(bsdata<widget>::elements + v.value);
	}
}

static void paint_panel() {
	if(!last_panel)
		return;
	pushrect push;
	auto push_column = column_width;
	caret.x += metrics::padding + metrics::border;
	caret.y += metrics::padding + metrics::border;
	paint_menu_panel();
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

static void background_proc() {
	strategy_background();
	paint_objects();
	paint_panel();
}

static void finish_proc() {
	input_camera();
}

static void object_drag_droping_proc() {
	last_object->position = match_grid(caret + camera);
	caret = last_object->position - camera;
	if(bsdata<creature>::have(last_object->data)) {
		auto p = (creature*)last_object->data;
		p->index = s2i(last_object->position);
		player = p;
	}
}

static void paint_monster_avatar(monsteri* p) {
	auto pa = gres(p->id, "art/avatars");
	circle_image(caret.x, caret.y, pa, 0, 32);
}

static void paint_standart_circle() {
	auto push_fore = fore;
	fore = colors::border;
	circle(32);
	fore = push_fore;
}

static void paint_drop_position() {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = colors::border;
	caret = match_grid(caret + camera);
	circle(32);
	caret = push_caret;
	fore = push_fore;
}

static void paint_monster(void* object) {
	auto p = (monsteri*)object;
	// paint_drop_position();
	paint_monster_avatar(p);
	paint_standart_circle();
}

static void paint_player_marker() {
	auto push_fore = fore;
	fore = colors::active;
	circle(32);
	circle(31);
	fore = push_fore;
}

static void paint_creature() {
	auto p = (creature*)last_object->data;
	auto pa = gres(p->avatar, "art/avatars");
	circle_image(caret.x, caret.y, pa, 0, 32);
	paint_standart_circle();
	if(player == p)
		paint_player_marker();
	if(ishilite(32, last_object))
		hcursor = cursor::Hand;
}

static point drag_drop_element(fncommand paint, void* object) {
	auto push_proc = object_before_paint; object_before_paint = 0;
	pushrect push;
	dragbegin(object);
	while(ismodal()) {
		strategy_background();
		paint_objects();
		caret = hmouse;
		paint(object);
		switch(hkey) {
		case MouseLeft:
			if(hpressed)
				execute(buttonok);
			break;
		case MouseRight:
			if(hpressed)
				execute(buttoncancel);
			break;
		case KeyEscape:
			execute(buttoncancel);
			break;
		}
		domodal();
	}
	object_before_paint = push_proc;
	dragcancel();
	if(getresult())
		return hmouse + camera;
	return {-1, -1};
}

static void paint_hilite_circle(const void* object, int size, fnevent choose_event) {
	auto push_fore = fore;
	if(ishilite(size)) {
		fore = colors::active;
		if(hkey == MouseLeft && hpressed)
			fore = fore.mix(colors::border);
		hcursor = cursor::Hand;
		if(hkey == MouseLeft && !hpressed)
			execute(choose_event, (long)object);
	} else
		fore = colors::border;
	circle(size);
	fore = push_fore;
}

static void cancel_area() {
	auto is_hilite = ishilite();
	if(hkey == KeyEscape || (hkey == MouseLeft && hpressed && !is_hilite))
		execute(buttoncancel);
}

static void choose_menu_window(const array& source, fncommand paint) {
	const int dx = 72;
	pushrect push;
	auto maximum = source.getcount();
	width = imin(maximum, size_t(10)) * dx + metrics::padding;
	height = dx;
	swindow(false);
	cancel_area();
	caret.x += dx / 2;
	caret.y += dx / 2;
	for(size_t i = 0; i < maximum; i++) {
		auto p = source.ptr(i);
		paint(source.ptr(i));
		paint_hilite_circle(p, 32, buttonparam);
		caret.x += dx;
	}
}

static long choose_menu(const array& source, fncommand paint) {
	pushrect push;
	while(ismodal()) {
		strategy_background();
		paint_objects();
		caret.x += metrics::padding + metrics::border;
		caret.y += metrics::padding + metrics::border;
		choose_menu_window(source, paint);
		paintfinish();
		domodal();
	}
	return getresult();
}

static void monsters_select_and_place() {
	auto choosen = (monsteri*)choose_menu(bsdata<monsteri>::source, paint_monster);
	if(!choosen)
		return;
	auto pt = drag_drop_element(paint_monster, choosen);
	if(pt.x < 0)
		return;
	auto push_player = player;
	player->create(choosen->id);
	player->index = s2i(pt);
	update_ui();
	player = push_player;
}

void initialize_ui() {
	initialize_png();
	pbackground = background_proc;
	pfinish = finish_proc;
	metrics::padding = 2;
	object_before_paint = object_drag_drop;
	object_drag_droping = object_drag_droping_proc;
}

static draworder* modify(object* po) {
	return po->addorder(1000);
}

static void update_object(void* object, point pt, fnevent paint, int priority, bool appearing = true) {
	auto pd = findobject(object);
	if(!pd) {
		pd = addobject(pt, object, paint, 0, priority, 0);
		if(appearing) {
			auto po = modify(pd);
			po->alpha = 255;
		} else
			pd->alpha = 255;
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
		update_object(&e, e.getscreenc(), paint_creature, 30, false);
}

void update_ui() {
	update_rooms();
	update_creatures();
	wait_all();
}

BSDATA(widget) = {
	{"EmphtyWidget"}
};
BSDATAF(widget)
BSDATA(commandi) = {
	{"Monsters", monsters_select_and_place}
};
BSDATAF(commandi)