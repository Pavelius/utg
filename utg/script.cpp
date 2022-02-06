#include "crt.h"
#include "script.h"
#include "stringbuilder.h"

BSDATAC(scripti, 256)

static void def_script(int counter, int param) {
}

static void def_info(stringbuilder& sb, const scripti& e, int counter) {
	if(counter != 0)
		sb.add("%1%+2i", getnm(e.id), counter);
	else
		sb.add("%1", getnm(e.id));
}

void scripti::add(const char* id, fnevent proc, int param, fninfo info) {
	if(!proc)
		proc = def_script;
	if(!info)
		info = def_info;
	auto p = (scripti*)bsdata<scripti>::source.addfind(id);
	p->proc = proc;
	p->param = param;
	p->info = info;
}