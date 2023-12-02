#include "answers.h"
#include "bsreq.h"
#include "code_package.h"
#include "code_lexer.h"
#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"
#include "log.h"
#include "view_statusbar.h"

using namespace code;
using namespace draw;

void before_modal_statusbar();
void check_translation();
void initialize_code();
void initialize_interface();
void initialize_pixels();
void initialize_translation(const char* locale);
void paint_statusbar();
void set_dark_theme();
bool test_code();
void view_code_tree();
void view_pixels();
void update_code_tree();

static void clear_fill() {
	auto push_fore = fore;
	fore = colors::form;
	rectf();
	fore = push_fore;
}

static void main_before_modal() {
	before_modal_statusbar();
}

static void main_tips() {
	tips_statusbar();
}

static void mainscene() {
	clear_fill();
	paint_statusbar();
	caret.x = metrics::padding;
	caret.y = metrics::padding + 1;
	width = 200;
	height = height - metrics::padding * 2 - 1;
	view_code_tree();
	caret.x += width + metrics::padding * 2 - 1;
	width = getwidth() - caret.x - metrics::padding * 2 - 1;
	view_pixels();
}

static void mainstart() {
	if(!test_code())
		return;
	//last_package = bsdata<package>::add();
	//last_package->read("code/test.c2b");
	update_code_tree();
	draw::scene(mainscene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	set_dark_theme();
	bsreq::read("rules/Basic.txt");
	lexer::read("rules/lexer_c2.txt");
	initialize_translation("ru");
	check_translation();
	initialize_code();
	initialize_pixels();
	if(log::geterrors())
		return -1;
	awindow.flags = WFResize | WFMinmax;
	metrics::border = 6;
	metrics::padding = 3;
	fore = colors::text;
	initialize(getnm("AppTitle"));
	pbeforemodal = main_before_modal;
	ptips = main_tips;
	setnext(mainstart);
	start();
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}