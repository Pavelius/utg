#include "editor.h"
#include "io_stream.h"

color pallette[256];

static void read_pallette_rgb(io::stream& file) {
	for(auto i = 0; i < 256; i++) {
		file.read(pallette[i].r);
		file.read(pallette[i].g);
		file.read(pallette[i].b);
		pallette[i].r <<= 2;
		pallette[i].g <<= 2;
		pallette[i].b <<= 2;
		pallette[i].a = 0;
	}
}

void read_pallette(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return;
	read_pallette_rgb(file);
}