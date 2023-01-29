#include "code_package.h"
#include "code_rule.h"
#include "logparse.h"

using namespace code;

enum code_flag_s : unsigned char { Static, Public};

static pckh last_type, last_member_type;

static package* last_package;
static const char* last_member_identifier;
static const char* last_statement;
static const char* last_scope;

static unsigned member_flags;

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

static void add_variable() {
	auto result = last_package->findsym(last_identifier, This);
	if(result == None)
		error("Undefined identifier `%1`", last_identifier);
}

static void clear_flags() {
	member_flags = 0;
}

static void add_static() {
	member_flags |= FG(Static);
}

static void add_public() {
	member_flags |= FG(Public);
}

static void declare_function() {
	auto id = last_package->add(last_member_identifier);
	last_package->add(id, This, last_member_type, 0, 0);
}

static void declare_member() {
	auto id = last_package->add(last_member_identifier);
	last_package->add(id, This, last_member_type, 0, 0);
}

static rule c2_grammar[] = {
	{"global", {"^%import", "%enum", "%member_function", "%member_variable"}},
	{"clear_flags", {}, clear_flags},

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
	{"static", {"static"}, add_static},
	{"public", {"public"}, add_public},
	{"member", {"%type", "%identifier"}, add_member},
	{"parameter", {"%type", "%identifier"}},
	
	{"declare_function", {"@clear_flags", "?%static", "?%public", "%member", "(", ", ?%parameter", ")"}, declare_function},
	{"member_function", {"%declare_function", "%block_statements"}},
	
	{"declare_variable", {"@clear_flags", "?%static", "?%public", "%member", "?%array_scope"}, declare_member},
	{"member_variable", {"%declare_variable", "?%initialization", ";"}},
	{"local_variable", {"@clear_flags", "?%static", "%member", "?%array_scope", "?%initialization"}},

	{"variable", {"%identifier"}, add_variable},
	{"sizeof", {"sizeof", "(", "%expression", ")"}},
	{"array_scope", {"[", "%expression", "]"}},
	{"unary", {"^%number", "%string", "%sizeof", "%variable"}},
	{"prefix_op", {"^?\\++", "\\--", "\\&"}},
	{"prefix", {"?.%prefix_op", "%unary"}},
	{"indirection", {"\\.", "%identifier"}},
	{"postfix_op", {"^\\++", "\\--", "%indirection"}},
	{"postfix", {"%unary", "?.%postfix_op"}},
	{"multiplication_op", {"^/", "*"}},
	{"multiplication_op_state", {"%multiplication_op", "%postfix"}},
	{"multiplication", {"%postfix", "?.%multiplication_op_state"}},
	{"addiction_op", {"^\\+", "\\-"}},
	{"addiction_op_state", {"%addiction_op", "%multiplication"}},
	{"addiction", {"%multiplication", "?.%addiction_op_state"}},
	{"binary_op", {"^\\|", "\\&", "\\^"}},
	{"binary_op_state", {"%binary_op", "%addiction"}},
	{"binary", {"%addiction", "?.%binary_op_state"}},
	{"conditional_op", {"^\\>", "\\<", "\\<=", "\\>=", "\\==", "\\!="}},
	{"conditional_op_state", {"%conditional_op", "%binary"}},
	{"conditional", {"%binary", "?.%conditional_op_state"}},
	{"logical_op", {"^\\||", "\\&&"}},
	{"logical_op_state", {"%logical_op", "%conditional"}},
	{"logical", {"%conditional", "?.%logical_op_state"}},
	{"expression", {"%logical"}},

	{"while", {"while", "(", "%expression", ")", "%single_statement"}},
	{"if", {"if", "(", "%expression", ")", "%single_statement"}},
	{"switch", {"switch", "(", "%expression", ")", "{", "}"}},
	{"return", {"return", "?%expression"}},

	{"statement", {"^%return", "%local_variable"}},
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
	last_scope = 0;
	parse(p, 0, c2_grammar);
	log::close();
	last_package->write("code/test.c2b");
	return log::geterrors() == 0;
}