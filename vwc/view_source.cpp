#include "collection.h"
#include "code_package.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"
#include "view_source.h"

using namespace code;
using namespace draw;

symbol* code::active_symbol;

color colors::source::keyword = {76, 156, 214};
color colors::source::number = {181, 206, 168};
color colors::source::text = {200, 200, 174};

static void paint(const char* value, color value_color) {
	auto push = fore;
	fore = value_color;
	textmc(value);
	fore = push;
}

static void paint(int value, color value_color) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	paint(temp, value_color);
}

static const char* get_operation_name(operation type) {
	switch(type) {
	case operation::Plus: return "+";
	case operation::Minus: return "-";
	case operation::Less: return "<";
	case operation::LessEqual: return "<=";
	case operation::Greater: return ">";
	case operation::GreaterEqual: return ">=";
	default: return bsdata<operationi>::elements[(int)type].id;
	}
}

static void paint(code::operation type) {
	paint(get_operation_name(type), colors::source::keyword);
}

static void add_space() {
	caret.x += textw(' ');
}

static void paint(const ast* p) {
	static int tab = textw(' ') * 2;
	if(!p)
		return;
	if(p->isnumber())
		paint(p->left, colors::source::number);
	else if(p->isliteral())
		paint(last_package->getstr(p->left), colors::source::text);
	else if(p->issymbol()) {
		auto symbol = last_package->getsym(p->left);
		if(symbol)
		paint(last_package->getstr(symbol->id), colors::source::text);
	} else {
		auto push = caret;
		switch(p->type) {
		case operation::Statement:
			paint(last_package->getast(p->left));
			caret.x = push.x;
			caret.y += texth();
			paint(last_package->getast(p->right));
			break;
		case operation::Switch:
			paint(p->type);
			add_space();
			paint(last_package->getast(p->left));
			caret.x = push.x + tab;
			caret.y += texth();
			paint(last_package->getast(p->right));
			caret.x = push.x;
			break;
		case operation::Default:
			caret.x -= tab;
			paint(p->type);
			caret.x = push.x;
			break;
		case operation::Break:
			paint(p->type);
			break;
		case operation::Case:
			caret.x -= tab;
			paint(p->type);
			add_space();
			paint(last_package->getast(p->left));
			caret.x = push.x;
			break;
		case operation::Return:
			paint(p->type);
			add_space();
			paint(last_package->getast(p->left));
			break;
		default:
			paint(last_package->getast(p->left));
			add_space();
			paint(p->type);
			add_space();
			paint(last_package->getast(p->right));
			break;
		}
	}
}

void code::view_code_source() {
	if(!active_symbol)
		return;
	paint(last_package->getast(active_symbol->ast));
}