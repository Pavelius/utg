#include "item.h"

int item::getbonus(actionn value) const {
	auto r = 0;
	auto n = bsdata<tagi>::source.getcount();
	for(size_t i = 0; i < n; i++) {
		if(is((tag_s)i))
			r += bsdata<tagi>::elements[i].bonus[value];
	}
	return r;
}

int item::getsuccess(actionn value) const {
	auto r = 0;
	auto n = bsdata<tagi>::source.getcount();
	for(size_t i = 0; i < n; i++) {
		if(is((tag_s)i))
			r += bsdata<tagi>::elements[i].success[value];
	}
	return r;
}

int	item::getcost() const {
	return geti().ob;
}

//void item::getbonus(stringbuilder& sb, action_s action, const char* prefix, const char* postfix) const {
//	auto bs = getbonus(action);
//	auto sc = getsuccess(action);
//	if(bs || sc) {
//		if(prefix)
//			sb.adds(prefix);
//		if(bs) {
//			sb.addsep(',');
//			sb.add("%+1iD", bs);
//		}
//		if(sc) {
//			sb.addsep(',');
//			sb.add("%+1iS", sc);
//		}
//		if(postfix)
//			sb.add(prefix);
//	}
//}