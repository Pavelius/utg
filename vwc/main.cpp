#include "answers.h"
#include "bsreq.h"
#include "code_package.h"
#include "code_lexer.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"
#include "log.h"
#include "rand.h"
#include "timer.h"
#include "view_source.h"
#include "view_statusbar.h"
#include "viewpackage.h"

using namespace code;
using namespace draw;

void before_modal_statusbar();
void check_translation();
void initialize_interface();
void initialize_pixels();
void paint_statusbar();
void set_dark_theme();
void view_code_tree();
void view_pixels();
void update_code_tree();

symbol* get_active_symbol();

static void code_error(const char* position, const char* format, const char* format_param) {
	char temp[2048]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	//log::errorv(position, temp);
}

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

static bool test_code() {
	code::project = "code";
	bsdata<lexer>::elements[0].activate();
	last_package = code::openview("test");
	last_package->write("code/test.c2b");
	return log::errors == 0;
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
	code::active_symbol = get_active_symbol();
	view_code_source();
	//view_pixels();
}

static void mainstart() {
	if(!test_code())
		return;
	update_code_tree();
	draw::scene(mainscene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	set_dark_theme();
	perror = code_error;
	bsreq::read("rules/Basic.txt");
	initialize_pixels();
	if(log::errors)
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