#include "answers.h"
#include "area.h"
#include "card.h"
#include "deck.h"
#include "draw.h"
#include "draw_figure.h"
#include "draw_hexagon.h"
#include "draw_marker.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "entitya.h"
#include "pushvalue.h"
#include "player.h"
#include "province.h"
#include "script.h"
#include "structure.h"
#include "troop.h"
#include "unit.h"
#include "widget.h"

using namespace draw;

const int hms = 8;
inline point i2h(short unsigned i) { return {(short)(i % hms), (short)(i / hms)}; }
inline short unsigned h2i(point v) { return v.y * hms + v.x; }

int count_cards(const playeri* player, const entity* location);
bool filter_player_upgrade(const void* object);
const char* getinform(const void* object);

const int button_height = 20;
const int button_width = 43;
const int size = 256;
const int player_avatar_size = 32;
const int player_marker_size = 16;
const int indicator_width = 48;
const int tech_padding = 16;
const char* message_string;

static widget* show_widget;

static color tech_colors[] = {
	{40, 40, 40},
	{227, 91, 110},
	{152, 199, 108},
	{53, 189, 238},
	{249, 238, 106},
};

static point province_points[8] = {
	{-100, 120},
	{100, 120},
	{-80, -120},
	{80, -120},
};

static void empthy_scene() {
}

static color getplayercolor(const playeri* p) {
	return p->fore;
}

static point system_position(point caret, int index) {
	switch(index) {
	case 1: return caret + point{size / 3, -2 * size / 3};
	default: return caret + point{-size / 3, -2 * size / 3};
	}
}

static point planet_position(point caret, int index) {
	const double cos_30 = 0.86602540378;
	auto fsize = 6 * size / 10;
	point position[] = {
		{(short)(caret.x + fsize * cos_30), (short)(caret.y - fsize / 2)},
		{(short)(caret.x - fsize * cos_30), (short)(caret.y + fsize / 2)},
		{(short)(caret.x + fsize / 2), (short)(caret.y + 2 * fsize / 3)},
		{caret}
	};
	return position[index];
}

static void texth2cg(const char* format) {
	auto push_fore = fore;
	fore = fore.mix(colors::window, 64);
	texth2c(format);
	fore = push_fore;
}

static void show_players() {
	auto push_y = caret.y;
	caret.x += 16; caret.y += 20;
	auto push_x = caret.x;
	auto res = gres("races_small");
	auto push_alpha = alpha;
	alpha = 32;
	for(auto& e : bsdata<playeri>()) {
		//image(res, getbsi((playeri*)p), 0);
		if(ishilite(16)) {
			hilite_object = &e;
			hilite_size = 17;
			hilite_position = caret;
		}
		caret.x += 32;
	}
	alpha = push_alpha;
	//auto pi = players.find(player);
	//if(pi != -1) {
	//	auto p = players[pi];
	//	auto push_x1 = caret.x;
	//	caret.x = push_x + 32 * pi;
	//	//image(res, getbsi(p), 0);
	//	auto push_fore = fore;
	//	caret.x = push_x1;
	//}
	caret.y = push_y;
	caret.x -= 16;
}

static const char* getnmsh(const char* id) {
	auto pn = getnme(str("%1Short", id));
	if(!pn)
		pn = getnm(id);
	return pn;
}

static void show_widget_scene() {
	while(show_widget && ismodal()) {
		paintstart();
		caret.x += metrics::padding + metrics::border;
		caret.y += metrics::padding + metrics::border;
		show_widget->proc();
		paintfinish();
		domodal();
	}
}

static void open_widget() {
	if(show_widget) {
		auto p = (widget*)hobject;
		if(show_widget == p)
			show_widget = 0;
		else
			show_widget = p;
	} else {
		auto push = show_widget;
		show_widget = (widget*)hobject;
		show_widget_scene();
		show_widget = push;
	}
}

static void hilite_button() {
	pushvalue push_fore(fore, colors::button);
	pushvalue push_alpha(alpha, (unsigned char)(hpressed ? 16 : 32));
	pushvalue push_height(height, height + 4);
	rectf();
}

static void marked_button() {
	pushvalue push_fore(fore, colors::active);
	pushvalue push_alpha(alpha, (unsigned char)64);
	pushvalue push_height(height, height + 4);
	rectf();
}

