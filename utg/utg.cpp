#include "io_stream.h"
#include "recordset.h"
#include "utg.h"

fngetinfo		utg::callback::getinfo;
fnstatus		utg::callback::getstatus;
const char*		utg::url_avatars = "art/avatars";
static char		sb_value[4096];
stringbuilder	utg::sb(sb_value);

const char* utg::getchoose(const char* id) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("Choose%1", id);
	return getnm(temp);
}

const char* utg::chooseavatar(answers& an, const char* title) {
	auto push_paint = answers::paintcell;
	answers::paintcell = draw::avatarch;
	auto p = (const char*)an.choose(title);
	answers::paintcell = push_paint;
	return p;
}

void utg::chooseavatar(stringbuilder& result, const char* title, const char* mask) {
	answers an; char temp[260];
	for(io::file::find fn(utg::url_avatars); fn; fn.next()) {
		auto p = fn.name();
		if(p[0] == '.')
			continue;
		if(mask && !szpmatch(p, mask))
			continue;
		szfnamewe(temp, p);
		an.add(0, temp);
	}
	for(auto& e : an)
		e.value = (void*)e.text;
	auto p = utg::chooseavatar(an, title);
	if(p)
		result.add(p);
}