#include "crt.h"
#include "draw.h"
#include "player.h"

using namespace draw;

static void paint_vborder() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	line(caret.x, caret.y + texth());
	fore = push_fore;
	caret = push_caret;
	caret.x += metrics::padding + 1;
}

static void field(const char* format, int minimal_width) {
	caret.x += metrics::padding * 2;
	auto push_width = width;
	auto push_height = height;
	auto push_caret = caret;
	textfs(format);
	caret.x += (minimal_width - width) / 2;
	textf(format);
	caret = push_caret;
	caret.x += minimal_width + metrics::padding * 4;
	width = push_width;
	height = push_height;
}

static void paint_cost(const costable& v, const costable& u) {
	char temp[260]; stringbuilder sb(temp);
	for(auto i = 0; i <= Gems; i++) {
		auto& e = bsdata<costi>::elements[i];
		sb.clear();
		if(v.cost[i])
			sb.add("%1i", v.cost[i]);
		if(u.cost[i])
			sb.add("%+1i", u.cost[i]);
		if(!temp[0])
			continue;
		auto minimal_width = 60;
		if(e.width)
			minimal_width = e.width;
		field(temp, minimal_width);
		paint_vborder();
	}
}

void status_info() {
	auto push_caret = caret;
	paint_cost(*player, player->upkeep);
	caret = push_caret;
	caret.y += texth() + metrics::padding;
}