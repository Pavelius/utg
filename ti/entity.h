#include "ability.h"
#include "answers.h"
#include "indicator.h"
#include "nameable.h"
#include "tag.h"

#pragma once

struct actioncard;
struct playeri;
struct planeti;
struct systemi;
struct uniti;

enum color_s : unsigned char;
enum planet_trait_s : unsigned char;
enum unit_type_s : unsigned char;

struct entity : nameable {
	playeri*			player;
	entity*				location;
	unsigned short		flags;
	constexpr explicit operator bool() const { return id != 0; }
	void				add(answers& an);
	void				clear();
	void				exhaust();
	int					get(ability_s v) const;
	int					get(indicator_s v) const;
	const actioncard*	getactioncard() const;
	const char*			getid() const;
	playeri*			getenemy() const;
	const char*			getname() const;
	planeti*			getplanet() const;
	int					getproduction() const;
	color_s				getspeciality() const;
	int					getsumary(ability_s v) const;
	int					getsumary(unit_type_s v) const;
	systemi*			getsystem() const;
	planet_trait_s		gettrait() const;
	const uniti*		getunit() const;
	void				hit();
	bool				is(tag_s v) const;
	void				set(tag_s v, bool n);
	void				startcombat();
	void				status(int type, const char* format) const;
};