#include "collection.h"
#include "code_package.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"

using namespace code;
using namespace draw;

static collection<symbol> symbols;
static int current;

void table_text_icon();

static bool symbol_visible(const void* object) {
	auto p = (symbol*)object;
	auto pn = last_package->getstr(p->id);
	if(p->ispointer())
		return false;
	if(p->is(Standart))
		return false;
	return true;
}

void update_code_tree() {
	symbols.collectiona::select(last_package->symbols, symbol_visible, true);
}

static int get_icon(symbol* p) {
	if(p->parent == Modules)
		return 1;
	return 0;
}

static void add_name(stringbuilder& sb, const symbol* p) {
	sb.add(last_package->getstr(p->id));
	if(p->is(Function)) {
		sb.add("(");
		sb.add(")");
	}
	if(p->parent == This && p->result != Void && p->result != None) {
		auto parent = last_package->getsym(p->result);
		if(parent) {
			sb.adds(":");
			sb.adds(last_package->getstr(parent->id));
		}
	}
}

static const char* main_getname(const void* object, stringbuilder& sb) {
	auto ps = *((symbol**)object);
	add_name(sb, ps);
	gui.number = get_icon(ps);
	return sb.begin();
}

symbol* get_active_symbol() {
	return symbols[current];
}

static void code_tree_widget() {
	static int origin;
	rectpush push;
	showborder();
	partoffset(1, 1);
	showbackground();
	draw::list(origin, current, texth() + 2, table_text_icon);
}

void view_code_tree() {
	auto push = gui;
	list_data.data = symbols.data;
	list_data.size = sizeof(symbol*);
	list_data.count = symbols.getcount();
	list_data.mask = 0;
	gui.pgetname = main_getname;
	code_tree_widget();
	gui = push;
}