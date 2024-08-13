#include "calculator.h"
#include "io_stream.h"
#include "stringbuilder.h"
#include "stringa.h"

BSDATAD(asti)
BSDATAD(definei)
BSDATAD(symboli)
BSDATAD(sectioni)

calculator_fnprint	calculator_error_proc;
static int			function_params;
static int			last_type;
static unsigned		last_flags;
static long			last_number;
static char			last_string[4096];
static const char*	p;
static const char*	p_start;
static const char*	p_url;
static stringa		strings;
static int			operations[128];
static int*			operation;
static int			errors_count;
static adat<int>	scopes;

static int			module_sid;
static const char*	project_url;
const char*			library_url;

static void parse_expression();
static void unary();
static int expression();

bool isterminal(operation_s v) {
	switch(v) {
	case Number: case Text: case Identifier: return true;
	case Continue: case Break: return true;
	default: return false;
	}
}

bool isbinary(operation_s op) {
	switch(op) {
	case Plus: case Minus: case Div: case Mul: case DivRest:
	case BinaryOr: case BinaryAnd: case BinaryXor:
	case ShiftLeft: case ShiftRight:
	case Less: case LessEqual: case Greater: case GreaterEqual: case Equal: case NotEqual:
	case Or: case And:
	case Scope: case Cast: case Point:
		return true;
	default: return false;
	}
}

bool isstrict(operation_s op) {
	switch(op) {
	case Plus: case Mul: case BinaryOr: case BinaryAnd: case BinaryXor: return false;
	default: return true;
	}
}

bool iserrors() {
	return errors_count > 0;
}

int predefined_symbol_size(int type) {
	switch(type) {
	case i8: case u8: return 1;
	case i16: case u16: return 2;
	case i32: case u32: return 4;
	default: return 0;
	}
}

static const char* create_example() {
	static char temp[260]; stringbuilder sb(temp); sb.clear();
	if(!p)
		return 0;
	auto p1 = p;
	// Find line begin
	while(p1 > p_start) {
		if(p1[-1] == 10 || p1[-1] == 13 || p1[-1]==9)
			break;
		p1--;
	}
	// Skip until line end
	while(*p1 && *p1 != 10 && *p1 != 13) {
		sb.add(*p1);
		p1++;
		if(sb.isempthy())
			break;
	}
	return temp;
}

static void error(const char* format, ...) {
	errors_count++;
	if(!calculator_error_proc)
		return;
	calculator_error_proc(p_url, format, xva_start(format), create_example());
}

int define_ast(int sid) {
	if(sid == -1)
		return -1;
	return bsdata<definei>::get(sid).ast;
}

bool symbol(int sid, symbol_flag_s v) {
	if(sid == -1)
		return false;
	return bsdata<symboli>::get(sid).is(v);
}

const char* symbol_name(int sid) {
	if(sid == -1)
		return "";
	auto& e = bsdata<symboli>::get(sid);
	return strings.get(e.ids);
}

void symbol_set(int sid, symbol_flag_s v) {
	if(sid == -1)
		return;
	bsdata<symboli>::get(sid).set(v);
}

int symbol_ast(int sid) {
	if(sid == -1)
		return -1;
	return bsdata<symboli>::get(sid).value;
}

void symbol_ast(int sid, int value) {
	if(sid == -1)
		return;
	bsdata<symboli>::get(sid).value = value;
}

int symbol_type(int sid) {
	if(sid == -1)
		return -1;
	auto n = bsdata<symboli>::get(sid).type;
	if(n == -1)
		return sid;
	return n;
}

int symbol_size(int sid) {
	if(sid == -1)
		return 0;
	auto type = symbol_type(sid);
	if(symbol_scope(type) == PointerScope)
		return 4;
	auto result = predefined_symbol_size(type);
	if(!result) {
		// Calculate each member size
		for(auto& e : bsdata<symboli>()) {
			if(e.parent == sid && e.scope == 0 && !e.is(Static))
				result += symbol_size(&e - bsdata<symboli>::begin());
		}
	}
	return result;
}

offseti symbol_section(int sid) {
	if(sid == -1)
		return {};
	return bsdata<symboli>::get(sid).instance;
}

