#include "calculator.h"
#include "stringbuilder.h"
#include "stringa.h"

BSDATAD(asti)
BSDATAD(symboli)

calculator_fnprint	calculator_error_proc;
static int			last_type;
static unsigned		last_flags;
static long			last_number;
static char			last_string[4096];
static const char*	p;
static const char*	p_start;
static stringa		strings;
static int			operations[128];
static int*			operation;
static adat<int>	scopes;

static void parse_expression();
static int expression();

static void error(const char* format, ...) {
	if(calculator_error_proc)
		calculator_error_proc(format, xva_start(format));
}

int symbol_ast(int sid) {
	if(sid == -1)
		return -1;
	return bsdata<symboli>::get(sid).value;
}

void symbol_ast(int sid, int value) {
	if(sid == -1)
		return;
	auto& e = bsdata<symboli>::get(sid);
	e.value = value;
}

void symbol_count(int sid, int value) {
	if(sid == -1)
		return;
	auto& e = bsdata<symboli>::get(sid);
	e.count = value;
}

void symbol_scope(int sid, int value) {
	if(sid == -1)
		return;
	auto& e = bsdata<symboli>::get(sid);
	e.scope = value;
}

const char* string_name(int sid) {
	if(sid == -1)
		return "";
	return strings.get(sid);
}

