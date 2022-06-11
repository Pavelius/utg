#include "choosestep.h"

bool choosestep::stop;
bool choosestep::human;
bool choosestep::applied;
int choosestep::options;
void* choosestep::result;
const choosestep* choosestep::last;
fnevent choosestep::pstd_apply;
choosestep::fnanswer choosestep::pstd_answer;

void choosestep::run() const {
	draw::pause();
	auto push_stop = stop;
	auto push_last = last;
	last = this;
	if(pbefore)
		pbefore();
	char temp[260]; stringbuilder sb(temp);
	answers an;
	while(!stop) {
		sb.clear(); an.clear();
		sb.add(getnm(id));
		if(options > 0)
			sb.adds(getnm("ChooseOptions"), options);
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
			break;
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
	last = push_last;
}

void choosestep::run(const char* id) {
	auto p = bsdata<choosestep>::find(id);
	if(p)
		p->run();
}