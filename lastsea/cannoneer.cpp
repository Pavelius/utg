#include "cannoneer.h"
#include "math.h"
#include "rand.h"

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

bool cannoneer::unloadgun(int level, bool run) {
	for(auto& e : guns) {
		if(!e)
			continue;
		if(!isloaded(e))
			continue;
		if(getlevel(e) > level)
			continue;
		if(run)
			e--;
		return true;
	}
	return false;
}

int cannoneer::getgunbonus(int level) {
	static int bonus[] = {0, 3, 5, 7, 9};
	return maptbl(bonus, level);
}

const char* cannoneer::getgunname(int index) {
	static const char* names[] = {"Gun1", "Gun2", "Gun3", "Gun4"};
	return maptbl(names, index);
}

int	cannoneer::getgunlevel(int n) const {
	if(n < 0 || n >= sizeof(guns) / sizeof(guns[0]))
		return 0;
	return getlevel(guns[n]);
}

bool cannoneer::is(int level, bool loaded) const {
	for(auto v : guns) {
		if(getlevel(v) != level)
			continue;
		if(isloaded(v) != loaded)
			continue;
		return true;
	}
	return false;
}

bool cannoneer::isgunloaded(int n) const {
	if(n < 0 || n >= sizeof(guns) / sizeof(guns[0]))
		return false;
	if(!guns[n])
		return false;
	return isloaded(guns[n]);
}