const char* symbol_name(int sid) {
	if(sid == -1)
		return "";
	auto& e = bsdata<symboli>::get(sid);
	return strings.get(e.ids);
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

int findsym(int ids, int scope) {
	for(auto& e : bsdata<symboli>()) {
		if(e.ids == ids && e.scope == scope)
			return &e - bsdata<symboli>::begin();
	}
	return -1;
}

static int findast(operation_s op, int left, int right) {
	for(auto& e : bsdata<asti>()) {
		if(e.op == op && e.left == left && e.right == right)
			return &e - bsdata<asti>::begin();
	}
	return -1;
}

static bool is_strict_operands(operation_s op) {
	switch(op) {
	case Plus: case Mul: case BinaryOr: case BinaryAnd: case BinaryXor: return false;
	default: return true;
	}
}

int ast_add(operation_s op, int left, int right) {
	auto sid = findast(op, left, right);
	if(sid == -1 && !is_strict_operands(op))
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

static int getscope() {
	if(scopes)
		return scopes[scopes.count - 1];
	return 0;
}

static int ast_add(operation_s op, int value) {
	return ast_add(op, -1, value);
}

static void add_op(int a) {
	operation[0] = a;
	operation++;
}

static void add_op(operation_s op, int value) {
	add_op(ast_add(op, value));
}

static void add_op(operation_s op, int left, int right) {
	add_op(ast_add(op, left, right));
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

static int create_symbol(int id, int type, unsigned flags, int scope) {
	auto p = bsdata<symboli>::add();
	p->ids = id;
	if(scope == -1)
		scope = getscope();
	p->scope = scope;
	p->type = i32;
	p->flags = flags;
	p->value = -1;
	return p - bsdata<symboli>::begin();
}

static int create_symbol(int ids, int type, unsigned flags, int scope, const char* error_format) {
	if(scope == -1)
		scope = getscope();
	auto sid = findsym(ids, scope);
	if(sid != -1) {
		if(error_format)
			error(error_format, string_name(ids));
	} else
		sid = create_symbol(ids, type, flags, scope);
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
	auto sid = findsym(type, PointerScope);
	if(sid != -1)
		return sid;
	return create_symbol(type, type, 0, PointerScope);
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
	last_type = findsym(ids, TypeScope);
	if(last_type == -1) {
		p = p_push;
		return;
	}
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

		} else
			break;
	}
}

static int find_variable(int ids) {
	auto sid = -1;
	for(auto i = (int)scopes.count - 1; sid == -1 && i >= 0; i--)
		sid = findsym(ids, scopes.data[i]);
	if(sid == -1)
		sid = findsym(ids, 0);
	if(sid == -1)
		sid = findsym(ids, TypeScope);
	return sid;
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
			add_op(Cast, type);
		} else {
			parse_expression();
			skip(")");
		}
		break;
	case '\"':
		literal();
		add_op(Text, strings.add(last_string));
		break;
	case '0':case '1':case '2':case '3':case '4':
	case '5':case '6':case '7':case '8':case '9':
		parse_number();
		add_op(Number, last_number);
		break;
	default:
		if(isidentifier()) {
			parse_identifier();
			auto ids = strings.add(last_string);
			auto sid = find_variable(ids);
			if(sid == -1) {
				error("Symbol `%1` not exist", string_name(ids));
				sid = create_symbol(ids, i32, 0, -1);
			}
			add_op(Identifier, sid);
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

static void parse_initialization() {
	if(match("{")) {
		auto result = -1;
		do {
			auto p1 = p;
			parse_expression();
			if(p1 == p)
				break;
			add_list(result, pop_op());
		} while(match(","));
		add_op(Initialization, result);
		skip("}");
	} else
		parse_expression();
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
		auto sid = create_symbol(ids, type, flags, -1);
		parse_array_declaration(sid);
		if(match("="))
			symbol_ast(sid, expression());
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
		add_op(If, s, e);
	} else if(match("swith")) {
		skip("(");
		auto e = expression();
		skip(")");
		parse_statement();
		auto s = pop_op();
		add_op(Switch, s, e);
	} else if(match("while")) {
		skip("(");
		auto e = expression();
		skip(")");
		parse_statement();
		auto s = pop_op();
		add_op(While, s, e);
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
		add_op(Return, expression());
		skip(";");
	} else if(match("case")) {
		add_op(Case, expression());
		skip(":");
	} else if(match("break")) {
		add_op(Break, -1);
		skip(";");
	} else if(match("continue")) {
		add_op(Continue, -1);
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
		auto sid = create_symbol(ids, i32, 0, -1, "Identifier `%1` is already defined");
		symbol_ast(sid, ast_add(Number, index));
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
		parse_type();
		auto type = last_type;
		auto flags = last_flags;
		parse_identifier();
		if(match("="))
			ast = expression();
		auto ids = strings.add(last_string);
		auto sid = create_symbol(ids, type, flags, scope, "Parameter `%1` is already defined");
		symbol_ast(sid, ast);
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
		auto sid = create_symbol(ids, type, flags, -1, "Module member `%1` is already defined");
		if(match("(")) {
			push_visibility_scope();
			parse_parameters();
			if(!match(";")) {
				parse_statement();
				symbol_ast(pop_op());
			}
			pop_visibility_scope();
			break;
		} else {
			parse_array_declaration(sid);
			if(match("="))
				symbol_ast(sid, expression());
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
	auto sid = create_symbol(ids, 0, 0, TypeScope, "Type `%1` already imported");
	if(match("as")) {
		parse_identifier();
		auto als = strings.add(last_string);
	}
}

static void parse_module() {
	while(*p) {
		auto push_p = p;
		parse_import();
		if(p != push_p)
			continue;
		parse_enum();
		if(p != push_p)
			continue;
		parse_declaration();
		if(p != push_p)
			continue;
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
	if(findsym(ids, TypeScope) != -1)
		return;
	create_symbol(strings.add(id), -1, FG(Predefined), TypeScope);
}

static void symbol_initialize() {
	add_symbol(Void, "void");
	add_symbol(i8, "char");
	add_symbol(u8, "uchar");
	add_symbol(i16, "sint");
	add_symbol(u16, "usint");
	add_symbol(i32, "int");
	add_symbol(u32, "unsigned");
	add_symbol(i64, "long");
	add_symbol(u64, "ulong");
}

static void calculator_initialize() {
	symbol_initialize();
	operation = operations;
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
	p = p1; p_start = p1;
	skipws();
	parse_module();
	p_start = push_p_start;
	p = push_p;
	delete p1;
}