static void status(const char* id, const char* value, const void* object, int additional_width = 0) {
	auto name = getnmsh(id);
	auto push_fore = fore;
	auto push_font = font;
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	additional_width += indicator_width;
	width = additional_width;
	height = 32 + 4;
	if(object && show_widget == object)
		marked_button();
	if(ishilite(object)) {
		if(bsdata<widget>::have(object)) {
			hilite_button();
			if(hkey == MouseLeft && !hpressed)
				execute(open_widget, 0, 0, object);
		}
	}
	fore = colors::border;
	line(caret.x, caret.y + 40);
	caret = push_caret;
	font = metrics::h3;
	fore = colors::h3;
	textcnw(name);
	caret.y += texth();
	font = metrics::h2;
	fore = colors::text;
	caret.x = push_caret.x;
	textcnw(value);
	font = push_font;
	caret = push_caret;
	width = push_width;
	caret.x += additional_width;
	push_caret = caret;
	fore = colors::border;
	line(caret.x, caret.y + 40);
	caret = push_caret;
	fore = push_fore;
	height = push_height;
}

static void status(const char* id, int value, const void* object, int additional_width = 0) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value); status(id, temp, object, additional_width);
}

static void status(const char* id, int value, int maximum, const void* object) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i/%2i", value, maximum); status(id, temp, object);
}

static void status(ability_s v) {
	switch(v) {
	case Goods: case Resources: case Influence:
		status(bsdata<abilityi>::elements[v].id, player->get(v), player->getmaximum(v), bsdata<abilityi>::elements + v);
		break;
	case Fame:
		status(bsdata<abilityi>::elements[v].id,
			player->get(v), player->getmaximum(v),
			bsdata<widget>::find("OpenObjectives"));
		break;
	case Tactic:
		status(bsdata<abilityi>::elements[v].id,
			count_cards(player, bsdata<decki>::elements + TacticsDeck), player->getmaximum(v),
			bsdata<widget>::find("TacticForm"));
		break;
	default:
		status(bsdata<abilityi>::elements[v].id, player->get(v), bsdata<abilityi>::elements + v);
		break;
	}
}

static void show_indicators() {
	static ability_s source_tokes[] = {Hero, Army};
	static ability_s source_goods[] = {Resources, Influence, Gold, Goods};
	static ability_s source_score[] = {Fame};
	for(auto v : source_tokes)
		status(v);
	caret.x += 2;
	for(auto v : source_goods)
		status(v);
	caret.x += 2;
	for(auto v : source_score)
		status(v);
	caret.x += 2;
	status(Tactic);
	caret.x += 2;
	status("Upgrade", player->getupgradecount(), bsdata<widget>::find("UpgradesForm"), 8);
	caret.x += 2;
}

void status_info() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 4;
	show_players();
	fore = push_fore;
	show_indicators();
	caret = push_caret;
	caret.y += 4 * 2 + 32;
}

static void paint_hexagon() {
	auto push_size = fsize; fsize = size;
	auto push_fore = fore; fore = colors::border;
	hexagon();
	fore = push_fore;
	fsize = push_size;
}

static void paint_special(int index, int frame) {
	auto push_caret = caret; caret = planet_position(caret, index);
	image(gres("tiles"), frame, 0);
	caret = push_caret;
}

static void paint_player_banner() {
	auto r = gres("buildings", "art/fonts");
	image(r, 4, 0);
}

static void paint_player_banner(const playeri* player) {
	if(!player)
		return;
	auto push_fore = fore;
	fore = player->fore;
	paint_player_banner();
	fore = push_fore;
}

static void paint_player_marker(const playeri* p) {
	pushvalue push_alpha(alpha, (unsigned char)128);
	paint_player_banner(p);
}

static void paint_player_markers(const provincei* province) {
	auto push_caret = caret;
	caret.y -= size - 24;
	for(auto& e : bsdata<playeri>()) {
		if(province->is(&e)) {
			paint_player_marker((playeri*)&e);
			caret.x += 30;
			caret.y += 16;
		}
	}
	caret = push_caret;
}

static void paint_debug_mouse(const provincei* p) {
	textcn(str("%1i, %2i", p->position.x, p->position.y));
}

