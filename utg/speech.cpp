#include "bsreq.h"
#include "log.h"
#include "nameable.h"
#include "rand.h"
#include "speech.h"
#include "stringbuilder.h"

using namespace log;

BSDATAD(speech::element)
BSDATAC(speech, 1024)

BSMETA(speech) = {
	BSREQ(id),
	{}};

unsigned char* speech_params;

void speech_read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		p = sb.psidf(p + 1);
		auto pr = bsdata<speech>::add();
		pr->id = szdup(temp);
		if(!checksym(p, '\n'))
			break;
		p = skipwscr(p);
		auto psb = bsdata<speech::element>::source.count;
		while(allowparse && *p && *p != '#') {
			sb.clear();
			p = sb.psstrlf(skipwscr(p));
			p = skipwscr(p);
			speech::element e = {};
			e.name = szdup(temp);
			bsdata<speech::element>::source.add(&e);
		}
		if(psb != bsdata<speech::element>::source.count)
			pr->source.set((speech::element*)bsdata<speech::element>::source.ptr(psb), bsdata<speech::element>::source.count - psb);
	}
	log::close();
}

const char* speech_getid(int index) {
	return bsdata<speech>::elements[index].id;
}

const char* speech_name(int index) {
	return ((speech::element*)bsdata<speech::element>::source.ptr(index))->name;
}

const speech* speech_find(const char* id) {
	return bsdata<speech>::find(id);
}

int speech_first(const speech* p) {
	if(!p || !p->source)
		return -1;
	return p->source.start;
}

int speech_count(const speech* p) {
	if(!p || !p->source)
		return -1;
	return p->source.count;
}

int speech_random(const char* id) {
	auto p = bsdata<speech>::find(id);
	if(!p || !p->source)
		return -1;
	return p->source.start + (rand() % p->source.size());
}

const char* speech_get(const char* id) {
	auto p = bsdata<speech>::find(id);
	if(!p || !p->source)
		return 0;
	auto n = (speech_params ? *speech_params++ : rand()) % p->source.size();
	return p->source.begin()[n].name;
}

const char* speech_get(const speech* p, int n) {
	if(!p || !p->source)
		return 0;
	return p->source.begin()[n].name;
}

void speech_get(const char*& result, const char* id, const char* action, const char* middle, const char* postfix) {
	if(result)
		return;
	char temp[64]; stringbuilder sb(temp);
	sb.add(id);
	sb.add(action);
	sb.add(middle);
	sb.add(postfix);
	auto p = speech_get(temp);
	if(p)
		result = p;
}

bool apply_speech(const char* id, stringbuilder& sb) {
	auto p = speech_get(id);
	if(!p)
		return false;
	sb.add(p);
	return true;
}