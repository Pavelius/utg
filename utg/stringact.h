#pragma once

class stringbuilder;
enum gender_s : unsigned char;

void act_identifier(stringbuilder& sb, const char* identifier);
void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

extern const char	*act_name;
extern const char	*act_enemy_name;
extern gender_s		act_gender, act_enemy_gender;

