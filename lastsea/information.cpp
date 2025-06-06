#include "main.h"
#include "stringvar.h"
#include "widget.h"

static const char* history_source[] = {
	"NoHistory",
	"Epilogue", "Epilogue", "Epilogue", "Prologue",
	"Event1", "Event2", "Event3", "Event4", "Event5"
};
const quest* find_promt(int index);
void print(stringbuilder& sb, const variants& source);

void print(stringbuilder& sb, const char* id, int count, unsigned flags, char sep = ' ') {
	auto negative = count < 0;
	if((flags & FG(Negative)) != 0)
		negative = !negative;
	sb.addsep(sep);
	if(negative)
		sb.add("[-");
	if(count==-100)
		sb.add(getnm("RemoveAll"), getnm(id));
	else
		sb.add("%1%+2i", getnm(id), count);
	if(negative)
		sb.add("]");
}

static void print_choose(stringbuilder& sb, const quest* ph) {
	if(!ph)
		return;
	auto pe = bsdata<quest>::end();
	auto promt = "";
	auto separator = getnm("Or");
	for(auto p = ph + 1; p < pe && !p->next; p++) {
		if(!p->tags)
			continue;
		if(promt) {
			if(promt[0])
				sb.adds(promt);
			promt = 0;
		} else if(separator)
			sb.adds(separator);
		print(sb, p->tags);
	}
}

void scripti::getinfo(stringbuilder& sb, int bonus) const {
	if(!is(TipsInfo))
		return;
	sb.addsep(' ');
	if(info)
		info(sb, *this, bonus);
	else if(is(TipsInfoBonus))
		print(sb, getnm(id), bonus, flags);
	else
		sb.add(getnm(id), bonus);
}

void abilityi::getinfo(stringbuilder& sb, int bonus) const {
	if(!is(TipsInfo))
		return;
	sb.addsep(' ');
	print(sb, getnm(id), bonus, flags);
}

static void print(stringbuilder& sb, variant v) {
	switch(v.type) {
	case Ability: bsdata<abilityi>::elements[v.value].getinfo(sb, v.counter); break;
	case Script: bsdata<scripti>::elements[v.value].getinfo(sb, v.counter); break;
	default: break;
	}
}

void print(stringbuilder& sb, const variants& source) {
	for(auto v : source)
		print(sb, v);
}

static void print(stringbuilder& sb, const char* format) {
	sb.addv(format, 0);
}

static void printab(stringbuilder& sb, char* ability, const char* promt = 0) {
	for(auto i = Exploration; i <= Navigation; i = (abilityn)(i + 1)) {
		auto v = ability[i - Exploration];
		if(v) {
			if(promt) {
				sb.adds(promt);
				promt = 0;
			}
			print(sb, variant(Ability, i, v));
		}
	}
}

static const messagei* find_message(variant type, int value) {
	for(auto& e : bsdata<messagei>()) {
		if(e.type == type && e.value == value)
			return &e;
	}
	return 0;
}

static void add_events(answers& an, variant type, int level) {
	char temp[32]; stringbuilder sb(temp);
	for(auto i = 0; i < level; i++) {
		auto pn = find_message(type, 5 + i);
		if(!pn)
			continue;
		sb.clear(); sb.add("Event%1i", i + 1);
		an.add(pn->text, getnm(temp));
	}
}

static void set_pirate_header(stringbuilder& sb) {
	game.getname(sb);
	sb.add(" - ");
	sb.add(getnm("YourPirate"), getnm(game.getclass().id));
	answers::header = sb.begin();
}

static void print_message(const char* format, const char* button, const variants* tags = 0) {
	utg::sb.clear();
	game.actn(utg::sb, format, 0);
	if(tags)
		game.apply(*tags);
	pausenc(button, game.getname());
}

static void print_message_header(const char* format, const char* button) {
	auto push_header = answers::header;
	char temp[260]; stringbuilder sn(temp);
	set_pirate_header(sn);
	print_message(format, button);
	answers::header = push_header;
}

void player::background() {
	if(!answers::interactive)
		return;
	auto type = variant(Class, game.classid);
	auto pn = find_message(type, 4);
	if(!pn)
		return;
	auto push_mode = menu::current_mode;
	auto push_prompt = answers::prompt;
	auto push_header = answers::header;
	char name[260]; stringbuilder sn(name);
	set_pirate_header(sn);
	menu::current_mode = "PirateHistory";
	answers::header = name;
	char temp[4096]; stringbuilder sb(temp);
	game.actn(sb, pn->text, 0);
	answers::prompt = temp;
	while(!draw::isnext()) {
		answers an; add_events(an, type, game.get(History));
		auto p = (const char*)an.choose(0, getnm("CloseHistory"), true);
		if(!p)
			break;
		print_message(p, getnm("CloseHistory"));
	}
	answers::header = push_header;
	answers::prompt = push_prompt;
	menu::current_mode = push_mode;
}

