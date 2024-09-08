#include "answers.h"
#include "bsreq.h"
#include "draw.h"
#include "draw_figure.h"
#include "draw_strategy.h"
#include "log.h"
#include "variant.h"

using namespace draw;

static int window_width = 400, window_height = 400;

void set_dark_theme();
void set_light_theme();
void initialize_png();

static void beforemodal() {
	tips_caret.x = metrics::padding + metrics::border;
	tips_caret.y = getheight() - (metrics::padding + metrics::border);
	tips_size.x = getwidth() - (metrics::padding + metrics::border) * 2;
}

static void movedown(int dy) {
	caret.y += dy; height -= dy;
}

static void linedown() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	line(caret.x + width, caret.y);
	fore = push_fore;
	caret = push_caret;
}

void status_info();

static void statusbar() {
	status_info();
	if(caret) {
		linedown();
		draw::caret.y += 1;
		height -= draw::caret.y;
	}
}

bool draw::swindow(bool hilight) {
	rectpush push;
	auto push_alpha = alpha;
	auto push_fore = fore;
	setoffset(-metrics::border, -metrics::border);
	auto rs = ishilite();
	fore = colors::form;
	if(hilight && rs)
		fore = colors::button.mix(fore, hpressed ? 128 : 216);
	alpha = 220;
	rectf();
	alpha = push_alpha;
	fore = colors::border;
	rectb();
	fore = push_fore;
	return rs;
}

bool draw::ishilite(int size, const void* object) {
	if(!ishilite({caret.x - size, caret.y - size, caret.x + size, caret.y + size}))
		return false;
	hilite_object = object;
	return true;
}

bool draw::ishilite(const void* object) {
	if(!ishilite())
		return false;
	hilite_object = object;
	return true;
}

static bool window(bool hilite, const char* string, const char* resid, const char* prompt) {
	if((!string || string[0] == 0) && !resid && (!prompt || prompt[0] == 0))
		return false;
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
	auto rs = swindow(hilite);
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
	return rs;
}

static bool menufd(const char* title) {
	if(!title)
		return false;
	auto push_width = width;
	textas(title);
	width = push_width;
	auto result = swindow(true);
	texta(title, AlignCenterCenter);
	width = push_width;
	height += metrics::border * 2;
	return result;
}

static bool menuft(const char* title) {
	if(!title)
		return false;
	auto push_caret = caret;
	auto push_width = width;
	textfs(title);
	width = push_width;
	auto result = swindow(true);
	textf(title);
	width = push_width;
	height += metrics::border * 2;
	caret = push_caret;
	return result;
}

static void tipswindow() {
	fore = colors::tips::back;
	strokeout(rectf);
	fore = colors::border;
	strokeout(rectb);
	fore = colors::tips::text;
}

static bool menurd(const char* title) {
	if(!title)
		return false;
	auto push_caret = caret;
	auto push_height = height;
	textfs(title);
	auto result = swindow(false);
	textf(title);
	height = push_height;
	width += metrics::border * 2;
	caret = push_caret;
	return result;
}

void draw::texth2w(const char* string) {
	auto push_font = font;
	auto push_height = height;
	font = metrics::h2;
	height = texth();
	swindow(false);
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

static void strategy_answers_beforepaint() {
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

void draw::strategy_background() {
	fillform();
	statusbar();
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
		}
	}
	if(tips_sb) {
		rectpush push;
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

int draw::strategy(fnevent proc, fnevent afterread, const char* rules_url) {
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
	if(afterread)
		afterread();
	if(log::errors > 0)
		return -1;
	pbeforemodal = beforemodal;
	if(!pbackground)
		pbackground = strategy_background;
	answers::beforepaint = strategy_answers_beforepaint;
	answers::paintcell = menubt;
	ptips = tips;
	awindow.flags = WFResize | WFMinmax;
	initialize(getnm("AppTitle"));
	setnext(proc);
	start();
	return 0;
}