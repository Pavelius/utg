#include "main.h"

void packi::getinfo(stringbuilder& sb) const {
	auto m = elements.count;
	for(unsigned i = 0; i < m; i++) {
		if(i) {
			if(i == (m - 1))
				sb.add(" � ");
			else
				sb.add(", ");
		}
		sb.add(elements.begin()[i].getname());
	}
}