#include "calendar.h"

unsigned start_year, current_round;

int getyear() {
	return 0;
}

int getmonth() {
	return 0;
}

int getdayofweek() {
	return 0;
}

void add_hours(unsigned v) {
	current_round += v * 60 * 6;
}

void add_turns(unsigned v) {
	current_round += v * 60;
}

void add_rounds(unsigned v) {
	current_round += v;
}