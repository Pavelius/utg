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

//int utg::choose(const array& source, const char* title, const char* cancel) {
//	return source.indexof(recordset::choose(source, title, cancel));
//}

//int utg::choose(const array& source, const flaga& mask, const char* title, const char* cancel) {
//	recordset records;
//	for(auto i = 0; i < mask.getmaximum(); i++) {
//		if(!mask.is(i))
//			continue;
//		records.add(source.ptr(i));
//	}
//	return source.indexof(records.choose(title, cancel, interactive, url, header));
//}

static int get_answer_index(const answers& source, const void* pv) {
	for(auto& e : source) {
		if(e.value == pv)
			return source.indexof(&e);
	}
	return -1;
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

void utg::pause() {
	pause(getnm("Continue"));
}

void utg::pause(const char* title) {
	answers an;
	an.add(0, title);
	an.choose(0);
	sb.clear();
}

bool utg::yesno(const char* title, ...) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(title, xva_start(title));
	answers an;
	an.add((void*)1, getnm("Yes"));
	an.add((void*)0, getnm("No"));
	return an.choose(temp);
}

int utg::choosei::getmarked(int i) const {
	if(i >= sizeof(marked)/ sizeof(marked[0]))
		return 0;
	return marked[i];
}

void utg::choosei::addmarked(int i) {
	if(i < sizeof(marked) / sizeof(marked[0]))
		marked[i]++;
}

bool utg::choosei::isallow(int index, const void* object) const {
	if(ismarked(index))
		return false;
	return true;
}

void utg::choosei::choose(const char* title, int count) {
	if(count > source.getcount())
		count = source.getcount();
	answers an;
	char temp[512]; stringbuilder sb(temp);
	while(count > 0) {
		an.clear(); sb.clear();
		if(title) {
			sb.add(title);
			if(count > 1)
				sb.adds("(%-Choose [%1i])", count);
		} else
			sb.adds(getnm("ChooseOptions"), count);
		for(auto& e : source) {
			auto index = source.indexof(&e);
			if(!isallow(index, e.value))
				continue;
			an.add(e.value, e.text);
		}
		auto result = an.choose(temp);
		auto result_index = get_answer_index(source, result);
		apply(result_index, result);
		if(result_index != -1)
			addmarked(result_index);
		count--;
	}
}