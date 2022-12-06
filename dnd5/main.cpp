#include "bsreq.h"
#include "io_stream.h"
#include "strategy.h"
#include "character.h"
#include "script.h"

void status_info(void) {
}

void main_script(variant v);

static void main_menu() {
	character e1;
	e1.generate();
}

static void read_files(const char* url, const char* mask, void(*proc)(const char* url)) {
	for(io::file::find file(url); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, mask))
			continue;
		char temp[260];
		proc(file.fullname(temp));
	}
}

static void initialize() {
	read_files("rules", "*Races.txt", bsreq::read);
	read_files("rules", "Class*.txt", bsreq::read);
	bsreq::read("rules/Menu.txt");
	bsreq::read("rules/CharacterGenerate.txt");
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	script::prun = main_script;
	return draw::strategy(main_menu, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}