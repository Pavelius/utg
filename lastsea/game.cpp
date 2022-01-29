#include "main.h"
#include "pathfind.h"

gamei		game;
int			last_choose, last_page, last_scene, last_tile;
static bool	need_sail;
ability_s	last_ability;
static fnevent next_round;

static void generate_classes() {
	auto push_interactive = utg::interactive;
	utg::interactive = false;
	for(auto& e : bsdata<pirate>())
		e.generate();
	utg::interactive = push_interactive;
}

void gamei::clear() {
	memset(this, 0, sizeof(*this));
	shiplog::clear();
}

void gamei::generate() {
	for(auto& e : game.pirates) {
		e.clear();
		e.randomname();
	}
	game.pirate::clear();
	game.pirate::randomname();
	game.pirate::generate();
}

void gamei::choosehistory() {
	game.pirate::choosehistory();
	utg::sb.clear();
}

static void fixerror(const char* id, ...) {
	utg::sb.addn("[-");
	utg::sb.addv(getnm(id), xva_start(id));
	utg::sb.addn("]");
}

static void special_command(special_s v, int bonus) {
	switch(v) {
	case Roll:
		game.roll();
		break;
	case Choose:
		last_choose = bonus;
		break;
	case Skill:
		game.raiseskills(bonus);
		break;
	case Scout:
		game.chartacourse(bonus);
		break;
	case SetShip:
		game.setmarker(bonus);
		break;
	case Tile000: last_tile = bonus; break;
	case Tile900: last_tile = 900 + bonus; break;
	case TileRock: last_tile = pathfind::Blocked; break;
	case AddTile:
		game.setlocation(bonus, last_tile);
		break;
	case Page000: last_page = bonus; break;
	case Page100: last_page = 100 + bonus; break;
	case Page200: last_page = 200 + bonus; break;
	case Page300: last_page = 300 + bonus; break;
	case Page400: last_page = 400 + bonus; break;
	case Page500: last_page = 500 + bonus; break;
	case Page600: last_page = 600 + bonus; break;
	case Page700: last_page = 700 + bonus; break;
	case Page800: last_page = 800 + bonus; break;
	case Page900: last_page = 900 + bonus; break;
	case Scene:
		if(!bonus)
			last_scene = game.scene;
		else
			last_scene = bonus;
		break;
	case AddGun:
		if(game.addgun(bonus, true, true))
			game.information(getnm("GunAdded"), bonus);
		break;
	case AddGunUnloaded:
		if(game.addgun(bonus, false, true))
			game.information(getnm("GunAddedUnloaded"), bonus);
		break;
	case ReloadGun:
		game.reloadgun(bonus, true);
		break;
	case Bury:
		game.bury(bonus);
		break;
	case Block:
		if(bonus > 0)
			game.lock(bonus - 1);
		else if(bonus < 0)
			game.unlock(bonus - 1);
		break;
	case PaySupply:
		if(game.get(Supply) >= bonus)
			game.set(Supply, game.get(Supply) - bonus);
		break;
	case PaySupplyEat:
		bonus += game.getmaximum(Eat);
		if(game.get(Supply) >= bonus)
			game.set(Supply, game.get(Supply) - bonus);
		break;
	case ZeroSupplyOrDiscontent:
		if(game.get(Supply) == 0)
			game.set(Discontent, game.get(Discontent) + bonus);
		else
			game.set(Supply, 0);
		break;
	case FullThrottle:
		game.fullthrottle(bonus);
		break;
	case TradeFriend:
		game.tradefriend();
		break;
	case Sail:
		need_sail = true;
		break;
	default:
		break;
	}
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability:
		last_ability = (ability_s)v.value;
		if(v.counter)
			game.set(last_ability, game.get(last_ability) + v.counter);
		break;
	case Special:
		special_command((special_s)v.value, v.counter);
		break;
	case Card:
		game.gaintreasure((treasurei*)v.getpointer());
		break;
	case Goal:
		game.setgoal((goali*)v.getpointer());
		break;
	}
}

