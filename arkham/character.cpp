#include "main.h"

static		adat<char, 32> roll_result;
character*	character::last;

static int compare(const void* p1, const void* p2) {
	return *((char*)p2) - *((char*)p1);
}

static void add_dices(int count) {
	for(auto i = 0; i < count; i++)
		roll_result.add(1 + rand() % 6);
	qsort(roll_result.data, roll_result.count, sizeof(roll_result.data[0]), compare);
}

static void roll_dices(int count) {
	roll_result.clear();
	add_dices(count);
}

static int roll_success(int sv) {
	int result = 0;
	for(auto v : roll_result) {
		if(v >= sv)
			result++;
	}
	return result;
}

void character::clear() {
	memset(this, 0, sizeof(*this));
}

int character::getsuccess() const {
	return 5;
}

static void add_clue() {
	auto p = character::last;
	p->add(Clue, -1);
	add_dices(1);
}

int character::roll(ability_s v, int m) {
	char header[128]; stringbuilder sh(header);
	sh.add("%Roll %1%+2i", getnm(bsdata<abilityi>::get(v).id), m);
	answers::header = header;
	answers an;
	char temp[512]; stringbuilder sb(temp);
	roll_dices(m);
	while(true) {
		auto sv = getsuccess();
		an.clear();
		sb.clear();
		if(roll_result) {
			sb.add("%Roll: ");
			for(auto i = 0; i < roll_result.getcount(); i++) {
				if(i != 0)
					sb.add(", ");
				auto v = roll_result[i];
				if(v >= sv)
					sb.add("[+%1i]", v);
				else
					sb.add("%1i", v);
			}
		} else
			sb.add(getnm("NoDicesForRoll"));
		an.add(0, getnm("ApplyRollResult"));
		if(get(Clue))
			an.add(add_clue, getnm("UseClueToAddDice"), 1);
		answers::prompt_ask = temp;
		auto p = (fnevent)an.open();
		if(!p)
			break;
		last = this;
		p();
	}
	return roll_success(getsuccess());
}