void symbol_type(int sid, int value) {
	if(sid == -1)
		return;
	bsdata<symboli>::get(sid).type = value;
}

void symbol_count(int sid, int value) {
	if(sid == -1)
		return;
	auto& e = bsdata<symboli>::get(sid);
	e.count = value;
}

int symbol_scope(int sid) {
	if(sid == -1)
		return -1;
	return bsdata<symboli>::get(sid).scope;
}

void symbol_scope(int sid, int value) {
	if(sid == -1)
		return;
	auto& e = bsdata<symboli>::get(sid);
	e.scope = value;
}

static int getscope() {
	if(scopes)
		return scopes[scopes.count - 1];
	return 0;
}

static int local_size(int sid) {
	auto scope = getscope();
	auto scope_size = 0;
	for(auto& e : bsdata<symboli>()) {
		if(e.parent == module_sid && e.scope == scope) {
			auto esid = bsdata<symboli>::begin() - &e;
			if(esid == sid)
				break;
			scope_size += symbol_size(esid);
		}
	}
	return scope_size;
}

static void symbol_alloc(int sid, int data_sid, int data_size) {
	if(sid==-1 || data_sid == -1 || !data_size)
		return;
	auto& e = bsdata<symboli>::get(sid);
	if(e.instance.sid != -1)
		return;
	if(symbol_scope(sid) != PointerScope && !symbol(e.type, Predefined) && !symbol(e.type, Complete))
		error("Use incomplete type `%1`", symbol_name(e.type));
	e.instance.sid = data_sid;
	if(data_sid == LocalSection)
		e.instance.value = local_size(sid);
	else {
		auto& s = bsdata<sectioni>::get(data_sid);
		e.instance.value = s.size;
		s.size += data_size;
	}
}

const char* string_name(int sid) {
	if(sid == -1)
		return "";
	return strings.get(sid);
}

static void skipws() {
	while(true) {
		if(*p == ' ' || *p == 9 || *p == 10 || *p == 13) {
			p++;
			continue;
		}
		if(p[0] == '\\' && (p[1] == 10 || p[1] == 13)) { // Trail symbol
			p++;
			while(*p == 10 || *p == 13)
				p++;
			continue;
		}
		if(p[0] == '/' && p[1] == '/') { // End line comment
			p += 2;
			while(*p && *p != 10 && *p != 13)
				p++;
			continue;
		}
		if(p[0] == '/' && p[1] == '*') { // Complex comment
			p += 2;
			while(*p) {
				if(p[0] == '*' && p[1] == '/') {
					p += 2;
					break;
				}
				p++;
			}
			continue;
		}
		break;
	}
}

static void skipws(int count) {
	p += count;
	skipws();
}

static bool match(const char* symbol) {
	auto i = 0;
	while(symbol[i]) {
		if(p[i] != symbol[i])
			return false;
		i++;
	}
	p += i;
	skipws();
	return true;
}

static bool word(const char* symbol) {
	auto i = 0;
	while(symbol[i]) {
		if(p[i] != symbol[i])
			return false;
		i++;
	}
	auto c = p[i];
	if(ischa(c) || isnum(c))
		return false; // This is identifier, not word
	p += i;
	skipws();
	return true;
}

static void skip(const char* symbol) {
	if(!match(symbol))
		error("Expected token `%1`", symbol);
}

static bool isnumber() {
	return isnum(*p);
}

static bool isidentifier() {
	return ischa(*p);
}

int find_symbol(int ids, int scope, int parent) {
	for(auto& e : bsdata<symboli>()) {
		if(e.ids == ids && e.scope == scope && e.parent == parent)
			return &e - bsdata<symboli>::begin();
	}
	return -1;
}

static int find_define(int ids) {
	for(auto& e : bsdata<definei>()) {
		if(e.ids == ids)
			return &e - bsdata<definei>::begin();
	}
	return -1;
}

static int find_section(int ids) {
	for(auto& e : bsdata<sectioni>()) {
		if(e.ids == ids)
			return &e - bsdata<sectioni>::begin();
	}
	return -1;
}

