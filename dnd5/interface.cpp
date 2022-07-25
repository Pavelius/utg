#include "draw.h"
#include "main.h"
#include "widget.h"

using namespace draw;

static variant ability;
static void* object;

static void texts(const char* format, unsigned state) {
	texta(format, state);
	caret.y += texth() + metrics::padding;
}

static void texth2(const char* format, unsigned state) {
	auto push_font = font;
	font = metrics::h2;
	texts(format, state);
	font = push_font;
}

static void texth2(int value, unsigned state) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	texth2(temp, state);
}

static void group(const menu& source) {
	auto pw = bsdata<widget>::find(source.id);
	if(!pw)
		return;
	auto push_ability = ability;
	for(auto& e : source.elements) {
		ability = e;
		pw->paint();
	}
	ability = push_ability;
}

static void ability_box() {
	if(!ability || !object)
		return;
	auto p = (character*)object;
	texts(ability.getname(), AlignCenter);
	texth2(p->abilitites[ability.value], AlignCenter);
}

static void sheet_window() {

}

void ui_initialize() {
	widget::add("AbilityBox", ability_box);
}