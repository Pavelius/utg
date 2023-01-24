#include "crt.h"
#include "report.h"

BSDATAD(reporti)

void reporti::add(const char* format, const char* header, unsigned turn, unsigned reciever) {
	auto p = bsdata<reporti>::add();
	p->text = szdup(format);
	p->header = header;
	p->turn = turn;
	p->reciever = reciever;
}