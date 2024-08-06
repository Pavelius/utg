#include "calculator.h"
#include "stringbuilder.h"
#include "stringa.h"

BSDATAD(asti)
BSDATAD(symboli)

static long				last_number;
static char				last_string[4096];
static const char*		p;
static const char*		p_start;
static stringa			strings;
static adat<int>		operations;
static adat<int>		scopes;

static void parse_expression();

void symbol_code_set(int sid, int value) {
	if(sid == -1)
		return;
	auto& e = bsdata<symboli>::get(sid);
	e.value = value;
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

static void error(const char* format, ...) {
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

static int findsym(int ids, int parent, int scope) {
	for(auto& e : bsdata<symboli>()) {
		if(e.ids == ids && e.parent == parent && e.scope == scope)
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

int ast_add(operation_s op, int left, int right) {
	auto ids = findast(op, left, right);
	if(ids == -1) {
		auto p = bsdata<asti>::add();
		p->op = op;
		p->left = left;
		p->right = right;
		ids = p - bsdata<asti>::begin();
	}
	return ids;
}

static int addop(operation_s op, int value) {
	return ast_add(op, value, -1);
}

static void pushop(operation_s op, int value) {
	operations.add(addop(op, value));
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
	if(!ischa(*p))
		error("Expected identifier");
	while(*p && (ischa(*p) || isnum(*p) || *p == '_'))
		sb.add(*p++);
	skipws();
}

static int get_scope() {
	if(scopes)
		return scopes[scopes.count - 1];
	return 0;
}

static int create_symbol(int id, int parent, int type, unsigned flags, int scope = -1) {
	auto p = bsdata<symboli>::add();
	p->ids = id;
	p->parent = parent;
	if(scope == -1)
		scope = get_scope();
	p->scope = scope;
	p->type = i32;
	p->flags = flags;
	return p - bsdata<symboli>::begin();
}

static void parse_flags(unsigned& flags) {
	flags = 0;
	while(*p) {
		if(word("static"))
			flags |= FG(Static);
		else if(word("public"))
			flags |= FG(Public);
		else
			break;
	}
}

static void unary_operation(operation_s v) {
	if(operations.count < 1) {
		error("Unary operations stack corupt");
		return;
	}
	auto p1 = operations.data[operations.count - 1];
	operations.data[operations.count - 1] = ast_add(v, p1, -1);
}

static void binary_operation(operation_s v) {
	if(operations.count < 2) {
		error("Binary operations stack corupt");
		return;
	}
	auto p1 = operations.data[operations.count - 1];
	auto p2 = operations.data[operations.count - 2];
	operations.data[operations.count - 2] = ast_add(v, p1, p2);
	operations.count--;
}

static void postfix() {
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
		parse_expression();
		skip(")");
		break;
	case '\"':
		literal();
		pushop(Text, strings.add(last_string));
		break;
	case '0':case '1':case '2':case '3':case '4':
	case '5':case '6':case '7':case '8':case '9':
		parse_number();
		pushop(Number, last_number);
		break;
	default:
		if(ischa(*p)) {
			unsigned flags;
			parse_flags(flags);
			parse_identifier();
			auto id = strings.add(last_string);
			auto sid = -1;
			for(auto i = (int)scopes.count - 1; sid == -1 && i >= 0; i--)
				sid = findsym(id, This, scopes.data[i]);
			if(sid == -1)
				sid = findsym(id, This, 0);
			if(sid == -1)
				sid = findsym(id, Modules, 0);
			if(sid == -1)
				sid = create_symbol(id, This, i32, flags);
			pushop(Identifier, sid);
		} else
			error("Unknown symbol");
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
		binary_operation(BinaryÕîr);
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
		binary_operation(LeftIfTrue);
		binary_operation(BooleanChoose);
	}
}

static int expression() {
	parse_expression();
	if(operations.count < 1) {
		error("Expression stack corruption");
		return -1;
	}
	return operations.data[operations.count--];
}

static void parse_enum() {
	while(word("enum")) {
	}
}

static int match_type() {
	auto id = strings.add(last_string);
	auto sid = findsym(id, Modules, 0);
	return sid;
}

static void parse_declaration() {
	auto push_p = p;
	unsigned flags = 0;
	parse_flags(flags);
	parse_identifier();
	if(!last_string[0]) {
		p = push_p;
		return;
	}
	auto type = findsym(strings.add(last_string), Modules, 0);
	if(type == -1) {
		p = push_p;
		return;
	}
	if(!ischa(*p)) {
		p = push_p;
		return;
	}
	while(ischa(*p)) {
		parse_identifier();
		auto id = strings.add(last_string);
		if(match("(")) {
			auto sid = create_symbol(id, This, type, flags, -1);
			skip(")");
			skip(";");
			break;
		} else {
			auto sid = create_symbol(id, This, type, flags, -1);
			if(match("="))
				symbol_code_set(sid, expression());
			if(match(","))
				continue;
			skip(";");
			break;
		}
	}
}

static void parse_import() {
	while(word("import")) {
	}
}

static void parse_module() {
	parse_import();
	while(*p) {
		auto push_p = p;
		parse_enum();
		parse_declaration();
		if(p == push_p) {
			error("Expected declaration");
			break;
		}
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
	if(findsym(ids, This, 0) != -1)
		return;
	create_symbol(strings.add(id), Modules, v, 0);
}

void calculator_initialize() {
	add_symbol(This, "this");
	add_symbol(Systems, ".systems");
	add_symbol(Modules, ".modules");
	add_symbol(Pointers, ".pointers");
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

void calculator_file_parse(const char* url) {
	auto p1 = loadt(url);
	if(!p1) {
		error("Can't find file");
		return;
	}
	auto push_p = p;
	auto push_p_start = p_start;
	p = p1; p_start = p1;
	skipws();
	parse_module();
	p_start = push_p_start;
	p = push_p;
	delete p1;
}