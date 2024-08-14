#include "crt.h"
#include "draw.h"
#include "image.h"
#include "log.h"
#include "stringbuilder.h"

using namespace draw;
using namespace log;

static const char* last_url_error;

void initialize_image_plugins();
void initialize_png();
void printcnf(const char* text);

static void printlnv(const char* format, const char* format_param) {
	char temp[512]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printcnf(temp);
	println();
}

static void println(const char* format, ...) {
	printv(format, xva_start(format));
	println();
}

static void image_test() {
	image_read("example.txt");
	if(log::geterrors() > 0)
		return;
	image_run("Main");
}

int main() {
	initialize_png();
	initialize_image_plugins();
	log::print_proc = printcnf;
	image_test();
	return 0;
}