void player::epilog(int level, bool apply_tags) {
	auto pn = find_message(variant(Class, classid), level);
	if(!pn)
		return;
	auto push_header = answers::header;
	answers::header = getnm(maptbl(history_source, level));
	const char* cancel = "CloseHistory";
	if(level <= 2)
		cancel = "EndAdventure";
	const variants* tags = 0;
	if(apply_tags)
		tags = &getclass().getevent(level);
	print_message(pn->text, getnm(cancel), tags);
	answers::header = push_header;
}

void gamei::sfgetinfo(const void* object, stringbuilder& sb) {
	if(bsdata<quest>::have(object)) {
		auto p = (quest*)object;
		if(p->index >= AnswerEntry && p->index < AnswerEntry + 100)
			print(sb, p->text);
		else if(p->next == 0)
			print(sb, p->tags);
		else {
			auto ph = quest_find(p->next);
			if(ph)
				print(sb, p->tags);
		}
	}
}

void treasurei::sfgetinfo(const void* object, stringbuilder& sb) {
	auto p = (treasurei*)object;
	auto pn = getdescription(p->id);
	if(pn)
		sb.adds(pn);
	switch(p->trigger) {
	case WhenUse:
		sb.adds(getnm("DiscardToGain"));
		print(sb, p->use);
		break;
	case WhenAbilityIncreased:
	case WhenAbilityDecreased:
		sb.adds(getnm(bsdata<triggeri>::elements[p->trigger].id), getnm(bsdata<abilityi>::elements[p->ability].id));
		print(sb, p->use);
		break;
	case WhenRoll:
		sb.adds(getnm("BeforeAbilityRoll"), getnm(bsdata<abilityi>::elements[p->ability].id));
		print(sb, p->use);
		sb.adds(getnm("To"));
		sb.adds(getnm("GainBonusToRollResult"), p->bonus);
		break;
	}
	printab(sb, p->abilities, getnm("Skills"));
}

void gamei::sfgetstatus(const void* object, stringbuilder& sb) {
	auto pg = static_cast<goal*>(&game);
	if(pg == object) {
		auto pn = pg->getgoaltext();
		if(pn)
			sb.add(pn);
	} else if(game.getplayer(0)==object) {
		sb.add(getdescription("YourPirate"), getnm(game.getclass().id));
	} else if(bsdata<tilei>::have(object)) {
		auto pt = (tilei*)object;
		auto pq = quest_find(pt->param);
		if(!pq)
			return;
		auto pv = pq->getheader();
		if(pv)
			sb.add(pv);
	} else if(bsdata<quest>::have(object))
		sfgetinfo(object, sb);
	else
		utg::getstatus(object, sb);
}

void pirate::sfgetproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (pirate*)object;
	int value, value2;
	switch(v.type) {
	case Ability:
		switch(v.value) {
		case Stars:
			value = p->get((abilityn)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((abilityn)v.value));
			value = p->getnextstar(value);
			if(value)
				sb.adds("(%1 %2i)", getnm("NextStar"), value);
			break;
		case Crew:
			value = p->get((abilityn)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((abilityn)v.value));
			value = p->get(Discontent);
			if(value)
				sb.adds(getnm("ShowDiscontent"), value);
			break;
		case Reroll: case Misfortune:
			value = p->get((abilityn)v.value);
			if(value)
				sb.add("%1i", value);
			break;
		default:
			value = p->get((abilityn)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((abilityn)v.value));
			value2 = p->getbonus((abilityn)v.value);
			if(value2)
				sb.adds("(%-IncludeItems %1i)", value + value2);
			break;
		}
		break;
	}
}

void gamei::sfgetproperty(const void* object, variant v, stringbuilder& sb) {
	pirate::sfgetproperty(static_cast<pirate*>(&game), v, sb);
}

static void listoftreasures() {
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		if(e.is(Valuable)) {
			sb.clear();
			sb.add("[%1]", getnm(e.id));
			draw::label(temp, 0, &e);
		} else
			draw::label(getnm(e.id), 0, &e);
	}
}

static void listofrecords() {
	for(unsigned i = AnswerEntry; i < AnswerEntry + 50; i++) {
		if(!game.istag(i))
			continue;
		draw::label(game.getentryname(i), 0, game.getentry(i));
	}
}

