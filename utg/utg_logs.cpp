#include "io_stream.h"
#include "recordset.h"
#include "utg.h"

const char*		logs::url;
const char*		logs::url_avatars = "art/avatars";
const char*		logs::header;
bool			logs::interactive = true;
static char		sb_value[4096];
stringbuilder	logs::sb(sb_value);

const char* logs::getchoose(const char* id) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("Choose%1", id);
	return getnm(temp);
}

int logs::choose(const array& source, const char* title, const char* cancel) {
	return source.indexof(recordset::choose(source, title, cancel, interactive, url, header));
}

int logs::choose(const answers& source, const char* title, const char* cancel) {
	return (int)source.choose(title, cancel, interactive, url, -1, header);
}

void* logs::choose(const answers& source, const char* title) {
	return source.choose(title, 0, interactive, url, -1, header, sb_value);
}

int logs::choose(const array& source, const flaga& mask, const char* title, const char* cancel) {
	recordset records;
	for(auto i = 0; i < mask.getmaximum(); i++) {
		if(!mask.is(i))
			continue;
		records.add(source.ptr(i));
	}
	return source.indexof(records.choose(title, cancel, interactive, url, header));
}

static int get_answer_index(const answers& source, const void* pv) {
	for(auto& e : source) {
		if(e.value == pv)
			return source.indexof(&e);
	}
	return -1;
}

const char* logs::chooseavatar(answers& an, const char* title) {
	auto push_paint = answers::paintcell;
	answers::paintcell = draw::avatarch;
	auto p = (const char*)an.choose(title, 0, interactive, logs::url, 1, logs::header, 0);
	answers::paintcell = push_paint;
	return p;
}

void logs::chooseavatar(stringbuilder& result, const char* title, const char* mask) {
	answers an; char temp[260];
	for(io::file::find fn(logs::url_avatars); fn; fn.next()) {
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
	auto p = logs::chooseavatar(an, title);
	if(p)
		result.add(p);
}

void logs::pause() {
	pause(getnm("Continue"));
}

void logs::pause(const char* title) {
	answers an;
	an.add(0, title);
	choose(an, 0);
}

int logs::choosei::getmarked(int i) const {
	if(i >= sizeof(marked)/ sizeof(marked[0]))
		return 0;
	return marked[i];
}

void logs::choosei::addmarked(int i) {
	if(i < sizeof(marked) / sizeof(marked[0]))
		marked[i]++;
}

bool logs::choosei::isallow(int index, const void* object) const {
	if(ismarked(index))
		return false;
	return true;
}

void logs::choosei::choose(const char* title, int count) {
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
		auto result = an.choose(temp, 0, interactive, url, -1, header, sb_value);
		auto result_index = get_answer_index(source, result);
		apply(result_index, result);
		if(result_index != -1)
			addmarked(result_index);
		count--;
	}
}