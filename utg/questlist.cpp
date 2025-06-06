#include "answers.h"
#include "bsreq.h"
#include "io_stream.h"
#include "log.h"
#include "pushvalue.h"
#include "questlist.h"
#include "script.h"

BSMETA(questlist) = {
	BSREQ(id),
	{}};
BSDATAC(questlist, 128)
const questlist* last_questlist;

void questlist_read(const char* url) {
	char temp[260];
	auto p = bsdata<questlist>::add();
	p->id = szdup(szfnamewe(temp, url));
	p->elements.setbegin();
	quest_read(url);
	p->elements.setend();
}

const quest* questlist::find(int index, const quest* pb) const {
	if(!pb)
		pb = elements.begin();
	for(auto pe = elements.end(); pb < pe; pb++) {
		if(pb->isanswer())
			continue;
		if(pb->index == index) {
			if(pb->tags && !script_allow(pb->tags))
				continue;
			return pb;
		}
	}
	return 0;
}

const questlist* find_quest(const quest* p) {
	if(!p)
		return 0;
	for(auto& e : bsdata<questlist>()) {
		auto pb = e.elements.begin();
		auto pe = e.elements.end();
		if(p >= pb && p < pe)
			return &e;
	}
	return 0;
}

const quest* questlist_find_prompt(short id) {
	return last_questlist->find(id, 0);
}