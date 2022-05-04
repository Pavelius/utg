#include "main.h"

bool cardpool::isdoubleclue(ability_s v) const {
	for(auto& e : source) {
		if(!e.isactive())
			continue;
		if(e.geti().isdoubleclue(v))
			return true;
	}
	return false;
}

bool cardpool::isrerollall(ability_s v) const {
	for(auto& e : source) {
		if(!e.isactive())
			continue;
		if(e.geti().isrerollall(v))
			return true;
	}
	return false;
}