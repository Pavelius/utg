#include "bsreq.h"
#include "widget.h"

BSMETA(widget) = {
	BSREQ(id),
	{}};

const widget* last_widget;

void widget::paint() const {
	auto push_last = last_widget;
	last_widget = this; proc();
	last_widget = push_last;
}

void add_widget(const char* id, fnevent proc, fnevent click) {
	auto p = (widget*)bsdata<widget>::source.addfind(id);
	p->proc = proc;
	p->click = click;
}