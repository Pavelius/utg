#include "answers.h"

const char* answers::header;
const char* answers::prompt;
const char* answers::resid;
bool answers::show_tips = true;
bool answers::interactive = true;
int answers::column_count = 1;
stringbuilder* answers::console;
fnevent answers::beforepaint;
fnevent answers::afterpaint;
answers::fnpaint answers::paintcell;

int answers::compare(const void* v1, const void* v2) {
	return strcmp(((answers::element*)v1)->text, ((answers::element*)v2)->text);
}

void answers::addv(const void* value, const char* text, const char* format) {
	auto p = elements.add();
	p->value = value;
	p->text = sc.get();
	sc.addv(text, format);
	sc.addsz();
}

void answers::sort() {
	qsort(elements.data, elements.count, sizeof(elements.data[0]), compare);
}

void answers::modal(const char* title, const char* cancel) const {
	auto proc = (fnevent)choose(title, cancel);
	if(proc)
		proc();
}

void* answers::random() const {
	if(!elements.count)
		return 0;
	return (void*)elements.data[rand() % elements.count].value;
}

const char* answers::getname(void* v) {
	for(auto& e : elements) {
		if(e.value == v)
			return e.text;
	}
	return 0;
}

void answers::clear() {
	elements.clear();
	sc.clear();
}

void draw::pause() {
	pause(getnm("Continue"));
}

void draw::pause(const char* title, ...) {
	answers an;
	an.addv(0, title, xva_start(title));
	an.choose(0);
	if(answers::console)
		answers::console->clear();
}

bool draw::yesno(const char* title, ...) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(title, xva_start(title));
	answers an;
	an.add((void*)1, getnm("Yes"));
	an.add((void*)0, getnm("No"));
	return an.choose(temp);
}