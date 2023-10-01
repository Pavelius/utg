#include "answers.h"
#include "move.h"
#include "result.h"
#include "crt.h"
#include "logparse.h"
#include "script.h"
#include "vagabond.h"

using namespace log;

BSDATA(movei) = {
	{"Acrobatics", RoguishFeat, Finess},
	{"Blindside", RoguishFeat, Finess},
	{"Counterfeit", RoguishFeat, Finess},
	{"DisableDevice", RoguishFeat, Finess},
	{"Hide", RoguishFeat, Finess},
	{"PickLock", RoguishFeat, Finess},
	{"PickPocket", RoguishFeat, Finess},
	{"SleightOfHand", RoguishFeat, Finess},
	{"Sneak", RoguishFeat, Finess},
	{"FigureSomeoneOut", BasicMove, Charm},
	{"PersuadeNPC", BasicMove, Charm},
	{"ReadTenseSituation", BasicMove, Cunning},
	{"TrickNPC", BasicMove, Cunning},
	{"TrustFate", BasicMove, Luck},
	{"WreckSomething", BasicMove, Might},
	{"EngageMelee", BasicMove, Might},
	{"GrappleEnemy", BasicMove, Might},
	{"TargetSomeone", BasicMove, Finess},
	{"Cleave", WeaponSkill, Might},
	{"ConfuseSenses", WeaponSkill, Finess},
	{"Disarm", WeaponSkill, Finess},
	{"HarryGroup", WeaponSkill, Cunning},
	{"ImproviseWeapon", WeaponSkill, Cunning},
	{"Parry", WeaponSkill, Finess},
	{"QuickShot", WeaponSkill, Luck},
	{"StormGroup", WeaponSkill, Might},
	{"TrickShot", WeaponSkill, Finess},
	{"ViciousStrike", WeaponSkill, Might},
	{"AskFavor", ReputationMove, Reputation},
	{"MeetSomeoneImportant", ReputationMove, Reputation},
	{"DrawAttention", ReputationMove, Reputation},
	{"SwayNPC", ReputationMove, Reputation},
	{"MakePointedThread", ReputationMove, Reputation},
	{"CommandResources", ReputationMove, Reputation},
};
assert_enum(movei, CommandResources)

const movei* lastmove;
int movei::choose_count;

static const char* read_move(const char* p, movei*& pm, stringbuilder& sb) {
	unsigned short result = 0xFFFF;
	p = readval(p, sb, bsdata<movei>::source, result, "move");
	if(result != 0xFFFF)
		pm = bsdata<movei>::elements + result;
	p = skipsp(p);
	if(checksym(p, '\n'))
		p = skipspcr(p);
	return p;
}

static const char* read_effect(const char* p, variants& result, stringbuilder& sb) {
	result.clear();
	if(*p != '{')
		return p;
	p = skipsp(p + 1);
	if(!result.count)
		result.start = bsdata<variant>::source.getcount();
	while(allowparse && ischa(*p)) {
		auto pv = bsdata<variant>::add(); pv->clear();
		p = readval(p, sb, *pv);
		p = skipsp(p);
		result.count++;
	}
	if(checksym(p, '}'))
		p = p + 1;
	return skipsp(p);
}

static const char* read_options(const char* p, stringbuilder& sb) {
	auto ps = bsdata<moveoptioni>::add();
	ps->clear();
	ps->next = 0xFFFF;
	if(ischa(*p)) {
		p = readval(p, sb, bsdata<resulti>::source, ps->index);
	} else if(isnum(*p)) {
		int result = 0;
		p = sb.read(p, result);
		ps->index = result;
	} else
		ps->index = 0xFFFF;
	p = skipsp(p);
	p = read_effect(p, ps->effect, sb);
	p = readstr(p, sb);
	ps->text = szdup(sb.begin());
	return skipspcr(p);
}

void movei::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		movei* move = 0;
		p = read_move(p + 1, move, sb);
		if(!move)
			break;
		move->options.start = bsdata<moveoptioni>::source.getcount();
		move->options.count = 0;
		while(*p && *p != '#') {
			p = read_options(p, sb);
			move->options.count++;
		}
	}
	close();
}

const moveoptioni* movei::findprompt(short unsigned v) const {
	if(v == 0xFFFF)
		return 0;
	for(auto& e : options) {
		if(e.index == v)
			return &e;
	}
	return 0;
}

const moveoptioni* movei::getanswer(const moveoptioni* pb) const {
	auto pe = options.end();
	auto index = pb->index;
	for(auto p = pb; p < pe; p++) {
		if(p->isanswer())
			return p;
	}
	return 0;
}

static const char* choose_header() {
	static char temp[128];
	if(!movei::choose_count)
		return 0;
	stringbuilder sb(temp);
	sb.add(getnm("ChooseOptions"), movei::choose_count);
	return temp;
}

static adat<void*, 32> choosed_answers;

const moveoptioni* movei::choose(const moveoptioni* p) const {
	answers an; auto pe = options.end();
	for(auto pa = getanswer(p); pa->isanswer() && pa < pe; pa++) {
		if(choosed_answers.is((void*)pa))
			continue;
		an.add(pa, pa->text);
	}
	p = (moveoptioni*)an.choose(choose_header(), 0, 1);
	choosed_answers.add((void*)p);
	if(p) {
		script_run(p->effect);
		p = findprompt(p->next);
	}
	return p;
}

void movei::run() const {
	if(!answers::console)
		return;
	auto p = findprompt(last_result);
	player->act(p->text);
	script_run(p->effect);
	if(choose_count>0) {
		choosed_answers.clear();
		while(choose_count > 0) {
			choose(p);
			choose_count--;
		}
	}
	draw::pause();
}