static void listofgoals() {
	auto pn = game.getgoalname();
	if(pn)
		draw::label(pn, 0, static_cast<goal*>(&game));
}

static void listofcounters() {
	char temp[64]; stringbuilder sb(temp);
	for(auto i = 0; i < variables.getcount(); i++) {
		auto pn = variables.getname(i);
		if(!pn)
			continue;
		auto v = variables.get(i);
		sb.clear(); sb.add("%1i", v);
		draw::label(pn, temp, 0);
	}
}

static void showlabel(npcname& e, fnevent proc, const char* format, ...) {
	char temp[260]; stringbuilder sb(temp);
	e.getname(sb);
	if(format) {
		sb.add(" - ");
		sb.addv(format, xva_start(format));
	}
	draw::label(temp, 0, &e, proc);
}

static void listofcharacters() {
	showlabel(game, game.background, getnm("YourPirate"), getnm(game.getclass().id));
	for(auto& e : game.getfriends())
		showlabel(e, 0, 0);
}

static void listofguns() {
	char temp[260]; stringbuilder sb(temp);
	for(auto i = 0; i < 4; i++) {
		auto level = game.getgunlevel(i);
		auto loaded = game.isgunloaded(i);
		if(level > 0) {
			sb.clear(); sb.add(getnm("GunStatus"), level, getnm(loaded ? "Loaded" : "Unloaded"));
			draw::label(getnm(game.getgunname(i)), temp, 0);
		}
	}
}

static void addvname(stringbuilder& sb, variant v, int padeg = 0) {
	if(!v)
		return;
	if(v.type == Group && v.value <= 1) {
		game.friends[v.value].getname(sb);
		return;
	}
	auto pv = (groupvaluei*)v;
	if(pv) {
		switch(padeg) {
		case 1: sb.addby(pv->name); break;
		case 2: sb.addof(pv->name); break;
		default: sb.add(pv->name); break;
		}
	}
}

static void show_values(stringbuilder& sb, const player& source) {
	auto index = 0;
	for(auto v : source.values) {
		if(v) {
			sb.addn("* ");
			sb.add(game.getclass().types[index].getname());
			sb.add(": ");
			addvname(sb, v);
		}
		index++;
	}
	sb.addsep('\n');
}

static void show_crew(stringbuilder& sb, const char* separator, const char* padding, bool hilite) {
	for(auto& e : game.getfriends()) {
		sb.addn(padding);
		if(hilite)
			sb.add("[");
		e.getname(sb);
		if(hilite)
			sb.add("]");
		sb.add(separator);
	}
}

static void show_values(stringbuilder& sb) {
	show_values(sb, game);
}

static void show_crew(stringbuilder& sb) {
	show_crew(sb, "\n", "* ", true);
}

static void show_value_1(stringbuilder& sb) {
	addvname(sb, game.values[0], 0);
}

static void show_value_2(stringbuilder& sb) {
	addvname(sb, game.values[1], 0);
}

static void show_value_3(stringbuilder& sb) {
	addvname(sb, game.values[2], 0);
}

static void show_value_4(stringbuilder& sb) {
	addvname(sb, game.values[3], 0);
}

static void show_value_5(stringbuilder& sb) {
	addvname(sb, game.values[4], 0);
}

static void show_class(stringbuilder& sb) {
	sb.add(getnm(game.getclass().id));
}

static void show_name(stringbuilder& sb) {
	game.getactive().getname(sb);
}

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gender_s gender);

static void custom_string(stringbuilder& sb, const char* identifier) {
	if(stringvar_identifier(sb, identifier))
		return;
	apply_action(identifier, sb, game.getname(), game.getgender());
}

BSDATA(stringvari) = {
	{"Class", show_class},
	{"name", show_name},
	{"ShowCrew", show_crew},
	{"ShowValues", show_values},
	{"Value1", show_value_1},
	{"Value2", show_value_2},
	{"Value3", show_value_3},
	{"Value4", show_value_4},
	{"Value5", show_value_5},
	{"�����", show_name},
};
BSDATAF(stringvari)

void initialize_information_widgets() {
	stringbuilder::custom = custom_string;
	widget::add("ListOfGoals", listofgoals);
	widget::add("ListOfGuns", listofguns);
	widget::add("ListOfCharacters", listofcharacters);
	widget::add("ListOfRecords", listofrecords);
	widget::add("ListOfTreasures", listoftreasures);
	widget::add("ListOfCounters", listofcounters);
}