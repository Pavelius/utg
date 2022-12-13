#include "rangeable.h"

void rangeable::addrange(int v) {
	int i = range + v;
	if(i < Intimate)
		i = Intimate;
	else if(i > Far)
		i = Far;
	range = (tag_s)i;
}