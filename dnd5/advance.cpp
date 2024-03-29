#include "advance.h"
#include "answers.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"

static void add_elements(answers& an, const variants& elements) {
	for(auto v : elements) {
		if(v.iskind<listi>())
			add_elements(an, bsdata<listi>::elements[v.value].elements);
		else if(v.iskind<modifieri>())
			fnscript<modifieri>(v.value, v.counter);
		else if(script_allow(v))
			an.add(v.getpointer(), v.getname());
	}
}

static variant choose_advance(const char* id, const variants& elements, int count, int count_original) {
	char temp[260]; stringbuilder sb(temp);
	answers an; add_elements(an, elements);
	if(szstart(id, "Choose"))
		sb.add(getnm(id));
	else {
		sb.add("%Choose [%1]", getnm(id));
		if(count_original > 1 && count > 0)
			sb.adds("(%-Left %1i)", count);
	}
	return an.choose(temp);
}

void advancei::apply() const {
	pushvalue push_modifier(modifier, NoModifier);
	pushvalue push_permanent(permanent_modifier, true);
	if(choose) {
		pushvalue push_columns(answers::column_count);
		if(columns)
			answers::column_count = columns;
		auto count = choose;
		while(count-- > 0) {
			modifier = NoModifier;
			permanent_modifier = true;
			auto v = choose_advance(id, elements, count + 1, choose);
			if(object.iskind<script>()) {
				pushvalue push(last_result, v);
				bsdata<script>::elements[object.value].proc(0);
			} else
				script_run(v);
		}
	} else
		script_run(elements);
}