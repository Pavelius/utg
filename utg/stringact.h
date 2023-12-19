#pragma once

class stringbuilder;
enum gender_s : unsigned char;

//void act_identifier(stringbuilder& sb, const char* identifier);
//void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);
bool apply_name(const char* identifier, stringbuilder& sb, const char* name);
bool apply_gender(const char* identifier, stringbuilder& sb, gender_s gender);

extern const char	*act_name;
extern const char	*act_enemy_name;
extern gender_s		act_gender, act_enemy_gender;

