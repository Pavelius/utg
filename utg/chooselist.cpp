#include "chooselist.h"

int chooselist::getmarked(int i) const {
	if(i >= sizeof(marked) / sizeof(marked[0]))
		return 0;
	return marked[i];
}

void chooselist::addmarked(int i) {
	if(i < sizeof(marked) / sizeof(marked[0]))
		marked[i]++;
}

bool chooselist::isallow(int index, const void* object) const {
	if(ismarked(index))
		return false;
	return true;
}

int chooselist::getvalueindex(const void* pv) const {
	for(auto& e : *this) {
		if(e.value == pv)
			return indexof(&e);
	}
	return -1;
}

bool chooselist::choose(const char* title, int count, const char* cancel) {
	if(count > getcount())
		count = getcount();
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
		for(auto& e : *this) {
			auto index = indexof(&e);
			if(!isallow(index, e.value))
				continue;
			an.add(e.value, e.text);
		}
		if(cancel)
			an.add(0, cancel);
		auto result = an.choose(temp);
		if(!cancel)
			return false;
		auto result_index = getvalueindex(result);
		apply(result_index, result);
		if(result_index != -1)
			addmarked(result_index);
		count--;
		cancel = 0;
	}
	return true;
}