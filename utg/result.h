#pragma once

enum result_s : unsigned char {
	Fail, PartialSuccess, Success, CriticalSuccess
};
struct resulti {
	const char*			id;
};
extern result_s			last_result;
extern int				last_roll;
extern result_s			pbaroll(int b);
