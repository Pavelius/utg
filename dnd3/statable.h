#pragma once

struct statable {
	char		abilities[64];
	unsigned	skills, skills_expert;
	unsigned	immunity, resistance, vulnerable;
	unsigned	proficiency, proficiency_expert;
	void		copy(const statable& v);
	bool		isimmunity(int material) const;
	bool		isresistance(int material) const;
	bool		isvulnerable(int material) const;
	void		update();
};
extern int ability_result;
extern int result_param[4];