#include "deck.h"

cardt deck::take() {
	if(count)
		return data[count--];
	return 0;
}

void deck::discard(cardt v) {
	if(count >= getmaximum())
		return;
	memmove(data + 1, data, count*sizeof(data[0]));
	data[0] = v;
	count++;
}