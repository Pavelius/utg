#include "stringact.h"
#include "npc.h"

static char	console_text[512];
stringbuilder console(console_text);

class localstringact : public stringact {
	void addidentifier(const char* identifier) override {
		if(opponent && strcmp(identifier, "оппонентa") == 0)
			addof(opponent->getname());
		else if(opponent && strcmp(identifier, "оппоненту") == 0)
			addto(opponent->getname());
		else if(opponent && (equal(identifier, "оппонент") || equal(identifier, "враг") || equal(identifier, "opponent")))
			add(opponent->getname());
		else
			stringact::addidentifier(identifier);
	}
public:
	localstringact(stringbuilder& sb, const char* name, gender_s gender) : stringact(sb, name, gender) {}
};

void actable::actv(stringbuilder& sbs, const char* format, const char* param, char add_sep) const {
	localstringact sb(sbs, getname(), getgender());
	if(add_sep)
		sb.addsep(' ');
	sb.addv(format, param);
	sbs = sb;
}