#include "image.h"
#include "io_stream.h"
#include "logparse.h"
#include "pushvalue.h"

using namespace log;

static const char* p;

static void skip(const char* command) {
	if(allowparse && !equal(p, command)) {
		error(p, "Expected token `%1`", command);
		allowparse = false;
	}
}

static void parse() {
	while(*p && allowparse) {
		skip("#");
		if(!allowparse)
			break;
		//readid();
	}
}

void image_read(const char* url) {
	pushvalue push(context);
	pushvalue push_parser(p);
	p = read(url);
	if(!p)
		return;
	parse();
	close();
}