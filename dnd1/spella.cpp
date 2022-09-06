#include "main.h"

void spella::known(const creature& source) {
	auto ps = begin();
	auto pe = endof();
	for(auto i = (spell_s)0; i <= LastSpell; i = (spell_s)(i + 1)) {
		if(source.isknown(i)) {
			if(ps < pe)
				*ps = i;
		}
	}
	count = ps - data;
}

void spella::prepared(const creature& source) {
	auto ps = begin();
	auto pe = endof();
	for(auto i = (spell_s)0; i <= LastSpell; i = (spell_s)(i + 1)) {
		if(source.spells[i]) {
			if(ps < pe)
				*ps = i;
		}
	}
	count = ps - data;
}