static void find_type(int ast, int& result) {
	if(ast == -1 || result != -1)
		return;
	auto p = bsdata<asti>::begin() + ast;
	if(p->op == Identifier) {
		auto type = symbol_type(p->right);
		if(type != -1)
			result = type;
	} else if(isbinary(p->op)) {
		find_type(p->right, result);
		find_type(p->left, result);
	} else if(isterminal(p->op)) {
		// Nothing to do
	} else
		find_type(p->right, result);
}

static int findast(operation_s op, int left, int right) {
	for(auto& e : bsdata<asti>()) {
		if(e.op == op && e.left == left && e.right == right)
			return &e - bsdata<asti>::begin();
	}
	return -1;
}

int ast_add(operation_s op, int left, int right) {
	auto sid = findast(op, left, right);
	if(sid == -1 && !isstrict(op))
		sid = findast(op, right, left);
	if(sid == -1) {
		auto p = bsdata<asti>::add();
		p->op = op;
		p->left = left;
		p->right = right;
		sid = p - bsdata<asti>::begin();
	}
	return sid;
}

static int ast_add(operation_s op, int value) {
	return ast_add(op, -1, value);
}

static void add_op(int a) {
	operation[0] = a;
	operation++;
}

static int pop_op() {
	auto result = -1;
	if(operation > operations) {
		operation--;
		result = operation[0];
	}
	return result;
}

static void parse_number() {
	auto p1 = p;
	last_number = 0;
	p = stringbuilder::read(p, last_number);
	if(p1 != p)
		skipws();
}

static void literal() {
	stringbuilder sb(last_string); sb.clear();
	if(*p == '\"') {
		p = sb.psstr(p + 1, '\"');
		skipws();
	}
}

static void parse_identifier() {
	stringbuilder sb(last_string); sb.clear();
	if(!isidentifier())
		error("Expected identifier");
	while(*p && (ischa(*p) || isnum(*p) || *p == '_'))
		sb.add(*p++);
	skipws();
}

static void parse_flags() {
	last_flags = 0;
	while(*p) {
		if(word("static"))
			last_flags |= FG(Static);
		else if(word("public"))
			last_flags |= FG(Public);
		else
			break;
	}
}

static int create_section(int ids) {
	auto p = bsdata<sectioni>::add();
	p->ids = ids;
	p->data = 0;
	p->size = 0;
	return p - bsdata<sectioni>::begin();
}

static int create_define(int ids, int ast) {
	auto p = bsdata<definei>::add();
	p->ids = ids;
	p->ast = ast;
	return p - bsdata<definei>::begin();
}

static int create_define(int ids, int ast, const char* error_format) {
	auto sid = find_define(ids);
	if(sid != -1) {
		if(error_format)
			error(error_format, string_name(ids));
	} else
		sid = create_define(ids, ast);
	return sid;
}

static int create_symbol(int id, int type, unsigned flags, int scope, int parent) {
	auto p = bsdata<symboli>::add();
	p->ids = id;
	if(scope == -1)
		scope = getscope();
	p->scope = scope;
	p->parent = parent;
	p->type = type;
	p->flags = flags;
	p->value = -1;
	p->instance.sid = -1;
	p->instance.value = 0;
	return p - bsdata<symboli>::begin();
}

static int create_symbol(int ids, int type, unsigned flags, int scope, int parent, const char* error_format) {
	if(scope == -1)
		scope = getscope();
	auto sid = find_symbol(ids, scope, parent);
	if(sid != -1) {
		if(error_format)
			error(error_format, string_name(ids));
	} else
		sid = create_symbol(ids, type, flags, scope, parent);
	return sid;
}

static const char* find_module_url(const char* folder, const char* id, const char* ext) {
	static char temp[260]; stringbuilder sb(temp); sb.clear();
	if(!folder || !id || !ext)
		return 0;
	sb.add(folder);
	sb.add("/");
	auto p = sb.get();
	sb.add(id);
	sb.change('.', '/');
	sb.add(".");
	sb.add(ext);
	if(!io::file::exist(temp))
		return 0;
	return temp;
}

static const char* get_type_url(const char* id) {
	auto p = find_module_url(project_url, id, "c2");
	if(!p)
		p = find_module_url(library_url, id, "c2");
	return p;
}

