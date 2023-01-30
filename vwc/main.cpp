#include "answers.h"
#include "bsreq.h"
#include "code_package.h"
#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"
#include "log.h"

using namespace code;
using namespace draw;

void check_translation();
void initialize_code();
void initialize_interface();
void initialize_translation(const char* locale);
void set_dark_theme();
bool test_code();

void table_text_icon();

static void test_list() {
	static int origin, current;
	showborder();
	partoffset(1, 1);
	showbackground();
	draw::list(origin, current, texth() + 2, table_text_icon);
}

static void clear_fill() {
	auto push_fore = fore;
	fore = colors::form;
	rectf();
	fore = push_fore;
}

static const char* test_getname(const void* object, stringbuilder& sb) {
	auto ps = last_package->getsym(gui.index);
	auto pn = last_package->getstr(ps->id);
	return pn;
}

static void mainscene() {
	clear_fill();
	caret.x = metrics::padding;
	caret.y = metrics::padding + 1;
	width = 200;
	height = height - metrics::padding * 2 - 1;
	gui.count = last_package->symbols.getcount();
	//gui.value = "Row number %1i";
	gui.number = 1;
	gui.pgetname = test_getname;
	test_list();
}

static void mainstart() {
	if(!test_code())
		return;
	last_package = bsdata<package>::add();
	last_package->read("code/test.c2b");
	draw::scene(mainscene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	set_dark_theme();
	bsreq::read("rules/Basic.txt");
	initialize_translation("ru");
	check_translation();
	initialize_code();
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