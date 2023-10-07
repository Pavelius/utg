#include "crt.h"
#include "province.h"
#include "structure.h"

provincei* province;

void initialize_province() {
	auto result = 0;
	for(auto& e : bsdata<provincei>())
		e.index = result++;
}