static void instance_symbol(int sid) {
	auto size = symbol_size(sid);
	auto section = ModuleSection;
	if(symbol(sid, Static)) {
		if(symbol_ast(sid)==-1)
			section = UDataSection;
		else
			section = DataSection;
	} else if(symbol_scope(sid) != 0)
		section = LocalSection;
	symbol_alloc(sid, section, size);
}

static int create_module(int ids) {
	auto sid = find_symbol(ids, TypeScope, 0);
	if(sid == -1) {
		auto url = get_type_url(string_name(ids));
		if(!url) {
			error("Can't find module `%1`", string_name(ids));
			return -1;
		}
		auto push_module = module_sid;
		module_sid = create_symbol(ids, -1, 0, TypeScope, 0);
		symbol_type(module_sid, module_sid);
		calculator_file_parse(url);
		symbol(module_sid, Complete);
		sid = module_sid;
		module_sid = push_module;
	}
	return sid;
}

static int find_variable(int ids) {
	auto sid = -1;
	for(auto i = (int)scopes.count - 1; sid == -1 && i >= 0; i--)
		sid = find_symbol(ids, scopes.data[i], module_sid);
	if(sid == -1 && function_params != -1)
		sid = find_symbol(ids, 0, function_params);
	if(sid == -1)
		sid = find_symbol(ids, 0, module_sid);
	return sid;
}

int dereference(int type) {
	if(type == -1)
		return -1;
	auto p = bsdata<symboli>::begin() + type;
	return p->type;
}

int reference(int type) {
	if(type == -1)
		return -1;
	auto sid = find_symbol(type, PointerScope, 0);
	if(sid != -1)
		return sid;
	return create_symbol(type, type, 0, PointerScope, 0);
}

static void parse_type() {
	last_type = -1;
	last_flags = 0;
	if(!isidentifier())
		return;
	auto p_push = p;
	parse_flags();
	parse_identifier();
	auto ids = strings.add(last_string);
	last_type = find_symbol(ids, TypeScope, 0);
	if(last_type == -1) {
		auto sid = find_define(ids);
		if(sid != -1)
			find_type(define_ast(sid), last_type);
	}
	if(last_type == -1) {
		p = p_push;
		return;
	}
	last_type = symbol_type(last_type);
	while(match("*"))
		last_type = reference(last_type);
}

static bool parse_member_declaration() {
	auto push_p = p;
	parse_type();
	if(last_type == -1) {
		p = push_p;
		return false;
	}
	if(!isidentifier()) {
		p = push_p;
		return false;
	}
	return true;
}

static void parse_url_identifier() {
	stringbuilder sb(last_string); sb.clear();
	if(!isidentifier())
		error("Expected identifier");
	while(*p && (ischa(*p) || isnum(*p) || *p == '_' || *p == '.'))
		sb.add(*p++);
	skipws();
}

static void unary_operation(operation_s v) {
	if(operation <= operations) {
		error("Unary operations stack corupt");
		return;
	}
	operation[-1] = ast_add(v, operation[-1]);
}

static void binary_operation(operation_s v) {
	if(operation <= operations + 1) {
		error("Binary operations stack corupt");
		return;
	}
	operation[-2] = ast_add(v, operation[-2], operation[-1]);
	operation--;
}

static void add_list(int& result, int value) {
	if(value == -1)
		return;
	if(result == -1)
		result = value;
	else
		result = ast_add(List, result, value);
}

static int parameter_list() {
	auto result = -1;
	while(true) {
		add_list(result, expression());
		if(match(","))
			continue;
		break;
	}
	return result;
}

static void postfix() {
	while(*p) {
		if(match("(")) {
			auto params = -1;
			if(*p != ')')
				params = parameter_list();
			skip(")");
		} else if(match("[")) {
			expression();
			skip("]");
		} else if(match("++")) {

		} else if(match("--")) {

		} else if(match(".")) {
			parse_identifier();
			auto ids = strings.add(last_string);
			auto sid = find_variable(ids);
			if(sid == -1)
				sid = create_symbol(ids, i32, 0, -1, module_sid);
			add_op(ast_add(Identifier, sid));
			binary_operation(Point);
		} else
			break;
	}
}

