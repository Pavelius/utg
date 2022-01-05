#include "answers.h"
#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "utg.h"

using namespace draw;

void set_dark_theme();
void set_light_theme();

bool draw::buttonfd(const char* title) {
	auto push_caret = caret;
	textfs(title);
	textf(title);
	caret = push_caret;
	auto hilited = ishilite();
	if(hilited)
		hot.cursor = cursor::Hand;
	return hilited;
}

static void stroke() {
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

static void strokeout() {
	rectpush push;
	caret.x -= 1;
	caret.y -= 1;
	width += 1;
	height += 1;
	stroke();
}

static void stroke(const sprite* p, int frame) {
	auto push_width = width;
	auto push_height = height;
	auto& f = p->get(0);
	width = f.sx;
	height = f.sy;
	strokeout();
	width = push_width;
	height = push_height;
}

static void imagev(const char* resid) {
	if(!resid)
		return;
	auto p = gres(resid, "art/images");
	if(!p)
		return;
	image(p, 0, 0);
	stroke(p, 0);
	caret.y += p->get(0).sy;
}

static void radiobutton(const char* title) {
	auto push_fore = fore;
	fore = fore.mix(colors::form, 192);
	text(title);
	auto dx = textw('0') * 3;
	caret.x += dx; width -= dx;
	fore = push_fore;
}

static void hotkeybutton(char key) {
	char temp[16]; stringbuilder sb(temp);
	sb.add(key);
	sb.add(")");
	radiobutton(temp);
}

void draw::answerbt(int i, const void* pv, const char* title) {
	static char answer_hotkeys[] = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
		'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K'
	};
	if(i >= (int)(sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0])))
		i = sizeof(answer_hotkeys) / sizeof(answer_hotkeys[0]) - 1;
	auto push_caret = caret;
	auto push_width = width;
	hotkeybutton(answer_hotkeys[i]);
	if(button(title, answer_hotkeys[i], buttonfd))
		execute(buttonparam, (long)pv);
	caret.x = push_caret.x;
	width = push_width;
}

void draw::texth2(const char* title) {
	if(!title)
		return;
	auto push_font = font;
	auto push_height = height;
	font = metrics::h2;
	height = texth();
	auto push_fore = fore;
	fore = colors::h2;
	texta(title, AlignCenter);
	fore = push_fore;
	caret.y += texth() + metrics::border * 2 + metrics::padding;
	height = push_height;
	font = push_font;
}

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

void* answers::choose(const char* title, const char* cancel_text, bool interactive, const char* resid, int columns, const char* header) const {
	if(!interactive)
		return random();
	if(!elements)
		return 0;
	auto push_caret = caret;
	auto push_width = width;
	if(columns == -1)
		columns = getcolumns(*this);
	auto standart_width = getwidth() - 320 - (metrics::padding + metrics::border) * 3;
	auto column_width = standart_width;
	if(columns > 1)
		column_width = column_width / columns - metrics::border;
	while(ismodal()) {
		if(pbackground)
			pbackground();
		if(pwindow)
			pwindow();
		setposru();
		imagev(resid);
		if(beforepaint)
			beforepaint();
		setposlu();
		width = standart_width;
		texth2(header);
		if(title) {
			auto push_fore = fore;
			fore = colors::h3;
			textf(title);
			fore = push_fore;
			caret.y += metrics::padding;
		}
		auto index = 0;
		auto y1 = caret.y, x1 = caret.x;
		auto y2 = caret.y;
		auto next_column = (elements.getcount() + columns - 1) / columns;
		auto push_width_normal = width;
		width = column_width;
		for(auto& e : *this) {
			answerbt(index, e.value, e.text);
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
			answerbt(elements.getcount(), 0, cancel_text);
			fore = push_fore;
		}
		width = push_width;
		if(afterpaint)
			afterpaint();
		caret = push_caret;
		domodal();
	}
	return (void*)getresult();
}

void draw::avatar(const char* id) {
	auto p = gres(id, "art/portraits");
	if(!p)
		return;
	image(caret.x, caret.y, p, 0, 0);
	width = p->get(0).sx;
	height = p->get(0).sy;
	strokeout();
}

void draw::bar(int value, int maximum) {
	if(!value || !maximum)
		return;
	int w2 = width;
	if(value != maximum)
		w2 = (width - 2) * value / maximum;
	auto push_fore = fore;
	auto push_width = width;
	fore = colors::form;
	rectf();
	fore = push_fore;
	width = w2;
	auto push_caret = caret;
	setoffset(1, 1);
	rectf();
	caret = push_caret;
	fore = colors::border;
	rectb();
	fore = push_fore;
	width = push_width;
	caret.y += height;
}

static void tooltips_paint() {
	if(!draw::font)
		return;
	if(!tips_sb)
		return;
}

void draw::utg::tips() {
	tooltips_paint();
}

void draw::utg::beforemodal() {
	tips_caret.x = metrics::padding + metrics::border;
	tips_caret.y = getheight() - (metrics::padding + metrics::border);
	tips_size.x = getwidth() - (metrics::padding + metrics::border) * 2;
}

void draw::utg::paint() {
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
}

static void infopaint() {
	caret.y += metrics::padding;
	avatar("mistra");
	caret.x += width + metrics::padding;
	avatar("gordek");
	caret.x += width + metrics::padding;
	avatar("airel");
	caret.x += width + metrics::padding;
	avatar("airel");
}

int draw::utg::run(fnevent proc) {
	if(!proc)
		return -1;
	set_dark_theme();
	initialize_translation("ru");
	bsreq::read("rules/basic.txt");
	check_translation();
	if(log::geterrors())
		return -1;
	answers::beforepaint = infopaint;
	pbackground = paint;
	awindow.flags = 0;
	initialize(getnm("AppTitle"));
	setnext(proc);
	start();
	return 0;
}