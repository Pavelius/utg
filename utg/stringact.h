#pragma once

class stringbuilder;
enum gender_s : unsigned char;

void act_identifier(stringbuilder& sb, const char* identifier);

extern const char*	act_name;
extern gender_s		act_gender;