static void unary() {
	switch(p[0]) {
	case '-':
		if(p[1] == '-') {
			skipws(2);
			unary();
		} else {
			skipws(1);
			unary();
			unary_operation(Neg);
		}
		break;
	case '+':
		if(p[1] == '+') {
			skipws(2);
			unary();
		} else {
			skipws(1);
			unary();
		}
		break;
	case '!':
		skipws(1);
		unary();
		unary_operation(Not);
		break;
	case '*':
		skipws(1);
		unary();
		unary_operation(Dereference);
		break;
	case '&':
		skipws(1);
		unary();
		unary_operation(AdressOf);
		break;
	case '(':
		skipws(1);
		parse_type();
		if(last_type != -1 && *p == ')') {
			auto type = last_type;
			skipws(1);
			unary();
			add_op(ast_add(Cast, type));
		} else {
			parse_expression();
			skip(")");
		}
		break;
	case '\"':
		literal();
		add_op(ast_add(Text, strings.add(last_string)));
		break;
	case '0':case '1':case '2':case '3':case '4':
	case '5':case '6':case '7':case '8':case '9':
		parse_number();
		add_op(ast_add(Number, last_number));
		break;
	default:
		if(isidentifier()) {
			parse_identifier();
			auto ids = strings.add(last_string);
			auto idf = find_define(ids);
			if(idf != -1) {
				add_op(define_ast(idf));
				break;
			}
			auto sid = find_variable(ids);
			if(sid == -1) {
				error("Symbol `%1` not exist", string_name(ids));
				sid = create_symbol(ids, i32, 0, -1, module_sid);
			}
			add_op(ast_add(Identifier, sid));
		}
		break;
	}
	postfix();
}

static void multiplication() {
	unary();
	while((p[0] == '*' || p[0] == '/' || p[0] == '%') && p[1] != '=') {
		char s = p[0]; skipws(1);
		operation_s op;
		switch(s) {
		case '/': op = Div; break;
		case '%': op = DivRest; break;
		default: op = Mul; break;
		}
		unary();
		binary_operation(op);
	}
}

static void addiction() {
	multiplication();
	while((p[0] == '+' || p[0] == '-') && p[1] != '=') {
		char s = p[0]; skipws(1);
		operation_s op;
		switch(s) {
		case '+': op = Plus; break;
		case '-': op = Minus; break;
		default: op = Mul; break;
		}
		multiplication();
		binary_operation(op);
	}
}

static void binary_cond() {
	addiction();
	while((p[0] == '>' && p[1] != '>')
		|| (p[0] == '<' && p[1] != '<')
		|| (p[0] == '=' && p[1] == '=')
		|| (p[0] == '!' && p[1] == '=')) {
		char t1 = *p++;
		char t2 = 0;
		if(p[0] == '=')
			t2 = *p++;
		skipws();
		operation_s op;
		switch(t1) {
		case '>':
			op = Greater;
			if(t2 == '=')
				op = GreaterEqual;
			break;
		case '<':
			op = Less;
			if(t2 == '=')
				op = LessEqual;
			break;
		case '!': op = NotEqual; break;
		default: op = Equal; break;
		}
		addiction();
		binary_operation(op);
	}
}

static void binary_and() {
	binary_cond();
	while(p[0] == '&' && p[1] != '&') {
		skipws(1);
		binary_cond();
		binary_operation(BinaryAnd);
	}
}

static void binary_xor() {
	binary_and();
	while(p[0] == '^') {
		skipws(1);
		binary_and();
		binary_operation(BinaryXor);
	}
}

static void binary_or() {
	binary_xor();
	while(p[0] == '|' && p[1] != '|') {
		skipws(1);
		binary_xor();
		binary_operation(BinaryOr);
	}
}

static void binary_shift() {
	binary_or();
	while((p[0] == '>' && p[1] == '>') || (p[0] == '<' && p[1] == '<')) {
		operation_s op;
		switch(p[0]) {
		case '>': op = ShiftRight; break;
		default: op = ShiftLeft; break;
		}
		skipws(2);
		binary_or();
		binary_operation(op);
	}
}

