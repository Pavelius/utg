#include "crt.h"
#include "draw.h"
#include "main.h"

using namespace draw;

static void field(const char* format, int v1, int v2, int v3) {
	if(!format)
		return;
	char temp[128]; stringbuilder sb(temp);
	sb.add(format, v1, v2, v3);
	auto push_y = caret.y;
	caret.x += metrics::padding;
	textfs(temp);
	textf(temp);
	caret.x += width + metrics::padding;
	caret.y = push_y;
}

void status_info() {
	if(true) {
		rectpush push;
		for(auto& e : bsdata<stati>()) {
			switch(e.format) {
			case 1:
				field("%1i", 100, 1, -1);
				break;
			default:
				field("%1i+%2i", 100, 4, -1);
				break;
			}
		}
	}
	caret.y += texth() + metrics::padding;
}