#pragma once

class stringbuilder;
enum gender_s : unsigned char;

void addact(stringbuilder& sbs, const char* name, gender_s gender, const char* format, const char* format_param);

