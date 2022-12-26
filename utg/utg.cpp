#include "io_stream.h"
#include "pushvalue.h"
#include "draw_utg.h"

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

static const char* chooseavatar(answers& an, const char* title) {
	auto push_paint = answers::paintcell;
	answers::paintcell = draw::avatar;
	auto p = (const char*)an.choose(title);
	answers::paintcell = push_paint;
	return p;
}

const char* avatarable::choose(const char* title, const char* mask, int columns_count) {
	pushvalue push_columns(answers::column_count, columns_count);
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
	return (const char*)chooseavatar(an, title);
}