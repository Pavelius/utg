#include "main.h"

class nameableact : public stringbuilder {
	const char* name;

	void addidentifier(const char* identifier) override {
		if(name && (equal(identifier, "герой") || equal(identifier, "name")))
			add(name);
		else if(name && strcmp(identifier, "героя") == 0)
			addof(name);
		else if(name && strcmp(identifier, "герою") == 0)
			addto(name);
		else
			stringbuilder::addidentifier(identifier);
	}

public:

	nameableact(const stringbuilder& v, const char* id) : stringbuilder(v), name(id) {}

};

void nameable::actv(const char* format, const char* format_param, int level) {
	if(!answers::console)
		return;
	nameableact sb(*answers::console, getnm(id));
}