#include "crt.h"

#pragma once

enum trait_s : unsigned char {
	Bigpaw, Bitter, Bodyguard, Bold, Brave,
	Calm, Clever, Compassionate, Cunning, Curious,
	DeepEar, Defender, Determined, Driven, EarlyRiser,
	Extrovert, Fat, Fearful, Fearless, Fiery,
	Generous, Graceful, GuardsHonor, Innocent, Jaded,
	Leader, Longtail, Lost, NaturalBearings, Nimble,
	Nocturnal, Oldfur, QuickWitted, Quiet, Scarred,
	SharpEyed, Sharptooth, Short, Skeptical, Skinny,
	Stoic, Stubborn, Suspicious, Tall, Thoughtful,
	Tough, WeatherSense, WiseTrait, WolfsSnout, Young,
	Alert, HardWorker, Independent, OpenMinded, SteadyPaw, Inquisitive, Rational,
	FirstTraits = Bigpaw, LastTraits = Rational,
};
enum animal_s : unsigned char {
	Badger, Bear, Crab, Deer, Fox, Frog, Hawk, Moose, Mouse, Owl, Racoon, Snake, Squirrel, Turtle, Weasel, Wolf,
};
enum wise_s : unsigned char {
	BirdsWise, BurrowWise, MammalsWise, PredatorWise,
	CoastWise, ForestWise, LakeWise, RockyTerrainWise, SwampWise, TallGrassWise,
	CraftWise, CrimeWise, GovernorWise, GuardmouseWise,
	RainWise, ClearWeatherWise, SnowWise,
	BarkstoneWise, CooperwoodWise, DarkHeatherWise, ElmossWise, IvydaleWise, LockhavenWise, PortSumacWise, ShaleburrowWise, SprucetuckWise,
	FirstWise = BirdsWise, LastWise = SprucetuckWise
};
enum conflict_s : unsigned char {
	FightConflict,
};
enum roll_type_s : unsigned char {
	StandartRoll, ConflictRoll, RecoveryRoll,
};
enum weather_s : unsigned char {
	ClearAndWarm, SpringSnow, SpringRain, SpringStorms, SpringUnseasonablyCold, SpringUnseasonablyWarm,
	WarmAndHumid, SummerRain, Thunderstorms, HeatWaves,
	AutumStorms, ColdRain, AutumUnseasonablyCold, AutumUnseasonablyWarm,
	ClearAndCold, Snow, Blizzard, ColdSnap, IceStorm, WinterUnseasonablyWarm,
	FirstWeather = ClearAndWarm, LastWeather = WinterUnseasonablyWarm,
};
enum special_s : unsigned char {
	Bonus, Penalty, Success,
};
enum wear_s : unsigned char {
	Hands, Offhand, Body,
	FirstGear, LastGear = FirstGear + 3,
};
enum roll_flag_s : unsigned char {
	MarkExperience, RecoveryTest, ActWithNature,
};
//struct wisei {
//	const char*					id;
//	const char*					name;
//	variant						subject;
//};
//struct traiti {
//	const char*					id;
//	const char*					name;
//	skill_s						bonus[4];
//};
//struct conflicti {
//	const char*					id;
//	const char*					name;
//	skill_s						skills[2][4];
//};
//struct locationi {
//	const char*					id;
//	skilla						skills;
//	traita						traits;
//	wise_s						wise;
//	location_size_s				size;
//};
//struct landscapei {
//	const char*					id;
//	const char*					name;
//	wise_s						wise;
//};
//class hero : public nameable {
//	char						checks, fate, persona;
//	unsigned char				conditions;
//	char						skills[LastSkill + 1];
//	char						traits[LastTraits + 1];
//	flagable<LastTraits>		trait_used;
//	char						fail[LastSkill + 1];
//	char						pass[LastSkill + 1];
//	wisea						wises;
//	item						wears[LastGear + 1];
//	unsigned short				family_id;
//	unsigned char				age;
//	rang_s						rang;
//	skill_s						specialization;
//	location_s					homeland;
//	//
//	void						tallyskills();
//public:
//	void						addplayer();
//	void						buyeqipment();
//	bool						canhelp(skill_s value, skill_s* result = 0) const;
//	static hero*				choose(skill_s skill);
//	static hero*				choose(bool interactive, bool (hero::*proc)() const);
//	static rang_s				chooserang(bool interactive);
//	void						create(animal_s type);
//	void						create(animal_s type, gender_s gender, skill_s specialization, location_s homeland);
//	void						create(rang_s rang, bool interactive);
//	static void					fight(animal_s type);
//	static action_roll_s		get(action_s player, action_s opposition);
//	int							get(skill_s value) const;
//	int							get(trait_s value) const { return traits[value]; }
//	static void					get(adat<condition_s, 8>& conditions, skill_s skill);
//	item&						get(wear_s v) { return wears[v]; }
//	void						getinfo(stringbuilder& sb) const;
//	location_s					gethomeland() const { return homeland; }
//	static const char*			getnameby(action_s value);
//	hero*						getparent() const { return (family_id==Blocked) ? 0 : bsmeta<hero>::elements + family_id; }
//	skill_s						getspecial() const { return specialization; }
//	static int					getobstacle(season_s value);
//	static void					gonext();
//	bool						is(condition_s value) const;
//	bool						is(wise_s v) const { return wises.is(v); }
//	bool						isalive() const { return !is(Dead); }
//	static bool					isbonus(trait_s base, skill_s value);
//	bool						ischeck() const { return checks >= 1; }
//	bool						ischeck2() const { return checks >= 2; }
//	bool						isconditions() const;
//	static bool					isfactor(weather_s object, skill_s value);
//	bool						isfreegear() const;
//	static bool					ismatch(landscape_s id, wise_s value);
//	static bool					ismatch(location_s value, wise_s wise);
//	static bool					ismatch(animal_s value, wise_s wise);
//	static bool					ismatch(bool (hero::*proc)() const);
//	bool						isplayer() const;
//	static bool					passtest(skill_s skill, int obstacle);
//	static void					quest(const char* name);
//	int							roll(skill_s value, int obstacle, int bonus_dices = 0, int bonus_success = 0, bool interactive = true);
//	int							roll(roll_type_s roll_type, heroa& allies, heroa& helpers, bool interactive, skill_s value, int obstacle, int bonus_dices = 0, int bonus_success = 0, hero* opponent = 0, skill_s opponent_skill = Nature, int opponent_bonus_dices = 0, int opponent_bonus_success = 0);
//	bool						rollresource(int obstacle, bool interactive = true);
//	void						recover();
//	void						recover(condition_s value);
//	void						remove(condition_s value);
//	void						set(condition_s value);
//	void						set(wear_s i, item v) { wears[i] = v; }
//	void						set(rang_s rang);
//	void						set(skill_s value, int number);
//	void						set(trait_s value, int number) { traits[value] = number; }
//	void						set(wise_s v) { wises.set(v); }
//	void						setfamily(const hero* v);
//	void						sethomeland(location_s v) { homeland = v; }
//	void						setspecial(skill_s v) { specialization = v; }
//	static void					playersturn();
//	static void					twistconditions(bool interactive, skill_s skill, heroa& helps);
//	static void					twistweather(bool interactive, skill_s skill, heroa& helps);
//	void						use(trait_s value);
//	static void					weatherwatch();
//};
//struct order {
//	action_s					action;
//	hero*						actor;
//	item*						weapon;
//};
//class squadi : public heroa {
//	short unsigned				year;
//	short unsigned				year_index;
//	static season_s				year_cicle[14];
//	weather_s					year_weather[14];
//	variant						location;
//	hero*						opposition;
//	//
//	bool						stage();
//public:
//	void						addweather();
//	void						clear();
//	short unsigned				getnext() const { return (year_index + 1) % (sizeof(year_cicle) / sizeof(year_cicle[0])); }
//	season_s					getseason() const { return year_cicle[year_index]; }
//	static season_s				getseason(short unsigned i) { return year_cicle[i]; }
//	weather_s					getweather() const { return year_weather[year_index]; }
//	short unsigned				getyear() const { return year; }
//	bool						match(const twisti& e) const;
//	bool						match(wise_s v) const;
//	void						play();
//	bool						play(const twisti& e);
//	void						set(variant v);
//	void						set(short unsigned index, weather_s v) { year_weather[index] = v; }
//	void						setopponent(hero* v) { opposition = v; }
//	void						setyearweather();
//};