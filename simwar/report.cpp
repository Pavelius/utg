#include "crt.h"
#include "report.h"

BSDATAD(reporti)

void reporti::add(const char* format, unsigned turn, unsigned reciever) {
	auto p = bsdata<reporti>::add();
	p->text = szdup(format);
	p->turn = turn;
	p->reciever = reciever;
}