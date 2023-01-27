#include "code_package.h"
#include "code_rule.h"
#include "logparse.h"

using namespace code;

static package* last_package;
static pckh last_type, last_member_type;
static const char* last_member_identifier;

static void type_identifier() {
	if(equal(last_identifier, "int"))
		last_type = i32;
	else if(equal(last_identifier, "char"))
		last_type = i8;
	else if(equal(last_identifier, "short"))
		last_type = i16;
	else if(equal(last_identifier, "unsigned"))
		last_type = u32;
	else {
		last_type = last_package->findsym(last_identifier, Modules);
		if(!last_type)
			last_type = i32;
	}
}

static void type_reference() {
	last_type = last_package->reference(last_type);
}

static void add_member() {
	last_member_type = last_type;
	last_member_identifier = last_identifier;
}

static void declare_function() {
	auto id = last_package->add(last_member_identifier);
	last_package->add(id, This, last_member_type, 0, 0);
}

static void declare_variable() {
	auto id = last_package->add(last_member_identifier);
	last_package->add(id, This, last_member_type, 0, 0);
}

static rule c2_grammar[] = {
	{"global", {"^%import", "%enum", "%member_function", "%member_variable"}},

	{"identifier", {}, identifier},
	{"number", {}, number},
	{"string", {}, string},
		
	{"import", {"import", "%url", "?%pseudoname", ";"}},
	{"pseudoname", {"as", "%identifier"}},
	{"url", {". %identifier"}},

	{"enum", {"enum", "{", ", %enum_value", "}", ";"}},
	{"enum_value", {"%identifier", "?%enum_assign"}},
	{"enum_assign", {"=", "%constant"}},

	{"constant", {"%expression"}},

	{"type_idenfifier", {"%identifier"}, type_identifier},
	{"type_reference", {"?.*"}, type_reference},
	{"type", {"%type_idenfifier", "%?type_reference"}},
	{"initialization", {"=", "%expression"}},
	{"static", {"static"}},
	{"public", {"public"}},
	{"member", {"%type", "%identifier"}, add_member},
	{"parameter", {"%type", "%identifier"}},
	
	{"declare_function", {"?%static", "?%public", "%member", "(", ", ?%parameter", ")"}, declare_function},
	{"member_function", {"%declare_function", "%block_statements"}},
	
	{"declare_variable", {"?%static", "?%public", "%member", "?%array_scope"}, declare_variable},
	{"member_variable", {"%declare_variable", "?%initialization", ";"}},
	{"local_variable", {"?%static", "%member", "?%array_scope", "?%initialization"}},

	{"sizeof", {"sizeof", "(", "%expression", ")"}},
	{"array_scope", {"[", "%expression", "]"}},
	{"unary", {"^%number", "%string", "%sizeof"}},
	{"prefix_op", {"^?\\++", "\\--", "\\&"}},
	{"prefix", {"?.%prefix_op", "%unary"}},
	{"indirection", {"\\.", "%identifier"}},
	{"postfix_op", {"^\\++", "\\--", "%indirection"}},
	{"postfix", {"%unary", "?.%postfix_op"}},
	{"multiplication_op", {"^/", "*"}},
	{"multiplication_op_state", {"%multiplication_op", "%postfix"}},
	{"multiplication", {"%postfix", "?%multiplication_op_state"}},
	{"addiction_op", {"^\\+", "\\-"}},
	{"addiction_op_state", {"%addiction_op", "%multiplication"}},
	{"addiction", {"%multiplication", "?%addiction_op_state"}},
	{"binary_op", {"^\\|", "\\&", "\\^"}},
	{"binary_op_state", {"%binary_op", "%addiction"}},
	{"binary", {"%addiction", "?%binary_op_state"}},
	{"conditional_op", {"^\\>", "\\<", "\\<=", "\\>=", "\\==", "\\!="}},
	{"conditional_op_state", {"%conditional_op", "%binary"}},
	{"conditional", {"%binary", "?%conditional_op_state"}},
	{"logical_op", {"^\\||", "\\&&"}},
	{"logical_op_state", {"%logical_op", "%conditional"}},
	{"logical", {"%conditional", "?%logical_op_state"}},
	{"expression", {"%logical"}},

	{"while", {"while", "(", "%expression", ")", "%single_statement"}},
	{"if", {"if", "(", "%expression", ")", "%single_statement"}},
	{"switch", {"switch", "(", "%expression", ")", "{", "}"}},

	{"statement", {"^%local_variable"}},
	{"block_statements", {"{", ".?%single_statement", "}"}},
	{"single_statement", {"?%statement", ";"}},
};

static void code_error(const char* position, const char* format, const char* format_param) {
	char temp[2048]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	log::errorv(position, temp);
}

void initialize_code() {
	perror = code_error;
	setrules(c2_grammar);
}

bool test_code() {
	auto p = log::read("code/test.c2");
	if(!p)
		return false;
	last_package = bsdata<package>::add();
	last_package->create("test");
	parse(p, 0, c2_grammar);
	log::close();
	return log::geterrors() == 0;
}