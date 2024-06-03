#include "ability.h"
#include "indicator.h"
#include "nameable.h"
#include "tag.h"

#pragma once

struct componenti;
struct decki;
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
	unsigned			flags;
	constexpr explicit operator bool() const { return id != 0; }
	void				clear();
	void				discard();
	void				exhaust();
	void				focusing() const;
	int					get(ability_s v) const;
	int					get(indicator_s v) const;
	const componenti*	getcard() const;
	decki*				getdeck() const;
	const char*			getid() const;
	playeri*			getenemy() const;
	const char*			getname() const;
	planeti*			getplanet() const;
	playeri*			getplayer() const;
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
	bool				usedeck() const { return getdeck() != 0; }
};