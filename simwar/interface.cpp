#include "crt.h"
#include "draw.h"
#include "resource.h"

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
	rectpush push;
	for(auto& e : bsdata<resourcei>())
		field(e.format, 100, 1, -1);
}