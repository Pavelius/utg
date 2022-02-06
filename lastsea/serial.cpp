#include "archive.h"
#include "main.h"

template<> void archive::set<treasurei>(treasurei& e) {
	set(e.tags);
	set(e.owner);
}

template<> void archive::set<tilei>(tilei& e) {
	set(e.tags);
	set(e.index);
}

static void main_serial(const char* url, bool write_mode) {
	io::file file(url, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return;
	archive a(file, write_mode);
	if(!a.signature("GAM"))
		return;
	if(!a.version(0, 9))
		return;
	a.set(game);
	a.setc<treasurei>(bsdata<treasurei>::source);
	a.setc<tilei>(bsdata<tilei>::source);
}

void gamei::write(const char* url) {
	main_serial(url, true);
}