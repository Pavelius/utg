#include "answers.h"
#include "bsreq.h"
#include "crt.h"
#include "draw.h"
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
	draw::list(origin, current, 20, table_text_icon);
}

static void mainscene() {
	fore = colors::window;
	rectf();
	caret.x = 100;
	caret.y = 100;
	width = 200;
	height = 300;
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
	initialize(getnm("AppTitle"));
	setnext(mainstart);
	start();
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}