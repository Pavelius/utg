#include "answers.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

static void add_worldmap() {
	auto p = addobject(0, 0);
	p->resource = getbackground("worldmap");
	p->priority = 1;
}

static void add_monsters() {
	auto p = addobject(764, 128);
	p->resource = getres("byakhee");
	p->priority = 90;
}

static void add_gates() {
	auto p = addobject(764, 108);
	p->resource = getres("gates");
	p->frame = 1;
	p->priority = 10;
}

static void add_clues() {
}

void update_board() {
	clearobjects();
	add_worldmap();
	add_monsters();
	add_gates();
	add_clues();
}

void status_info() {
}