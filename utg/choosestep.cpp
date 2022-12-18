#include "choosestep.h"
#include "pushvalue.h"
#include "script.h"

bool choosestep::human;
bool choosestep::applied;
bool choosestep::stop;
int choosestep::options;
void* choosestep::result;
const choosestep* laststep;
fnevent choosestep::pstd_apply;
choosestep::fnanswer choosestep::pstd_answer;

static void add_choose_options(stringbuilder& sb) {
	if(choosestep::options > 0)
		sb.adds(getnm("ChooseOptions"), choosestep::options);
}

void choosestep::run() const {
	draw::pause();
	auto push_last = laststep;
	laststep = this;
	if(pbefore)
		pbefore();
	char temp[260]; stringbuilder sb(temp);
	answers an;
	auto push_stop = stop;
	while(!stop) {
		sb.clear(); an.clear();
		sb.add(getnm(id));
		add_choose_options(sb);
		panswer(sb, an);
		if(pstd_answer)
			pstd_answer(sb, an);
		const char* cancel_text = 0;
		if(cancel)
			cancel_text = getnm(cancel);
		if(human)
			result = an.choose(temp, cancel_text);
		else if(paichoose) {
			paichoose(an);
			break; // All choose make in one case
		} else
			result = an.random();
		if(!result)
			break;
		applied = false;
		if(pstd_apply)
			pstd_apply();
		if(!applied) {
			if(papply)
				papply();
		}
	}
	stop = push_stop;
	if(pafter)
		pafter();
	draw::pause();
	laststep = push_last;
}

void choosestep::run(const char* id) {
	auto p = bsdata<choosestep>::find(id);
	if(p)
		p->run();
}

void choosestep::addscript(answers& an, const char* id) {
	auto p = bsdata<script>::find(id);
	if(p) {
		an.add(p, getnm(p->id));
		return;
	}
	auto ps = bsdata<choosestep>::find(id);
	if(ps) {
		an.add(ps, getnm(ps->id));
		return;
	}
}

template<> void fnscript<choosestep>(int index, int bonus) {
	pushvalue push_options(choosestep::options, bonus);
	bsdata<choosestep>::elements[index].run();
}