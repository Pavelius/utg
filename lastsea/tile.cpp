#include "crt.h"
#include "tile.h"

const tilei* tilei::find(int param) {
	for(auto& e : bsdata<tilei>()) {
		if(e.param == param)
			return &e;
	}
	return 0;
}