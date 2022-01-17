#include "recordset.h"
#include "utg.h"

const char* logs::url;
const char* logs::header;
bool logs::interactive;
flagable<4> logs::multiply_choose;
static char sb_value[4096];
stringbuilder logs::sb(sb_value);

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

void logs::apply(const answers& source, const char* title, fncommand proc, int count) {
	if(source.getcount() > sizeof(multiply_choose) * 8)
		return;
	if(!proc)
		return;
	if(count > source.getcount())
		count = source.getcount();
	flagable<4> choosed;
	answers an;
	char temp[512]; stringbuilder sb(temp);
	while(count > 0) {
		an.clear();
		sb.clear();
		if(title) {
			sb.add(title);
			if(count > 1)
				sb.adds("(%-Choose [%1i])", count);
		} else
			sb.adds("%Choose [%1i]", count);
		for(auto& e : source) {
			auto index = source.indexof(&e);
			if(choosed.is(index) && !multiply_choose.is(index))
				continue;
			an.add(e.value, e.text);
		}
		auto result = an.choose(temp, 0, interactive, url, -1, header, sb_value);
		auto result_index = get_answer_index(source, result);
		proc(result);
		if(result_index != -1)
			choosed.set(result_index);
		count--;
	}
	multiply_choose.clear();
}