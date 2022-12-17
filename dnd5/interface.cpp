#include "ability.h"
#include "character.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_figure.h"
#include "menu.h"
#include "skill.h"
#include "draw_strategy.h"
#include "variant.h"
#include "widget.h"

using namespace draw;

static variant ability;

static void texth2(const char* format, unsigned state) {
	auto push_font = font;
	font = metrics::h2;
	texta(format, state);
	caret.y += texth() + metrics::padding;
	font = push_font;
}

static void texth2(int value, unsigned state) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	texth2(temp, state);
}

static void paint(const menu& source) {
	auto pw = bsdata<widget>::find(source.format);
	if(!pw)
		return;
	auto push_ability = ability;
	for(auto& e : source.elements) {
		ability = e;
		pw->paint();
	}
	ability = push_ability;
}

static void small_header(const char* format) {
	auto push_font = font;
	draw::font = metrics::small;
	texta(format, AlignCenter);
	caret.y += texth() + metrics::padding;
	font = push_font;
}

static void small_short(const char* id) {
	auto pn = getnme(str("%1Short", id));
	if(!pn)
		pn = getnm(id);
	small_header(pn);
}

static void window_border(int total_height) {
	auto push_height = height;
	height = total_height;
	strokeout(strokeborder);
	height = push_height;
	caret.y += total_height + metrics::padding * 2 + metrics::border;
}

static void small_bonus_box(int total_height, int bonus) {
	rectpush push;
	height = width = 16;
	caret.x += 56;
	caret.y += (total_height - height) / 2;
	strokeout(strokeborder);
	auto push_fore = fore;
	if(bonus < 0)
		fore = colors::red;
	else if(bonus > 0)
		fore = colors::green;
	texta(str("%1i", bonus), AlignCenterCenter);
	fore = push_fore;
}

static void ability_box_widget() {
	if(ability.iskind<abilityi>()) {
		caret.y += metrics::padding;
		auto push_caret = caret;
		small_short(ability.getid());
		texth2(player->abilitites[ability.value], AlignCenter);
		auto total_height = caret.y - push_caret.y;
		caret = push_caret;
		small_bonus_box(total_height, player->abilitites[ability.value] / 2 - 5);
		window_border(total_height);
	}
}

static void small_check(bool check) {
	auto push_caret = caret;
	auto push_fore = fore;
	auto size = texth();
	fore = colors::border;
	caret.x += size / 2;
	caret.y += size / 2 + 1;
	field(figure::Rect, size / 2);
	if(check)
		field(figure::Check, size);
	caret = push_caret;
	fore = push_fore;
	caret.x += size + metrics::padding * 2;
}

static void skill_box_widget() {
	if(ability.iskind<skilli>()) {
		auto push_caret = caret;
		text(str("%1i", player->getskill(ability.value))); caret.x += 20;
		text(ability.getname());
		caret = push_caret;
		caret.y += texth() + metrics::padding;
	}
}

static void padding_box() {
}

static void group_horizontal() {
	auto push_width = width;
	auto push_caret = caret;
	width = width * ability.counter / 100;
	caret.x += metrics::padding; width -= metrics::padding;
	if(ability.iskind<menu>())
		paint(bsdata<menu>::elements[ability.value]);
	else if(ability.iskind<widget>())
		bsdata<widget>::elements[ability.value].paint();
	caret.y = push_caret.y;
	caret.x += width + metrics::padding * 2;
	width = push_width;
}

static void separator() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x -= metrics::border;
	caret.y += metrics::padding / 2;
	line(caret.x + width + metrics::border * 2 - 1, caret.y);
	caret = push_caret;
	fore = push_fore;
	caret.y += metrics::padding;
}

static void rightline() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += width;
	caret.y -= metrics::padding - 1;
	line(caret.x, caret.y + height + metrics::padding * 2);
	fore = push_fore;
	caret = push_caret;
}

static void tabs(const variants& elements) {
	rectpush push;
	caret.x -= metrics::padding;
	height = texth() + metrics::padding * 2;
	for(auto& e : elements) {
		auto pn = e.getname();
		width = textw(pn) + metrics::padding * 2;
		if(width > 160)
			width = 160;
		rightline();
		texta(pn, AlignCenterCenter);
		ishilite(&e);
		caret.x += width;
	}
}

static void footer(const variants& elements) {
	auto push_caret = caret;
	caret.y = caret.y + height - metrics::padding * 3 - texth();
	separator();
	tabs(elements);
	caret = push_caret;
}

static void character_sheet() {
	auto pm = bsdata<menu>::find(lastwidget->id);
	if(!pm)
		return;
	rectpush push;
	width = 320; height = 400;
	caret.x = metrics::padding + metrics::border;
	caret.y = 1 + metrics::padding + metrics::border;
	draw::swindow(false);
	footer(pm->elements);
	paint(*pm);
}

static void background_map() {
	auto pi = gres("hills", "maps");
	image(pi, 0, 0);
}

static void add_widget(const char* id, unsigned char priority, bool absolute_position = true) {
	auto pm = bsdata<widget>::find(id);
	if(!pm)
		return;
	auto p = addobject(0, 0);
	p->data = pm;
	p->priority = priority;
	if(absolute_position)
		p->set(drawable::AbsolutePosition);
}

static void object_painting(const object* p) {
	if(bsdata<widget>::have(p->data))
		((widget*)p->data)->paint();
}

void ui_initialize() {
	object::painting = object_painting;
	object::initialize();
	widget::add("BackgroundMap", background_map);
	widget::add("AbilityBox", ability_box_widget);
	widget::add("PaddingBox", padding_box);
	widget::add("CharacterSheet", character_sheet);
	widget::add("GroupHorizontal", group_horizontal);
	widget::add("Separator", separator);
	widget::add("SkillBox", skill_box_widget);
	add_widget("CharacterSheet", 50);
	add_widget("BackgroundMap", 0, false);
}