static void paint_background() {
	auto pi = gres("terrain", "art/objects");
	image(pi, 0, 0);
}

void provincei::paint() const {
	paint_background();
	paint_player_banner(player);
	//paint_hexagon();
	paint_player_markers(this);
}

static void texthd(const char* format) {
	auto push_caret = caret;
	caret.x += (width - textw(format)) / 2;
	text(format);
	caret = push_caret;
	caret.y += texth() + metrics::padding * 2;
}

static void texth2(const char* format) {
	auto push_font = font;
	font = metrics::h2;
	texthd(format);
	font = push_font;
}

static void texth2cw(const char* format) {
	auto push_font = font;
	font = metrics::h2;
	auto push_caret = caret;
	caret.x = (getwidth() - textw(format)) / 2;
	text(format);
	caret = push_caret;
	caret.y += texth() + metrics::padding * 2;
	font = push_font;
}

static void textv(const char* format, ...) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	textcn(temp);
}

static void buttonback(int size, const void* data) {
	auto push_alpha = alpha;
	rectpush push;
	width = size * 2;
	height = button_height - 1;
	caret.y -= (height + 1) / 2;
	caret.x -= (width + 1) / 2;
	alpha = (192 * push_alpha) >> 8;
	rectf();
	alpha = push_alpha;
	rectb();
	if(data && ishilite()) {
		hilite_object = data;
		//if(hot.key == MouseLeft && !hot.pressed)
		//	execute(buttonparam, (long)data);
	}
}

void troopi::paint() const {
	auto push_color = fore;
	fore = getplayercolor(player);
	buttonback(button_width, this);
	fore = push_color;
	textcn(getname());
}

static void windowed(const char* format) {
	rectpush push;
	swindow(false);
	textf(format);
}

static void background(figure shape, int size) {
	auto push_fore = fore;
	fore = colors::black;
	switch(shape) {
	case figure::Rect: field(figure::RectFill, size); break;
	case figure::Circle: field(figure::CircleFill, size); break;
	}
	fore = push_fore;
	field(shape, size);
}

static void textvalue(figure shape, const char* format) {
	background(shape, 12);
	textcn(format);
}

static void textvalue(figure shape, int v) {
	if(!v)
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1i", v);
	textvalue(shape, temp);
}

static void paint_structure() {
	auto p = (structure*)last_object->data;
	if(!p->player)
		return;
	auto i = ((structurei*)p->id)->avatar;
	auto r = gres("buildings", "art/fonts");
	auto push_fore = fore;
	fore = p->player->fore;
	image(caret.x, caret.y, r, i, 0);
	fore = push_fore;
	ishilite(16, last_object->data);
}

template<> void updateui<provincei>(provincei* p) {
	auto pt = draw::h2p(p->position, size);
	auto ps = findobject(p);
	if(!ps)
		ps = addobject(pt, p, ftpaint<provincei>, 0, 23);
}

static void marker_press() {
	auto p = (object*)hparam;
	breakmodal((long)p->data);
}

static void paint_green_marker() {
	auto push_fore = fore;
	auto push_alpha = alpha;
	fore = colors::green;
	alpha = 16;
	auto radius = last_object->param;
	if(ishilite(radius - 4, last_object->data)) {
		if(hpressed)
			alpha = 32;
		else
			alpha = 42;
		hcursor = cursor::Hand;
		if(hkey == MouseLeft && !hpressed)
			execute(marker_press, (long)last_object);
	}
	circlef(radius);
	alpha = 255;
	circle(radius);
	alpha = push_alpha;
	fore = push_fore;
}

static void remove_all(fnevent proc) {
	for(auto& e : bsdata<object>()) {
		if(e.painting == proc)
			e.clear();
	}
}

static void add_maker(void* object, fnevent proc, int size) {
	auto pm = findobject(object);
	if(pm)
		addobject(pm->position, object, proc, size, 50);
}

provincei* entitya::chooseprovince() const {
	auto total_count = count;
	if(total_count == 1)
		return (provincei*)data[0];
	else if(total_count == 0)
		return 0;
	for(auto p : *this)
		add_maker(p, paint_green_marker, size / 3);
	auto result = (provincei*)scene(0);
	remove_all(paint_green_marker);
	return result;
}

