#include "answers.h"
#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"
#include "log.h"

using namespace draw;

void check_translation();
void initialize_interface();
void initialize_translation(const char* locale);
void set_dark_theme();

void table_text_icon();

static void test_list() {
	static int origin, current;
	showborder();
	draw::list(origin, current, texth() + 2, table_text_icon);
}

static void clear_fill() {
	auto push_fore = fore;
	fore = colors::window;
	rectf();
	fore = push_fore;
}

static const char* test_getname(const void* object, stringbuilder& sb) {
	sb.add(gui.value, gui.index);
	return sb.begin();
}

static void mainscene() {
	clear_fill();
	caret.x = 100;
	caret.y = 100;
	width = 200;
	height = 100;
	gui.count = 10;
	gui.value = "Row number %1i";
	gui.number = 3;
	gui.pgetname = test_getname;
	test_list();
}

static void mainstart() {
	draw::scene(mainscene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	set_dark_theme();
	bsreq::read("rules/Basic.txt");
	initialize_translation("ru");
	check_translation();
	if(log::geterrors())
		return -1;
	awindow.flags = WFResize | WFMinmax;
	metrics::border = 6;
	metrics::padding = 3;
	fore = colors::text;
	initialize(getnm("AppTitle"));
	setnext(mainstart);
	start();
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}