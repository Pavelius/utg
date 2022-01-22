#include "main.h"
#include "stringact.h"

class historyable::string : public stringact {
	const historyable& source;
	void addvname(variant v) {
		add(v.getname());
	}
	void addidentifier(const char* identifier) override {
		if(equal(identifier, "Value1"))
			addvname(source.values[0]);
		else if(equal(identifier, "Value2"))
			addvname(source.values[1]);
		else if(equal(identifier, "Value3"))
			addvname(source.values[2]);
		else if(equal(identifier, "Value4"))
			addvname(source.values[3]);
		else if(equal(identifier, "Value5"))
			addvname(source.values[4]);
		else if(equal(identifier, "герой"))
			source.getname(*this);
		else
			stringact::addidentifier(identifier);
	}
public:
	string(const historyable& source, const stringbuilder& sb) :
		source(source),
		stringact(sb, source.getname(), source.getgender()) {
	}
};

void historyable::clear() {
	memset(this, 0, sizeof(*this));
	classid = 0xFFFF;
	npcname::clear();
}

void historyable::actv(stringbuilder& sbs, const char* format, const char* format_param) const {
	string sb(*this, sbs);
	sb.addsep(' ');
	sb.addv(format, format_param);
	sbs = sb;
}

void historyable::chooseclass() {
	answers an;
	for(auto& e : bsdata<classi>()) {
		if(game.match(&e))
			continue;
		an.add((void*)(&e - bsdata<classi>::elements), getnm(e.id));
	}
	char temp[260]; temp[0] = 0;
	stringbuilder sb(temp);
	act(sb, getnm("ChooseClass"));
	classid = (int)an.choose(temp, 0, logs::interactive, logs::url, -1, logs::header);
}