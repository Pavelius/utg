#include "condition.h"
#include "main.h"
#include "quest.h"
#include "pathfind.h"

static int m_ability, m_value;

static const quest* find_roll_result(const quest* ph, int result) {
	const quest* pr = 0;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	if(result > 29)
		result = 29;
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index || p->next<1 || p->next > result)
			break;
		pr = p;
	}
	return pr;
}

static void make_roll(int bonus, int param) {
}

static void make_pay(int bonus, int param) {
}

static void make_buy(int bonus, int param) {
}

static void ask_agree(int bonus, int param) {
}

BSDATA(scripti) = {
	{"Buy", make_buy},
	{"Pay", make_pay},
	{"Roll", make_roll},
	{"YesNo", ask_agree},
};
BSDATAF(scripti)