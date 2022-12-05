#include "answers.h"
#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "utg.h"
#include "widget.h"

using namespace draw;

array*		draw::heroes;
fngetname	draw::heroes_getavatar;
const void*	draw::focus_object;
int			draw::title_width = 220;
static point hide_separator;
static void* current_tab;

void set_dark_theme();
void set_light_theme();

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

static void stroke(const sprite* p, int frame) {
	auto push_width = width;
	auto push_height = height;
	auto& f = p->get(0);
	width = f.sx;
	height = f.sy;
	strokeout(strokeborder);
	width = push_width;
	height = push_height;
}

static void hiliting(const void* object, figure type = figure::Rect) {
	control_hilited = ishilite({caret.x, caret.y, caret.x + width, caret.y + height});
	if(control_hilited) {
		hilite_object = const_cast<void*>(object);
		hilite_type = type;
	}
}

static void focusing(const void* object) {
	if(!object)
		return;
	if(control_hilited && hot.key == MouseLeft && hot.pressed)
		execute(cbsetptr, (long)hilite_object, 0, &focus_object);
	if(focus_object == object)
		strokeactive();
}

static void imagev(const char* resid) {
	if(!resid)
		return;
	auto p = gres(resid, "art/images");
	if(!p)
		return;
	image(p, 0, 0);
	stroke(p, 0);
	caret.y += p->get(0).sy + metrics::padding + metrics::border * 2;
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

static void strokebar() {
	auto push_caret = caret;
	line(caret.x, caret.y + height);
	line(caret.x + width, caret.y);
	line(caret.x, caret.y - height);
	caret = push_caret;
}

static void barborder() {
	auto push_fore = fore;
	fore = colors::border;
	strokebar();
	fore = push_fore;
}

static void bartext(const char* format) {
	auto push_caret = caret;
	caret.y += metrics::border * 2;
	caret.x += metrics::padding;
	text(format);
	caret = push_caret;
}

static void bartextselected(const char* format) {
	auto push_fore = fore;
	fore = colors::active;
	bartext(format);
	fore = push_fore;
}

static void hilitingx(const char* id, figure v) {
	rectpush push;
	caret.x += 1;
	caret.y += 1;
	hiliting(id, v);
}

static void paintbar(const char* id, const void* element) {
	auto push_width = width;
	width = textw(id) + metrics::padding * 2;
	if(current_tab == element) {
		fillwindow();
		barborder();
		bartextselected(id);
	} else {
		bartext(id);
		hilitingx(id, figure::Rect3D);
		if(control_hilited && hot.key == MouseLeft && hot.pressed)
			execute(cbsetptr, (long)element, 0, &current_tab);
	}
	caret.x += width;
	width = push_width;
}

static int getbarpageheight() {
	return texth() + metrics::border * 2;
}

static void paintbars(void** pages, unsigned count) {
	rectpush push;
	caret.y += height + metrics::border - 1;
	caret.x -= metrics::border;
	height = getbarpageheight() + metrics::border;
	auto pe = pages + count;
	for(auto ps = pages; ps < pe; ps++) {
		auto pv = *ps;
		if(bsdata<menu>::have(pv))
			paintbar(getnm(((menu*)pv)->id), pv);
		else
			paintbar("Unknown", pv);
	}
}

static unsigned choose_pages_by_focus(void** ps, void** pe) {
	auto pb = ps;
	auto pm = varianti::find(focus_object);
	if(pm) {
		for(auto& e : bsdata<menu>()) {
			if(e.source != pm)
				continue;
			if(!e.isvisible())
				continue;
			if(ps < pe)
				*ps++ = &e;
		}
	}
	for(auto& e : bsdata<menu>()) {
		if(e.source != 0)
			continue;
		if(!e.isvisible())
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - pb;
}

static void standart_getproperty(const void* object, variant v, stringbuilder& sb) {
	auto pm = varianti::find(object);
	if(!pm)
		return;
	for(auto p = pm->metadata; *p; p++) {
		if(p->source == bsdata<varianti>::elements[v.type].source) {
			if(p->is(KindDSet)) {
				auto value = p->get(p->ptr(object, v.value));
				sb.add("%1i", value);
			} else if(p->is(KindFlags)) {
			}
		}
	}
}

static void properties() {
	void* pages[32];
	auto count = choose_pages_by_focus(pages, pages + sizeof(pages) / sizeof(pages[0]));
	height = getmaximumheight() - caret.y + 1;
	if(count > 1)
		height -= getbarpageheight() + metrics::border;
	strokeout(fillwindow);
	strokeout(strokeborder);
	if(!current_tab && count > 0)
		current_tab = pages[0];
	if(count > 1)
		paintbars(pages, count);
	caret.x += metrics::padding;
	caret.y += metrics::padding;
	width -= metrics::padding * 2;
	if(!current_tab || !count)
		return;
	if(bsdata<menu>::have(current_tab)) {
		auto push_menu = menu::last;
		auto push_title = title_width;
		title_width = 120;
		menu::last = (menu*)current_tab;
		if(menu::last->source) {
			auto proc = standart_getproperty;
			if(menu::last->source->pgetproperty)
				proc = menu::last->source->pgetproperty;
			label(focus_object, menu::last->elements, proc);
		} else {
			auto proc = standart_getproperty;
			if(utg::callback::getinfo)
				proc = utg::callback::getinfo;
			label(menu::last, menu::last->elements, proc);
		}
		title_width = push_title;
		menu::last = push_menu;
	}
}

static void avatars() {
	height = width = 0;
	if(!heroes || !heroes_getavatar)
		return;
	auto index = 0;
	auto size = heroes->size;
	auto pe = heroes->end();
	char temp[128]; stringbuilder sb(temp);
	for(auto p = heroes->begin(); p < pe; p += size) {
		sb.clear();
		auto pn = heroes_getavatar(p);
		if(!pn || pn[0] == 0)
			continue;
		avatar(index++, p, pn);
	}
}

static void panelv() {
	vertical(avatars);
	properties();
}

static void labelheader(const char* title) {
	if(!title || title[0] == 0)
		return;
	auto push_caret = caret;
	textf(title);
	caret = push_caret;
	caret.x += title_width;
}

static void labelvalue(const char* title) {
	if(!title || title[0] == 0)
		return;
	auto push_caret = caret;
	textf(title);
	caret = push_caret;
}

void draw::label(const char* title, const char* value) {
	auto push_caret = caret;
	labelheader(title);
	labelvalue(value);
	caret = push_caret;
	caret.y += texth() + 2;
}

void draw::label(const char* title, const char* value, const void* object) {
	auto push_height = height;
	height = texth();
	hiliting(object, figure::RectFill);
	label(title, value);
	height = push_height;
}

void draw::label(const char* title, const char* value, const void* object, fnevent click) {
	label(title, value, object);
	if(click && control_hilited && hot.key == MouseLeft && !hot.pressed)
		execute(click, 0, 0, object);
}

static void separator() {
	if(hide_separator == caret)
		return;
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x -= metrics::padding + 1;
	caret.y += texth() / 2;
	line(caret.x + width + metrics::padding * 2 + 2, caret.y);
	fore = push_fore;
	caret = push_caret;
	caret.y += texth();
	hide_separator = caret;
}

static void right_align_value() {
	title_width = 320 - textw('0') * 4 - metrics::padding * 2;
}

void widget::button() {
	if(!last)
		return;
	if(last->click)
		label(getnm(last->id), 0, last, last->click);
}

void draw::label(const void* object, const variants& elements, fngetinfo pget) {
	char temp[260]; stringbuilder sb(temp);
	auto push_tab = draw::tab_pixels;
	draw::tab_pixels = textw('0') * 6;
	hide_separator = caret;
	for(auto v : elements) {
		auto po = v.getpointer();
		if(bsdata<widget>::have(po))
			((widget*)po)->paint();
		else {
			auto id = v.getid();
			sb.clear(); pget(object, v, sb);
			if(sb)
				label(getnm(id), temp, v.getpointer());
		}
	}
	draw::tab_pixels = push_tab;
}

static void answerbt(int i, const void* pv, const char* title, fnevent press_event) {
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

static void answers_beforepaint() {
	auto push_height = height;
	setposru();
	imagev(answers::resid);
	panelv();
	setposlu();
	width = getwidth() - 320 - (metrics::padding + metrics::border * 2) * 2 - metrics::padding;
	height = push_height - metrics::padding - metrics::border * 2;
	strokeout(fillwindow);
	strokeout(strokeborder);
	caret.x += metrics::padding; width -= metrics::padding;
	caret.y += metrics::padding; height -= metrics::padding;
	texth2(answers::header);
	if(answers::prompt && *answers::prompt) {
		textf(answers::prompt);
		caret.y += metrics::padding;
	}
	if(answers::prompa) {
		auto push_fore = fore;
		fore = colors::h3;
		textf(answers::prompa);
		fore = push_fore;
		caret.y += metrics::padding;
	}
}

static void focusing_choose(const void* object) {
	if(!object)
		return;
	if(control_hilited && hot.key == MouseLeft && !hot.pressed)
		execute(buttonparam, (long)hilite_object);
}

static void avatar_common(int index, const void* object, const char* id, void(*proc)(const void* object)) {
	auto p = gres(id, utg::url_avatars);
	if(!p)
		return;
	width = p->get(0).sx;
	height = p->get(0).sy;
	auto dx = width + metrics::padding + metrics::border * 2;
	if(caret.x + dx >= clipping.x2) {
		caret.x = metrics::padding * 2 + metrics::border;
		caret.y += height + metrics::padding + metrics::border * 2;
	}
	image(caret.x, caret.y, p, 0, 0);
	strokeout(strokeborder);
	hiliting(object);
	proc(object);
	caret.x += dx;
}

void draw::avatar(int index, const void* object, const char* id) {
	avatar_common(index, object, id, focusing);
}

void draw::avatarch(int index, const void* object, const char* id, fnevent press_event) {
	avatar_common(index, object, id, focusing_choose);
}

void draw::noavatar() {
	width = 64;
	height = 64;
	strokeout(strokeborder);
	caret.x += width + metrics::padding + metrics::border * 2;
}

static void circleactive() {
	auto push_fore = fore;
	auto push_caret = caret;
	caret.x += width / 2;
	caret.y += height / 2;
	fore = colors::active;
	auto w = width / 2;
	if(hot.pressed)
		w -= 2;
	circle(w);
	caret = push_caret;
	fore = push_fore;
}

static void hilite_paint() {
	rectpush push;
	caret.x = hot.hilite.x1;
	caret.y = hot.hilite.y1;
	width = hot.hilite.width();
	height = hot.hilite.height();
	switch(hilite_type) {
	case figure::Circle: circleactive(); break;
	case figure::Rect: strokeout(strokeactive); break;
	case figure::RectFill: strokeout(stroke_active_fill, 2); break;
	case figure::Rect3D: stroke_active_fill(); break;
	default: break;
	}
}

static void tooltips_paint() {
	if(hot.hilite)
		hilite_paint();
}

static void statusbar_paint() {
	if(!hilite_object || !utg::callback::getstatus)
		return;
	char temp[512]; stringbuilder sb(temp); sb.clear();
	utg::callback::getstatus(hilite_object, sb);
	if(!temp[0])
		return;
	rectpush push;
	caret.x = metrics::padding + metrics::border;
	caret.y = getheight() - texth() - metrics::border - 1;
	width = getwidth() - caret.x * 2;
	auto push_width = width;
	textfs(temp);
	caret.x += (push_width - width) / 2;
	textf(temp);
}

static void statusbar() {
	auto push_height = height;
	auto dy = texth() + metrics::border * 2; height = dy;
	caret.y = getheight() - dy;
	gradv(colors::form, colors::window, 0);
	strokeline();
	caret.y += metrics::border;
	height = push_height;
	height -= dy;
}

static void downbar() {
	auto push_caret = caret;
	statusbar();
	caret = push_caret;
}

static void getinformation(const char* id, stringbuilder& sb) {
	auto p = getdescription(id);
	if(!p)
		return;
	sb.add(p);
}

void utg::getstatus(const void* object, stringbuilder& sb) {
	auto pm = varianti::find(object);
	if(!pm)
		return;
	if(pm->pgetinfo)
		pm->pgetinfo(object, sb);
	else if(pm->metadata) {
		auto pk = pm->metadata->find("id");
		if(pk) {
			auto id = pk->gets(object);
			if(id)
				getinformation(id, sb);
		}
	}
}

static void beforemodal() {
	hilite_type = figure::Rect;
	tips_caret.x = metrics::padding + metrics::border;
	tips_caret.y = getheight() - (metrics::padding + metrics::border);
	tips_size.x = getwidth() - (metrics::padding + metrics::border) * 2;
}

static void paint() {
	fillform();
	caret.y += metrics::padding + metrics::border * 2;
	height -= metrics::padding + metrics::border * 2;
	downbar();
}

static void tips() {
	tooltips_paint();
	statusbar_paint();
}

static void initialize_widgets() {
	widget::add("Separator", separator);
	widget::add("RightAlignValue", right_align_value);
}

void initialize_png();
void check_translation();
void initialize_translation(const char* locale);

int draw::start(fnevent proc, fnevent afterread) {
	initialize_png();
	if(!utg::callback::getstatus)
		utg::callback::getstatus = utg::getstatus;
	if(!proc)
		return -1;
	set_dark_theme();
	initialize_widgets();
	bsreq::read("rules/Basic.txt");
	initialize_translation("ru");
	if(afterread)
		afterread();
	check_translation();
	if(log::geterrors())
		return -1;
	pbeforemodal = beforemodal;
	pbackground = paint;
	ptips = tips;
	awindow.flags = 0;
	metrics::border = 1;
	metrics::padding = 4;
	answers::paintcell = answerbt;
	answers::beforepaint = answers_beforepaint;
	initialize(getnm("AppTitle"));
	setnext(proc);
	start();
	return 0;
}