#include "code_package.h"
#include "code_rule.h"
#include "logparse.h"

using namespace code;

enum code_flag_s : unsigned char { Static, Public };

static pckh last_type, member_type;
static package* last_package;
static const char *file_source, *last_url;
static unsigned member_flags;
static adat<unsigned> locals;

static void clear_flags() {
	member_flags = 0;
}

static void add_static() {
	member_flags |= FG(Static);
}

static void add_public() {
	member_flags |= FG(Public);
}

static void push_locale() {
	locals.add(code::p - file_source);
}

static void pop_locale() {
	if(!locals.count)
		error("Closing scope bracing without opening one");
	else
		locals.count--;
}

static void set_url() {
	last_url = code::p;
}

static void set_type() {
	if(equal(last_identifier, "int") || equal(last_identifier, "bool"))
		last_type = i32;
	else if(equal(last_identifier, "char"))
		last_type = i8;
	else if(equal(last_identifier, "short"))
		last_type = i16;
	else if(equal(last_identifier, "unsigned"))
		last_type = u32;
	else if(equal(last_identifier, "void"))
		last_type = Void;
	else {
		last_type = last_package->findsym(last_identifier, Modules);
		if(!last_type) {
			error("Not found type `%1`", last_identifier);
			last_type = i32;
		}
	}
}

static void type_reference() {
	last_type = last_package->reference(last_type);
}

static void set_member_type() {
	member_type = last_type;
}

static void add_variable() {
	auto id = last_package->add(last_identifier);
	pckh result = None;
	for(int i = locals.count - 1; (result == None) && i >= 0; i--)
		result = last_package->findsymscope(id, This, locals.data[i]);
	if(result == None)
		result = last_package->findsymscope(id, This, 0);
	if(result == None)
		result = last_package->findsym(id, Modules);
	if(result == None)
		error("Undefined identifier `%1`", last_identifier);
}

static void add_member() {
	auto scope = 0;
	if(locals)
		scope = locals.data[locals.count - 1];
	auto id = last_package->add(last_identifier);
	auto symbol = last_package->findsymscope(id, This, scope);
	if(symbol != None) {
		error("Symbol `%1` already defined", last_identifier);
		return;
	}
	last_package->add(id, This, member_type, member_flags, code::p - file_source, scope);
}

static void add_type() {
	auto id_result = last_package->add(last_url);
	auto id = last_package->add(last_identifier);
	last_package->add(id, Modules, id_result, 0, code::p - file_source, 0);
}

static rule c2_grammar[] = {
	{"global", {"^%import", "%enum", "%declare_function", "%declare_variable"}},
	{"clear_flags", {}, clear_flags},
	{"add_member", {}, add_member},
	{"set_member_type", {}, set_member_type},
	{"set_type", {}, set_type},
	{"set_url", {}, set_url},
	{"push_locale", {}, push_locale},
	{"pop_locale", {}, pop_locale},

	{"identifier", {}, identifier},
	{"number", {}, number},
	{"string", {}, string},

	{"import", {"import", "@set_url", ". %identifier", "?%pseudoname", ";"}, add_type},
	{"pseudoname", {"as", "%identifier"}},

	{"enum", {"enum", "{", ", %enum_value", "}", ";"}},
	{"enum_value", {"%identifier", "?%enum_assign"}},
	{"enum_assign", {"=", "%constant"}},

	{"constant", {"%expression"}},

	{"type_reference", {"?.*"}, type_reference},
	{"type", {"%identifier", "@set_type", "%?type_reference"}},
	{"initialization", {"=", "%expression"}},
	{"static", {"static"}, add_static},
	{"public", {"public"}, add_public},
	{"parameter", {"%type", "%identifier"}},

	{"declare_function", {"@clear_flags", "?%static", "?%public", "%type", "@set_member_type", "%identifier", "(", "@add_member", ", ?%parameter", ")", "%block_statements"}},
	{"declare_variable_loop", {"%identifier", "@add_member", "?%array_scope", "?%initialization"}},
	{"declare_variable", {"@clear_flags", "?%static", "?%public", "%type", "@set_member_type", ", %declare_variable_loop", ";"}},
	{"declare_local", {"@clear_flags", "?%static", "%type", "@set_member_type", ", %declare_variable_loop", ";"}},

	{"funtion_call", {"(", "?, %expression", ")"}},
	{"variable", {"%identifier"}, add_variable},
	{"sizeof", {"sizeof", "(", "%expression", ")"}},
	{"array_scope", {"[", "%expression", "]"}},
	{"unary", {"^%number", "%string", "%sizeof", "%variable"}},
	{"prefix_op", {"^?\\++", "\\--", "\\&"}},
	{"prefix", {"?.%prefix_op", "%unary"}},
	{"indirection", {"\\.", "%identifier"}},
	{"postfix_op", {"^\\++", "\\--", "%indirection", "%funtion_call"}},
	{"postfix", {"%unary", "?.%postfix_op"}},
	{"multiplication_op", {"^\\/", "\\*"}},
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
	{"return", {"return", "?%expression", ";"}},

	{"statement", {"^%return", "%if", "%while", "%switch", "%declare_local"}},
	{"block_statements", {"{", "@push_locale", ".?%single_statement", "@pop_locale", "}"}},
	{"single_statement", {"^%block_statements", "%statement", ";"}},
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
	file_source = p;
	parse(p, 0, c2_grammar);
	log::close();
	last_package->write("code/test.c2b");
	return log::geterrors() == 0;
}