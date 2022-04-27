#include "drawobject.h"
#include "main.h"

using namespace draw;

static void add_worldmap() {
	auto p = addobject(0, 0);
	p->resource = getres("worldmap");
}

static void add_monsters() {
}

static void add_investigators() {
}

static void add_gates() {
}

static void add_clues() {
}

static void create_figures() {
	clearobjects();
	add_worldmap();
	add_monsters();
	add_investigators();
	add_gates();
	add_clues();
}

void main_menu() {
	create_figures();
	draw::chooseobject();
}