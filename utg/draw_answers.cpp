#include "answers.h"
#include "draw.h"

using namespace draw;

static int getcolumns(const answers& an) {
	auto divider = an.getcount() % 2;
	if(an.getcount() <= 14)
		return 1;
	for(auto& e : an) {
		auto len = zlen(e.text);
		if(len > 20)
			return 1;
	}
	return 2;
}

void answers::paintanswers(int columns, const char* cancel_text) const {
	auto column_width = draw::width;
	if(columns > 1)
		column_width = column_width / columns - metrics::border;
	auto next_column = (elements.getcount() + columns - 1) / columns;
	auto index = 0;
	auto y1 = caret.y, x1 = caret.x;
	auto y2 = caret.y;
	auto push_width_normal = width;
	width = column_width;
	for(auto& e : elements) {
		paintcell(index, e.value, e.text);
		if(caret.x > x1 + column_width - 64) {
			caret.x = x1;
			caret.y += height + metrics::padding + metrics::border * 2;
		}
		if(caret.y > y2)
			y2 = caret.y;
		if((index % next_column) == next_column - 1) {
			caret.y = y1;
			caret.x += column_width + metrics::border * 2;
		}
		index++;
	}
	caret.x = x1; caret.y = y2;
	width = push_width_normal;
	if(cancel_text) {
		auto push_fore = fore;
		fore = fore.mix(colors::h3, 128);
		paintcell(elements.getcount(), 0, cancel_text);
		fore = push_fore;
	}
}

void* answers::choose(const char* title, const char* cancel_text, int cancel_mode) const {
	if(!interactive)
		return random();
	if(!elements) {
		if(!cancel_mode || (cancel_mode && cancel_text == 0))
			return 0;
	}
	auto columns = column_count;
	if(columns == -1)
		columns = getcolumns(*this);
	auto push_title = prompa;
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	prompa = title;
	while(ismodal()) {
		paintstart();
		if(beforepaint)
			beforepaint();
		paintanswers(columns, cancel_text);
		if(afterpaint)
			afterpaint();
		width = push_width;
		caret = push_caret;
		paintfinish();
		domodal();
	}
	prompa = push_title;
	return (void*)getresult();
}