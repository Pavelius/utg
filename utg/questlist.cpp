#include "answers.h"
#include "pushvalue.h"
#include "questlist.h"

BSDATAC(questlist, 128)
questlist* lastquest;

void questlist::read(const char* id) {
	char temp[260]; stringbuilder sb(temp);
	sb.addlocaleurl();
	sb.add("/");
	sb.add("%1.txt", id);
	auto p = bsdata<questlist>::add();
	p->id = szdup(id);
	auto q1 = bsdata<quest>::end();
	quest::read(temp);
	p->elements.set(q1, bsdata<quest>::end() - q1);
}

const quest* questlist::find(int index, const quest* pb) const {
	if(!pb)
		pb = elements.begin();
	for(auto pe = elements.end(); pb < pe; pb++) {
		if(pb->isanswer())
			continue;
		if(pb->index == index)
			return pb;
	}
	return 0;
}