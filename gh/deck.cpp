#include "deck.h"

cardt deck::take() {
	if(count)
		return data[count--];
	return 0;
}