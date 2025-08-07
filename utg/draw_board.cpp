#include "answers.h"
#include "bsreq.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_figure.h"
#include "log.h"
#include "variant.h"

using namespace draw;

static int window_width = 400, window_height = 400;

void set_dark_theme();
void set_light_theme();
void initialize_png();

static const char* getnms(const char* id) {
	auto p = getnme(ids(id, "Short"));
	if(!p)
		p = getnm(id);
	return p;
}

static void movey(int d) {
	caret.y += d; height -= d;
}

static void linedown() {
	pushfore push_fore(colors::border);
	auto push_caret = caret;
	line(caret.x + width, caret.y);
	caret = push_caret;
}

static void lineright() {
	pushfore push_fore(colors::border);
	auto push_caret = caret;
	caret.x += width;
	line(caret.x, caret.y + height);
	caret = push_caret;
}

static void paint_window(bool hilight) {
	pushrect push;
	pushfore push_fore(colors::form);
	setoffset(-metrics::border, -metrics::border);
	button_hilited = ishilite();
	if(hilight && button_hilited)
		fore = colors::button.mix(fore, hpressed ? 128 : 216);
	auto push_alpha = alpha;
	alpha = 220;
	rectf();
	alpha = push_alpha;
	fore = colors::border;
	rectb();
}

bool ishilite(int size, const void* object) {
	if(!ishilite({caret.x - size, caret.y - size, caret.x + size, caret.y + size}))
		return false;
	hilite_object = object;
	return true;
}

bool ishilite(const void* object) {
	if(!ishilite())
		return false;
	hilite_object = object;
	return true;
}

static void window(bool hilite, const char* string, const char* resid, const char* prompt) {
	button_hilited = false;
	if((!string || string[0] == 0) && !resid && (!prompt || prompt[0] == 0))
		return;
	auto text_height = 0;
	auto image_height = 0;
	const sprite* image_surface = 0;
	if(string) {
		auto push_width = width;
		textfs(string);
		width = push_width;
		text_height += height;
	}
	if(prompt) {
		auto push_width = width;
		textfs(prompt);
		width = push_width;
		text_height += height;
	}
	if(resid) {
		image_surface = gres(resid, "art/images");
		if(image_surface)
			image_height = image_surface->get(0).sy;
	}
	auto padding_height = 0;
	if(image_height && text_height)
		padding_height = metrics::padding;
	auto push_height = height;
	height = image_height + text_height + padding_height;
	paint_window(hilite);
	if(image_surface) {
		image(caret.x, caret.y, image_surface, 0, 0);
		caret.y += image_height + padding_height;
	}
	height = push_height;
	if(string)
		textf(string);
	if(prompt) {
		auto push_fore = fore;
		fore = colors::h3;
		textf(prompt);
		fore = push_fore;
	}
	caret.y += metrics::border * 2;
}

static bool menufd(const char* title) {
	if(!title)
		return false;
	auto push_width = width;
	textas(title);
	width = push_width;
	paint_window(true);
	texta(title, AlignCenterCenter);
	width = push_width;
	height += metrics::border * 2;
	return button_hilited;
}

static bool menuft(const char* title) {
	if(!title)
		return false;
	auto push_caret = caret;
	auto push_width = width;
	textfs(title);
	width = push_width;
	paint_window(true);
	textf(title);
	width = push_width;
	height += metrics::border * 2;
	caret = push_caret;
	return button_hilited;
}

static void tipswindow() {
	fore = colors::tips::back;
	strokeout(rectf);
	fore = colors::border;
	strokeout(rectb);
	fore = colors::tips::text;
}

static void menurd(const char* title) {
	if(!title)
		return;
	auto push_caret = caret;
	auto push_height = height;
	textfs(title);
	paint_window(false);
	textf(title);
	height = push_height;
	width += metrics::border * 2;
	caret = push_caret;
}

void texth2w(const char* string) {
	auto push_font = font;
	auto push_height = height;
	font = metrics::h2;
	height = texth();
	paint_window(false);
	auto push_fore = fore;
	fore = colors::h2;
	texta(string, AlignCenter);
	fore = push_fore;
	caret.y += texth() + metrics::border * 2 + metrics::padding;
	height = push_height;
	font = push_font;
}

