#include "condition.h"
#include "script.h"
#include "quest.h"

static int expessions[8];

static bool need_stop;

static int getvalue(int n) {
	if(n < sizeof(expessions) / sizeof(expessions[0]))
		return expessions[n];
	return 0;
}

static void pushstack(int v) {
	memmove(expessions + 1, expessions, sizeof(expessions) - sizeof(expessions[0]));
	expessions[0] = v;
}

static void popstack() {
	memmove(expessions, expessions + 1, sizeof(expessions) - sizeof(expessions[0]));
}

void quest::apply(const variants& tags) {
	auto push_stop = need_stop;
	need_stop = false;
	for(auto v : tags) {
		if(need_stop)
			break;
		auto pc = (conditioni*)v;
		if(pc) // All conditions simply skip
			continue;
		auto ps = (scripti*)v;
		if(ps) {
			ps->proc(v.counter, ps->param);
			continue;
		}
	}
	need_stop = push_stop;
}

static void add_value(int counter, int param) {
	pushstack(param + counter);
}

static void set_page(int counter, int param) {
}

static void if_entry(int counter, int param) {
	quest::run(param + counter);
	need_stop = true;
}

void initialize_quest_script() {
	scripti::add("Value000", add_value, 0);
	scripti::add("Value100", add_value, 100);
	scripti::add("Value200", add_value, 200);
	scripti::add("Value300", add_value, 300);
	scripti::add("Value400", add_value, 400);
	scripti::add("Value500", add_value, 500);
	scripti::add("Value600", add_value, 600);
	scripti::add("Value700", add_value, 700);
	scripti::add("Value800", add_value, 800);
	scripti::add("Value900", add_value, 900);
	scripti::add("IfEntryForward", if_entry);
}