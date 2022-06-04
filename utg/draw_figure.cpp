#include "draw.h"
#include "draw_figure.h"

figure	draw::hilite_type;

void draw::field(figure type, int size) {
	rectpush push;
	int push_size;
	switch(type) {
	case figure::Check:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y - size + 2;
		line(push.caret.x, push.caret.y + size);
		line(push.caret.x + size, push.caret.y - size);
		break;
	case figure::Circle:
		circle(size);
		break;
	case figure::CircleFill:
		circlef(size);
		circle(size);
		break;
	case figure::Rect:
		setpos(push.caret.x - size, push.caret.y - size, size * 2, size * 2);
		rectb();
		break;
	case figure::Rect3D:
		setpos(push.caret.x - size, push.caret.y - size, size * 2, size * 2);
		rectb3d();
		break;
	case figure::RectFill:
		setpos(push.caret.x - size, push.caret.y - size, size * 2, size * 2);
		rectf();
		break;
	case figure::TrianlgeUp:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y - size;
		line(push.caret.x + size, push.caret.y - size);
		line(push.caret.x, push.caret.y + size);
		line(push.caret.x - size, push.caret.y - size);
		break;
	case figure::Trianlge:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y + size;
		line(push.caret.x + size, push.caret.y + size);
		line(push.caret.x, push.caret.y - size);
		line(push.caret.x, push.caret.y + size);
		break;
	case figure::Close:
		caret.x = push.caret.x - size;
		caret.y = push.caret.y - size;
		line(push.caret.x + size, push.caret.y + size);
		caret.x = push.caret.x - size;
		caret.y = push.caret.y + size;
		line(push.caret.x + size, push.caret.y - size);
		break;
	case figure::Cross:
		caret.x = push.caret.x - size;
		caret.x = push.caret.y - size;
		line(push.caret.x + size, push.caret.y + size);
		caret.x = push.caret.x + size;
		caret.x = push.caret.y - size;
		line(push.caret.x - size, push.caret.y + size);
		break;
	case figure::Hexagon:
		push_size = fsize;
		fsize = size;
		hexagon();
		fsize = push_size;
		break;
	case figure::FHexagon:
		push_size = fsize;
		fsize = size;
		fhexagon();
		fsize = push_size;
		break;
	case figure::HexagonFill:
		break;
	default:
		break;
	}
}

void draw::field(figure type, int size, const char* format) {
	field(type, size);
	if(format) {
		auto push_caret = caret;
		caret.x -= textw(format) / 2;
		caret.y += size + 2;
		text(format);
		caret = push_caret;
	}
}

void draw::painthilite() {
	if(hilite_type==figure::None || !hilite_size)
		return;
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::active;
	caret = hilite_position;
	field(hilite_type, hilite_size);
	hot.cursor = cursor::Hand;
	fore = push_fore;
	caret = push_caret;
}