#include "draw.h"
#include "stringbuilder.h"

using namespace draw;

static void tooltips_render() {
	auto push_fore = draw::fore;
	pushrect push;
	tipspos();
	fore = colors::tips::back;
	setoffset(-metrics::border, -metrics::border);
	rectf();
	fore = colors::border;
	rectb();
	setoffset(metrics::border, metrics::border);
	// Show text
	fore = colors::tips::text;
	textf(tips_sb);
	fore = push_fore;
}

void draw::tooltipspaint() {
	if(!draw::font)
		return;
	if(!tips_sb)
		return;
	if(draw::hkey != InputIdle)
		return;
	tooltips_render();
}