void gamei::apply(const variants& source) {
	last_ability = Infamy;
	for(auto v : source)
		apply(v);
}

void gamei::createtreasure() {
	auto m = bsdata<treasurei>::source.getcount();
	for(auto& e : bsdata<treasurei>()) {
		if(e.ismagic())
			continue;
		if(e.isstory())
			continue;
		treasures.add(bsdata<treasurei>::source.indexof(&e));
	}
	zshuffle(treasures.data, treasures.count);
}

void gamei::createtiles() {
	tiles.clear();
	for(auto i = 1; i <= 30; i++)
		tiles.add(i);
	zshuffle(tiles.data, tiles.count);
}

indext gamei::picktile() {
	if(!tiles)
		return 0;
	auto i = tiles.data[0];
	tiles.remove(0);
	return i;
}

const treasurei* gamei::picktreasure() {
	if(!treasures.count)
		return 0;
	auto pv = bsdata<treasurei>::elements + treasures.data[0];
	treasures.remove(0);
	return pv;
}

void gamei::sfgetproperty(const void* object, variant v, stringbuilder& sb) {
	switch(v.type) {
	case Ability:
		if(v.value >= Gun1 && v.value <= Gun4) {
			auto level = game.getgunlevel(v.value - Gun1);
			auto loaded = game.isgunloaded(v.value - Gun1);
			if(level > 0)
				sb.add(getnm("GunStatus"), level, getnm(loaded ? "Loaded" : "Unloaded"));
		} else
			pirate::sfgetproperty(static_cast<pirate*>(&game), v, sb);
		break;
	}
}

const quest* gamei::findpage(int v) {
	for(auto& e : bsdata<quest>()) {
		if(e.index == v)
			return &e;
	}
	return 0;
}

void gamei::playscene() {
	auto scene = game.scene;
	game.chooseactions(scene);
	game.playactions();
	game.endscene(scene);
}

void gamei::playsail() {
	auto index = game.oceani::chooseroute(0, 1);
	if(index == pathfind::Blocked)
		return;
	game.setmarker(index);
	auto tile_id = game.getlocation(index);
	if(!tile_id) {
		game.setlocation(index, game.picktile());
		game.createobjects();
		game.showsplash();
	}
	tile_id = game.getlocation(index);
	if(!tile_id)
		return;
	game.adventure(tile_id);
	utg::pause();
	game.afterapply();
}

void gamei::fullthrottle(int level) {
}

void gamei::chartacourse(int count) {
	while(count > 0) {
		char temp[260]; stringbuilder sb(temp);
		sb.add(getnm("PlaceNavigationTile"), count);
		createobjects();
		showsplash();
		addpossiblecourse();
		showsplash();
		auto index = choose(temp);
		auto tile = picktile();
		if(!tile)
			break;
		setlocation(index, tile);
		count--;
	}
	createobjects();
	showsplash();
}

void gamei::showseamap() {
	game.oceani::showseamap();
}

void gamei::endscene(int scene) {
	last_scene = 0;
	auto ph = findpage(4000 + scene);
	if(ph && ph->next) {
		game.adventure(ph->next);
		game.afterapply();
	}
}

void gamei::afterapply() {
	while(last_page || last_choose) {
		if(last_page > 0) {
			auto v = last_page;
			last_page = 0;
			game.adventure(v);
			continue;
		}
		if(last_choose > 0) {
			auto v = last_choose;
			last_choose = 0;
			game.playchoose(v);
			continue;
		}
	}
	if(last_scene) {
		if(game.scene != last_scene) {
			game.unlockall();
			game.scene = last_scene;
		}
		last_scene = 0;
		draw::setnext(game.playscene);
	} else if(need_sail) {
		need_sail = false;
		draw::setnext(game.playsail);
	}
}