static void menubt(int i, const void* pv, const char* title, fnevent press_event) {
	if(i >= (int)(sizeof(answers::hotkeys) / sizeof(answers::hotkeys[0])))
		i = sizeof(answers::hotkeys) / sizeof(answers::hotkeys[0]) - 1;
	auto proc = menufd;
	if(title[0] == '#') {
		title++;
		proc = menuft;
	}
	if(button(title, answers::hotkeys[i], proc)) {
		if(press_event)
			execute(press_event, (long)pv);
	}
	if(control_hilited) {
		if(answers::show_tips)
			hilite_object = pv;
	} else if(hilite_object) {
		if(pv == hilite_object)
			hcursor = cursor::Hand;
	}
}

static void paint_options() {
	width = 320;
	caret.y += metrics::padding + metrics::border;
	caret.x = getwidth() - width - (metrics::padding + metrics::border);
	if(answers::header)
		texth2w(answers::header);
	if(answers::prompt || answers::prompa || answers::resid) {
		window(false, answers::prompt, answers::resid, answers::prompa);
		caret.y += metrics::padding;
	}
}

static void tipsposition(const char* format) {
	textfs(format);
	auto x2 = getwidth(), y2 = getheight();
	auto y = hilite.bottom();
	if(y + height > y2 || y > y2 - (y2 / 4))
		y = y - height - metrics::border * 3;
	else
		y = hilite.bottom() + metrics::border * 2 + metrics::padding;
	auto x = hilite.right();
	if(x + width > x2 || x > x2 - (x2 / 3)) {
		x = hilite.left() - width - metrics::border - metrics::padding;
		y = hilite.top();
	} else
		x = hilite.left() + metrics::border;
	caret.x = x;
	caret.y = y;
}

static void paint_tips() {
	auto tips = tips_sb.begin();
	if(tips[0] == '@') {
		auto object = variant(tips + 1);
		if(object) {
			hilite_object = object.getpointer();
			tips_sb.clear();
		}
	}
	if(hilite_object && !tips_sb) {
		variant v = hilite_object;
		if(!v)
			return;
		auto pv = v.getpointer();
		auto pm = bsdata<varianti>::elements + v.type;
		if(pm->pstatus)
			pm->pstatus(pv, tips_sb);
		if(!tips_sb && pm->isnamed()) {
			auto id = *((const char**)pv);
			auto idn = getnme(ids(id, "Info"));
			if(idn) {
				tips_sb.add("###%1", getnm(id));
				tips_sb.addn("---");
				tips_sb.addn(idn);
			}
		}
	}
	if(tips_sb) {
		pushrect push;
		width = 400;
		tipsposition(tips_sb.begin());
		tipswindow();
		textf(tips_sb.begin());
	}
}

static void tips() {
	paint_hilite_figure();
	paint_tips();
}

void board_tool_panel(int dx, int dy) {
	height = dy;
	gradv(colors::border.mix(colors::form, 128), colors::form);
	auto push_caret = caret;
	caret.y += dy;
	pushfore push_fore(colors::border);
	line(caret.x + width, caret.y);
	caret = push_caret;
	width = dx;
}

void board_tool_button(const char* id, const char* value) {
	pushfore push_fore(colors::h3);
	auto push_font = font;
	auto push_caret = caret;
	auto push_height = height;
	lineright();
	auto header = getnm(id);
	font = metrics::small;
	texta(header, AlignCenter);
	movey(texth());
	linedown();
	font = metrics::h2;
	fore = colors::text;
	texta(value, AlignCenterCenter);
	font = push_font;
	height = push_height;
	caret = push_caret;
	caret.x += width;
}

int board_game_run(fnevent proc, const char* rules_url) {
	initialize_png();
	if(!proc)
		return -1;
	set_dark_theme();
	metrics::border = 5;
	metrics::padding = 1;
	if(!rules_url)
		rules_url = "rules/Basic.txt";
	bsreq::read(rules_url);
	initialize_translation();
	if(log::errors > 0)
		return -1;
	ptips = tips;
	awindow.flags = WFResize | WFMinmax;
	initialize(getnm("AppTitle"));
	setnext(proc);
	start();
	return 0;
}