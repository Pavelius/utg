#include "answers.h"
#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "utg.h"

using namespace draw;

point draw::offset;
const void* draw::hilite_object;
figure draw::hilite_type;
fnstatus draw::pstatus;

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

static void fill_form() {
	auto push_fore = fore;
	fore = colors::form;
	rectf();
	fore = push_fore;
}

static void fill_window() {
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
}

static void stroke_border() {
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

static void stroke_active() {
	auto push_fore = fore;
	fore = colors::active;
	rectb();
	fore = push_fore;
}

static void stroke_active_fill() {
	auto push_fore = fore;
	auto push_alpha = alpha;
	if(hot.pressed)
		fore = colors::active.mix(colors::button, 128);
	else
		fore = colors::active;
	alpha = 32;
	rectf();
	alpha = push_alpha;
	fore = push_fore;
}

static void strokex(fnevent proc, int dx = 0, int dy = 0) {
	rectpush push;
	caret.x -= metrics::border;
	caret.y -= metrics::border;
	width += metrics::border * 2 + dx;
	height += metrics::border * 2 + dy;
	proc();
}

static void stroke(const sprite* p, int frame) {
	auto push_width = width;
	auto push_height = height;
	auto& f = p->get(0);
	width = f.sx;
	height = f.sy;
	strokex(stroke_border, -1);
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
	caret.y += p->get(0).sy + metrics::padding + metrics::border;
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

static int getmaximumheight() {
	return getheight() - texth() - metrics::border * 2 - metrics::padding * 2;
}

static void property_bar() {
	auto push_height = height;
	height = getmaximumheight() - caret.y;
	strokex(fill_window);
	strokex(stroke_border);
}

void draw::pages() {
	property_bar();
}

void draw::vertical(fnevent proc) {
	auto push_caret = caret;
	auto push_width = width;
	proc();
	width = push_width;
	caret = push_caret;
	caret.y += height + metrics::padding + metrics::border * 2;
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
	if(control_hilited) {
		hilite_object = pv;
		hilite_type = figure::RectFill;
	}
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
	auto standart_width = getwidth() - 320 - (metrics::padding + metrics::border * 2) * 3;
	auto column_width = standart_width - metrics::padding * 2;
	if(columns > 1)
		column_width = column_width / columns - metrics::border;
	while(ismodal()) {
		if(pbackground)
			pbackground();
		auto push_height = height;
		if(pwindow)
			pwindow();
		auto push_caret = caret;
		caret.x = getwidth() - 320 - metrics::padding - metrics::border * 2;
		imagev(resid);
		if(beforepaint)
			beforepaint();
		caret = push_caret;
		caret.x = metrics::padding + metrics::border * 2;
		width = standart_width;
		height = push_height - metrics::padding - metrics::border*2;
		strokex(fill_window);
		strokex(stroke_border);
		caret.x += metrics::padding; width -= metrics::padding;
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

static bool hiliting(const void* object) {
	control_hilited = ishilite({caret.x, caret.y, caret.x + width, caret.y + height});
	if(control_hilited)
		hilite_object = const_cast<void*>(object);
	return control_hilited;
}

void draw::avatar(const char* id, const void* object) {
	auto p = gres(id, "art/portraits");
	if(!p)
		return;
	image(caret.x, caret.y, p, 0, 0);
	width = p->get(0).sx;
	height = p->get(0).sy;
	strokex(stroke_border, -1);
	hiliting(object);
}

void draw::noavatar() {
	width = 64;
	height = 64;
	strokex(stroke_border);
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

static void hilite_paint() {
	rectpush push;
	caret.x = hot.hilite.x1;
	caret.y = hot.hilite.y1;
	width = hot.hilite.width();
	height = hot.hilite.height();
	switch(hilite_type) {
	case figure::Rect:
		strokex(stroke_active);
		break;
	case figure::RectFill:
		strokex(stroke_active_fill, 2);
		break;
	}
}

static void tooltips_paint() {
	if(hilite_object && hot.hilite)
		hilite_paint();
}

static void statusbar_paint() {
	if(!hilite_object || !pstatus)
		return;
	char temp[512]; stringbuilder sb(temp); sb.clear();
	pstatus(hilite_object, sb);
	if(!temp[0])
		return;
	rectpush push;
	caret.x = metrics::padding + metrics::border;
	caret.y = getheight() - texth() - metrics::border;
	width = getwidth() - caret.x * 2;
	auto push_width = width;
	textfs(temp);
	caret.x += (push_width - width) / 2;
	textf(temp);
}

static void stroke_line() {
	rectpush push;
	auto push_fore = fore;
	fore = colors::border;
	line(caret.x + width, caret.y);
	fore = push_fore;
}

static void topbar() {
	auto push_height = height;
	height = texth() + metrics::border * 2;
	gradv(colors::form, colors::window, 0);
	caret.y += height;
	stroke_line();
	caret.y += metrics::padding + metrics::border * 2;
	height = push_height;
	height -= caret.y;
}

static void statusbar() {
	auto push_height = height;
	auto dy = texth() + metrics::border * 2; height = dy;
	caret.y = getheight() - dy;
	gradv(colors::form, colors::window, 0);
	stroke_line();
	caret.y += metrics::border;
	height = push_height;
	height -= dy;
}

static void downbar() {
	auto push_caret = caret;
	statusbar();
	caret = push_caret;
}

void draw::nextpos() {
	if(offset.x)
		caret.x += width + offset.x * (metrics::padding + metrics::border * 2);
	if(offset.y)
		caret.y += height + offset.y * (metrics::padding + metrics::border * 2);
}

void draw::utg::beforemodal() {
	hilite_object = 0;
	hilite_type = figure::Rect;
	tips_caret.x = metrics::padding + metrics::border;
	tips_caret.y = getheight() - (metrics::padding + metrics::border);
	tips_size.x = getwidth() - (metrics::padding + metrics::border) * 2;
}

void draw::utg::paint() {
	fill_form();
	topbar();
	downbar();
}

void draw::utg::tips() {
	tooltips_paint();
	statusbar_paint();
}

int draw::utg::run(fnevent proc, bool darkmode) {
	if(!proc)
		return -1;
	if(darkmode)
		set_dark_theme();
	else
		set_light_theme();
	initialize_translation("ru");
	bsreq::read("rules/basic.txt");
	check_translation();
	if(log::geterrors())
		return -1;
	pbeforemodal = beforemodal;
	pbackground = paint;
	ptips = tips;
	awindow.flags = 0;
	metrics::border = 2;
	metrics::padding = 4;
	initialize(getnm("AppTitle"));
	setnext(proc);
	start();
	return 0;
}