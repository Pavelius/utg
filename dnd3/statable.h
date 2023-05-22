#pragma once

struct statable {
	char		abilities[64];
	unsigned	skills, skills_expert;
	unsigned	proficiency, proficiency_expert;
	void		update();
};
extern int ability_result;
extern int result_param[4];
