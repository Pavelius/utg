#include "main.h"
#include "widget.h"

const quest* find_promt(int index);
void print(stringbuilder& sb, const variants& source);

static void print(stringbuilder& sb, ability_s v, int count) {
	auto& ei = bsdata<abilityi>::elements[v];
	auto negative = count < 0;
	if(ei.is(Negative))
		negative = !negative;
	sb.addsep(' ');
	if(negative)
		sb.add("[-");
	sb.add("%1%+2i", getnm(ei.id), count);
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

static void print(stringbuilder& sb, special_s v, int count) {
	auto& ei = bsdata<speciali>::elements[v];
	if(v == VisitManyTimes || v == VisitRequired)
		return;
	if(v >= Page000 && v <= Page900)
		return;
	if(v >= Tile000 && v <= Tile900)
		return;
	if(v >= IfEqual)
		return;
	sb.addsep(' ');
	switch(v) {
	case EatSupply:
		sb.add("[-%Supply%+1i]", -game.getmaximum(Eat));
		break;
	case ChooseCustom:
		print_choose(sb, find_promt(AnswerCustom + count));
		break;
	default:
		sb.add(getnm(ei.id), count);
		break;
	}
}

static void print(stringbuilder& sb, variant v) {
	switch(v.type) {
	case Ability: print(sb, (ability_s)v.value, v.counter); break;
	case Special: print(sb, (special_s)v.value, v.counter); break;
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
	for(auto i = Exploration; i <= Navigation; i = (ability_s)(i + 1)) {
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

void gamei::information(const char* format, ...) {
	utg::sb.addn("[+");
	utg::sb.addv(format, xva_start(format));
	utg::sb.add("]");
}

void gamei::warning(const char* format, ...) {
	utg::sb.addn("[-");
	utg::sb.addv(format, xva_start(format));
	utg::sb.add("]");
}

void gamei::information(ability_s v, int count) {
	auto& ei = bsdata<abilityi>::elements[v];
	auto negative = count < 0;
	if(ei.is(Negative))
		negative = !negative;
	if(negative)
		warning("%1%+2i", getnm(ei.id), count);
	else
		information("%1%+2i", getnm(ei.id), count);
}

static const messagei* find_message(variant type, int value) {
	for(auto& e : bsdata<messagei>()) {
		if(e.type == type && e.value == value)
			return &e;
	}
	return 0;
}

static void print_message(const char* format) {
	auto push_prompt = answers::prompt;
	char temp[4096]; stringbuilder sb(temp);
	answers::prompt = temp;
	game.actn(sb, format, 0);
	draw::pause();
	answers::prompt = push_prompt;
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

void player::background() const {
	if(!answers::interactive)
		return;
	auto type = variant(Class, classid);
	auto pn = find_message(type, 4);
	if(!pn)
		return;
	auto push_prompt = answers::prompt;
	char name[260]; stringbuilder sn(name);
	getname(sn);
	sn.add(" - ");
	sn.add(getnm("YourPirate"), getnm(getclass().id));
	auto push_header = answers::header;
	answers::header = name;
	char temp[4096]; stringbuilder sb(temp);
	actn(sb, pn->text, 0);
	answers::prompt = temp;
	while(!draw::isnext()) {
		answers an; add_events(an, type, 5);
		auto p = (const char*)an.choose(0, getnm("CloseHistory"));
		if(!p)
			break;
		print_message(p);
	}
	answers::header = push_header;
	answers::prompt = push_prompt;
}

void gamei::background() {
	if(answers::prompt != utg::sb.begin())
		return;
	game.player::background();
}

void player::epilog(int level) {
	auto pn = find_message(variant(Class, classid), level);
	if(!pn)
		return;
	auto& ei = getclass();
	act(pn->text);
	draw::pause();
}

void gamei::sfgetinfo(const void* object, stringbuilder& sb) {
	if(bsdata<quest>::have(object)) {
		auto p = (quest*)object;
		if(p->index >= AnswerEntry && p->index < AnswerEntry + 100)
			print(sb, p->text);
		else if(p->next == 0)
			print(sb, p->tags);
		else {
			auto ph = quest::find(p->next);
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
	if(bsdata<tilei>::have(object)) {
		auto pt = (tilei*)object;
		auto pq = quest::find(pt->param);
		if(!pq)
			return;
		auto pv = pq->getheader();
		if(pv)
			sb.add(pv);
	}
	else if(bsdata<quest>::have(object))
		sfgetinfo(object, sb);
	else
		utg::getstatus(object, sb);
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
	auto p = game.getgoal();
	if(p)
		draw::label(getnm(p->id), 0, p);
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

void initialize_information_widgets() {
	widget::add("ListOfGoals", listofgoals);
	widget::add("ListOfCharacters", listofcharacters);
	widget::add("ListOfRecords", listofrecords);
	widget::add("ListOfTreasures", listoftreasures);
	widget::add("ListOfCounters", listofcounters);
}