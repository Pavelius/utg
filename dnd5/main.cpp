#include "bsreq.h"
#include "io_stream.h"
#include "strategy.h"
#include "main.h"

void status_info(void) {
}

static void main_menu() {
}

static void read_files(const char* url, const char* mask) {
	for(io::file::find file(url); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, mask))
			continue;
		char temp[260];
		bsreq::read(file.fullname(temp));
	}
}

static void initialize() {
	read_files("rules", "*Races.txt");
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	return draw::strategy(main_menu, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}