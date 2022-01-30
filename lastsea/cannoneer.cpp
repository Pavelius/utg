#include "cannoneer.h"

bool cannoneer::addgun(int level, bool loaded, bool run) {
	for(auto& e : guns) {
		if(e)
			continue;
		if(run)
			e = getgun(level, loaded);
		return true;
	}
	return false;
}

bool cannoneer::reloadgun(int level, bool run) {
	for(auto& e : guns) {
		if(!e)
			continue;
		if(isloaded(e))
			continue;
		if(getlevel(e) > level)
			continue;
		if(run)
			e++;
		return true;
	}
	return false;
}

int	cannoneer::getgunlevel(int n) const {
	if(n < 0 || n >= sizeof(guns) / sizeof(guns[0]))
		return 0;
	return getlevel(guns[n]);
}

bool cannoneer::isgunloaded(int n) const {
	if(n < 0 || n >= sizeof(guns) / sizeof(guns[0]))
		return false;
	if(!guns[n])
		return false;
	return isloaded(guns[n]);
}