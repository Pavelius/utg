#include "answers.h"
#include "bsreq.h"
#include "log.h"
#include "pushvalue.h"
#include "questlist.h"

BSMETA(questlist) = {
	BSREQ(id),
	{}};
BSDATAC(questlist, 128)
questlist* last_questlist;

void questlist::read(const char* url) {
	char temp[260];
	szfnamewe(temp, url);
	auto p = bsdata<questlist>::add();
	p->id = szdup(temp);
	auto q1 = bsdata<quest>::end();
	quest::read(url);
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

void questlist_initialize() {
	log::readlocfolder(questlist::read, "quests", "*.txt");
}