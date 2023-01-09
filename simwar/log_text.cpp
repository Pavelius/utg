#include "io_stream.h"
#include "stringbuilder.h"

void log_text_raw(const char* format) {
	static io::file file("locale/ru/Debug.txt", StreamWrite|StreamText);
	if(!file)
		return;
	file << format;
}

void log_text(const char* format, ...) {
	char temp[4096]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	sb.add("\n");
	log_text_raw(temp);
}