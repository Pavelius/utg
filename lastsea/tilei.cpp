#include "main.h"

const tilei* tilei::find(indext page) {
	for(auto& e : bsdata<tilei>()) {
		if(e.page == page)
			return &e;
	}
	return 0;
}