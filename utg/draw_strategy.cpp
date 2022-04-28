#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "strategy.h"

using namespace draw;

static point	camera_drag;

void set_dark_theme();
void set_light_theme();
void initialize_png();
void check_translation();
void initialize_translation(const char* locale);

static void beforemodal() {
	tips_caret.x = metrics::padding + metrics::border;
	tips_caret.y = getheight() - (metrics::padding + metrics::border);
	tips_size.x = getwidth() - (metrics::padding + metrics::border) * 2;
}

static void linedown() {
	auto push_caret = caret;
	line(caret.x + width, caret.y);
	caret = push_caret;
	caret.y += metrics::padding;
}

void status_info();

static void statusbar() {
	status_info();
	caret.y += texth() + metrics::padding;
	linedown();
}

static void paint() {
	fillform();
	statusbar();
	caret.y += metrics::padding + metrics::border * 2;
	height -= metrics::padding + metrics::border * 2;
}

static void tips() {
}

static void camera_finish() {
	const int step = 32;
	switch(hot.key) {
	case KeyLeft: execute(cbsetsht, camera.x - step, 0, &camera.x); break;
	case KeyRight: execute(cbsetsht, camera.x + step, 0, &camera.x); break;
	case KeyUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case KeyDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseWheelUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case MouseWheelDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseLeft:
		if(hot.pressed && !hot.hilite) {
			dragbegin(&camera);
			camera_drag = camera;
		}
		break;
	default:
		if(dragactive(&camera)) {
			hot.cursor = cursor::All;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (dragmouse - hot.mouse);
		}
		break;
	}
}

static void finish() {
	camera_finish();
}

int draw::strategy(fnevent proc, fnevent afterread) {
	initialize_png();
	if(!proc)
		return -1;
	set_dark_theme();
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
	pfinish = finish;
	awindow.flags = WFResize|WFMinmax;
	metrics::border = 4;
	metrics::padding = 4;
	initialize(getnm("AppTitle"));
	setnext(proc);
	start();
	return 0;
}