static void logical_and() {
	binary_shift();
	while(p[0] == '&' && p[1] == '&') {
		skipws(2);
		binary_shift();
		binary_operation(And);
	}
}

static void logical_or() {
	logical_and();
	while(p[0] == '|' && p[1] == '|') {
		skipws(2);
		logical_and();
		binary_operation(Or);
	}
}

static void parse_expression() {
	logical_or();
	while(*p == '?') {
		skipws(1);
		parse_expression();
		skip(":");
		parse_expression();
	}
}

static int expression() {
	parse_expression();
	return pop_op();
}

static void parse_initialization_list() {
	if(match("{")) {
		auto result = -1;
		do {
			auto p1 = p;
			parse_initialization_list();
			if(p1 == p)
				break;
			add_list(result, pop_op());
		} while(match(","));
		add_op(ast_add(Initialization, result));
		skip("}");
	} else
		parse_expression();
}

static int initialization_list() {
	parse_initialization_list();
	return pop_op();
}

static int getmoduleposition() {
	return p - p_start;
}

static void parse_assigment() {
	unary();
	while(p[0] == '=') {
		skipws(1);
		parse_expression();
		binary_operation(Assign);
	}
}

static void push_visibility_scope() {
	scopes.add(getmoduleposition());
}

static void pop_visibility_scope() {
	scopes.count--;
}

static void parse_array_declaration(int sid) {
	if(match("[")) {
		auto index = const_number(expression());
		symbol_count(sid, index);
		skip("]");
	}
}

static void parse_local_declaration() {
	if(parse_member_declaration()) {
		auto type = last_type;
		auto flags = last_flags;
		parse_identifier();
		auto ids = strings.add(last_string);
		auto sid = create_symbol(ids, type, flags, -1, module_sid);
		parse_array_declaration(sid);
		instance_symbol(sid);
		if(match("="))
			symbol_ast(sid, initialization_list());
		skip(";");
	} else {
		parse_assigment();
		skip(";");
	}
}

static void parse_statement() {
	if(match("{")) {
		push_visibility_scope();
		auto push_p = p;
		auto previous = -1;
		while(*p && *p != '}') {
			parse_statement();
			add_list(previous, pop_op());
			if(p == push_p)
				break;
		}
		pop_visibility_scope();
		skip("}");
		add_op(previous);
	} else if(match(";")) {
		// Empthy statement
	} else if(match("if")) {
		skip("(");
		auto e = expression();
		skip(")");
		parse_statement();
		auto s = pop_op();
		add_op(ast_add(If, s, e));
	} else if(match("swith")) {
		skip("(");
		auto e = expression();
		skip(")");
		parse_statement();
		auto s = pop_op();
		add_op(ast_add(Switch, s, e));
	} else if(match("while")) {
		skip("(");
		auto e = expression();
		skip(")");
		parse_statement();
		auto s = pop_op();
		add_op(ast_add(While, s, e));
	} else if(match("for")) {
		skip("(");
		parse_local_declaration();
		auto bs = pop_op();
		auto e = expression();
		skip(";");
		auto es = expression();
		skip(")");
		parse_statement();
		auto s = pop_op();
		add_list(s, es);
		add_list(bs, ast_add(While, e, s));
		add_op(bs);
	} else if(match("return")) {
		add_op(ast_add(Return, expression()));
		skip(";");
	} else if(match("case")) {
		add_op(ast_add(Case, expression()));
		skip(":");
	} else if(match("break")) {
		add_op(ast_add(Break, -1));
		skip(";");
	} else if(match("continue")) {
		add_op(ast_add(Continue, -1));
		skip(";");
	} else
		parse_local_declaration();
}

static void parse_enum() {
	if(!word("enum"))
		return;
	auto index = 0;
	skip("{");
	while(*p) {
		parse_identifier();
		if(match("="))
			index = const_number(expression());
		auto ids = strings.add(last_string);
		auto sid = create_define(ids, ast_add(Number, index), "Identifier `%1` is already defined");
		if(!match(","))
			break;
	}
	skip("}");
	skip(";");
}

