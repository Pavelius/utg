#include "crt.h"
#include "draw.h"
#include "image.h"
#include "log.h"
#include "stringbuilder.h"

using namespace draw;

static const char* last_url_error;

void initialize_image_plugins();
void initialize_png();
void printcnf(const char* text);

static void println() {
	printcnf("\r\n");
}

static void printv(const char* format, const char* format_param) {
	char temp[512]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printcnf(temp);
}

static void printlnv(const char* format, const char* format_param) {
	char temp[512]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printcnf(temp);
	println();
}

static void print(const char* format, ...) {
	printv(format, xva_start(format));
}

static void println(const char* format, ...) {
	printv(format, xva_start(format));
	println();
}

static void print_error(const char* position, const char* format) {
	log::error_count++;
	if(position && log::context.file)
		print(" Line %1i:", log::getline(log::context.file, position));
	print(format);
	println();
}

static void image_test() {
	image_read("example.txt");
	if(log::geterrors() > 0)
		return;
	println("Read %1i header and %2i elements", bsdata<imagea>::source.getcount(), bsdata<imagei>::source.getcount());
	image_run("Main");
}

int main() {
	initialize_png();
	initialize_image_plugins();
	log::error_proc = print_error;
	image_errorv_proc = printlnv;
	image_messagev_proc = printlnv;
	image_test();
	return 0;
}