void entity::focusing() const {
	if(!this)
		return;
	auto ps = getprovince();
	if(ps)
		slide_camera(h2p(ps->position, size), 48);
}

static void update_units(point position, const entity* location) {
	if(!location)
		return;
	entitya source;
	source.select(location);
	source.sortunits();
	if(!source)
		return;
	auto y0 = position.y;
	auto n = source.getplayercount();
	auto x = position.x - (n - 1) * (button_width * 2 + 2) / 2;
	auto y = position.y;
	playeri* nv = 0;
	for(auto pt : source) {
		if(pt->player != nv) {
			if(nv)
				x += button_width * 2 + 2;
			y = y0;
			nv = pt->player;
		}
		auto p = findobject(pt);
		if(!p)
			p = addobject({(short)x, (short)y}, (troopi*)pt, ftpaint<troopi>, 0, 30);
		if(p->position.x != x || p->position.y != y) {
			auto po = p->addorder(1000);
			po->position.x = x;
			po->position.y = y;
		}
		y += button_height;
	}
}

static void update_buildings(point position, const entity* location) {
	if(!location)
		return;
	auto index = 0;
	for(auto& e : bsdata<structure>()) {
		if(e.location != location)
			continue;
		point np = position + province_points[index++];
		auto p = findobject(&e);
		if(!p)
			p = addobject(np, &e, paint_structure, 0, 20);
		else
			p->position = np;
	}
}

void prepare_game_ui() {
	clear_objects();
	for(auto& e : bsdata<provincei>())
		updateui(&e);
}

void update_ui() {
	static point system_offset = {0, -6 * size / 10};
	wait_all();
	for(auto& e : bsdata<object>()) {
		if(bsdata<provincei>::have(e.data)) {
			update_units(e.position + system_offset, (entity*)e.data);
			update_buildings(e.position, (entity*)e.data);
		}
	}
	wait_all();
}

static void transparent_window() {
	rectpush push;
	auto push_alpha = alpha;
	auto push_fore = fore;
	setoffset(-metrics::border, -metrics::border);
	fore = colors::form;
	alpha = 128;
	rectf();
	alpha = push_alpha;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

static void paint_message_string() {
	if(!message_string)
		return;
	rectpush push;
	textfs(message_string);
	caret.x = (getwidth() - width) / 2;
	caret.y += metrics::border * 3;
	transparent_window();
	textf(message_string);
}

static void main_background() {
	strategy_background();
	paint_objects();
	paint_message_string();
}

static void main_finish() {
	input_camera();
}

static void textct(const char* format) {
	rectpush push;
	setoffset(4, 4);
	texta(format, AlignCenterCenter);
}

static void window_frame(color v, const entity* pe, const char* format) {
	rectpush push;
	auto push_fore = fore;
	fore = v;
	auto push_alpha = alpha;
	alpha = 192;
	//if(ishilite(pe)) {
	//	alpha -= 32;
	//	if(hot.pressed)
	//		alpha += 64;
	//}
	rectf();
	alpha = push_alpha;
	rectb();
	fore = push_fore;
	setoffset(metrics::border, metrics::border);
	if(format)
		textf(format);
}

static void window_frame(color v, const entity* pe) {
	window_frame(v, pe, getinform(pe));
}

static void paint_querry() {
	rectpush push; width = 280; height = 100;
	auto origin = caret;
	for(auto p : querry) {
		window_frame(colors::window, p);
		caret.y += height + metrics::border;
		if(caret.y + height > clipping.y2) {
			caret.x = origin.x += width + metrics::border;
			caret.y = origin.y;
		}
	}
}

static void open_objectives() {
	querry.select(bsdata<decki>::elements + ObjectivesDeck, 0, 0);
	paint_querry();
}

static void tactic_form() {
	querry.select(bsdata<decki>::elements + TacticsDeck, 0, player);
	paint_querry();
}

static void open_upgrades() {
	querry.collectiona::select(bsdata<upgradei>::source, filter_player_upgrade, true);
	paint_querry();
}

void initialize_ui() {
	pbackground = main_background;
	pfinish = main_finish;
}

BSDATA(widget) = {
	{"UpgradesForm", open_upgrades},
	{"OpenObjectives", open_objectives},
	{"TacticForm", tactic_form},
};
BSDATAF(widget)