static void parse_parameters() {
	auto scope = getscope();
	while(*p && *p != ')') {
		auto ast = -1;
		auto p1 = p;
		parse_type();
		if(p1 == p)
			break;
		auto type = last_type;
		auto flags = last_flags;
		parse_identifier();
		if(match("="))
			ast = expression();
		auto ids = strings.add(last_string);
		auto sid = create_symbol(ids, type, flags, scope, function_params, "Parameter `%1` is already defined");
		symbol_ast(sid, ast);
		if(match(","))
			continue;
	}
	skip(")");
}

static void parse_declaration() {
	if(!parse_member_declaration())
		return;
	auto type = last_type;
	auto flags = last_flags;
	while(isidentifier()) {
		parse_identifier();
		auto ids = strings.add(last_string);
		auto sid = create_symbol(ids, type, flags, -1, module_sid, "Module member `%1` is already defined");
		if(match("(")) {
			auto push_params = function_params;
			function_params = sid;
			parse_parameters();
			if(!match(";")) {
				parse_statement();
				symbol_ast(pop_op());
			}
			function_params = push_params;
			break;
		} else {
			parse_array_declaration(sid);
			if(!symbol(sid, Static))
				instance_symbol(sid);
			if(match("="))
				symbol_ast(sid, initialization_list());
			if(match(","))
				continue;
			skip(";");
			break;
		}
	}
}

static void parse_import() {
	if(!word("import"))
		return;
	parse_url_identifier();
	auto ids = strings.add(last_string);
	auto sid = create_module(ids);
	if(match("as")) {
		parse_identifier();
		auto als = strings.add(last_string);
		create_define(als, ast_add(Identifier, sid));
	}
}

static void parse_define() {
	if(!word("define"))
		return;
	parse_identifier();
	skip("=");
	auto ids = strings.add(last_string);
	create_define(ids, expression());
}

static void parse_module() {
	while(*p) {
		auto push_p = p;
		parse_import();
		if(p != push_p)
			continue;
		parse_define();
		if(p != push_p)
			continue;
		parse_enum();
		if(p != push_p)
			continue;
		parse_declaration();
		if(p != push_p)
			continue;
		if(!errors_count)
			error("Expected declaration");
		break;
	}
}

void calculator_parse(const char* code) {
	auto push_p = p;
	auto push_p_start = p_start;
	p = code;
	p_start = code;
	parse_expression();
	p_start = push_p_start;
	p = push_p;
}

static void add_symbol(symbol_s v, const char* id) {
	auto ids = strings.add(id);
	if(find_symbol(ids, TypeScope, 0) != -1)
		return;
	create_symbol(ids, -1, FG(Predefined), TypeScope, 0);
}

static void add_symbol(section_s v, const char* id) {
	auto ids = strings.add(id);
	if(find_section(ids) != -1)
		return;
	create_section(ids);
}

static void symbol_initialize() {
	add_symbol(Void, "void");
	add_symbol(i8, "char");
	add_symbol(u8, "uchar");
	add_symbol(i16, "short");
	add_symbol(u16, "ushort");
	add_symbol(i32, "int");
	add_symbol(u32, "unsigned");
	add_symbol(i64, "long");
	add_symbol(u64, "ulong");
	add_symbol(ModuleSection, ".module");
	add_symbol(LocalSection, ".local");
	add_symbol(DataSection, ".data");
	add_symbol(UDataSection, ".bss");
}

static void calculator_initialize() {
	symbol_initialize();
	operation = operations;
	function_params = -1;
}

void calculator_file_parse(const char* url) {
	auto p1 = loadt(url);
	if(!p1) {
		error("Can't find file `%1`", url);
		return;
	}
	calculator_initialize();
	auto push_p = p;
	auto push_p_start = p_start;
	auto push_p_url = p_url;
	p = p1; p_start = p1; p_url = url;
	skipws();
	parse_module();
	p_url = push_p_url;
	p_start = push_p_start;
	p = push_p;
	delete p1;
}

void project_compile(const char* url) {
	auto push_project = project_url;
	errors_count = 0;
	project_url = url;
	calculator_initialize();
	create_module(strings.add("main"));
	project_url = push_project;
}