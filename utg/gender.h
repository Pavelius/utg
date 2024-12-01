#pragma once

enum gendern : unsigned char {
	NoGender, Male, Female
};
extern gendern last_gender;
struct genderi {
	const char*			id;
};