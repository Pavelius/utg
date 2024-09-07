#include "draw.h"
#include "image.h"
#include "log.h"
#include "stringbuilder.h"

using namespace draw;
using namespace log;

static const char* last_url_error;

const char* get_command_line();

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

static void image_script(const char* url) {
	image_read(url);
	if(log::errors > 0)
		return;
	image_run("Main");
}

int main(int argc, char *argv[]) {
	initialize_png();
	initialize_image_plugins();
	log::print_proc = printcnf;
	if(argc==2)
		image_script(argv[1]);
	return 0;
}