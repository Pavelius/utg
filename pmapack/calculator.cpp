#include "calculator.h"
#include "stringbuilder.h"
#include "stringa.h"

BSDATAD(asti)
BSDATAD(symboli)

static int				last_type;
static unsigned			last_flags;
static long				last_number;
static char				last_string[4096];
static const char*		p;
static const char*		p_start;
static stringa			strings;
static adat<int>		operations;
static adat<int>		scopes;

static void parse_expression();
static int expression();

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

static bool isnumber() {
	return isnum(*p);
}

static bool isidentifier() {
	return ischa(*p);
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

static int ast_add(operation_s op, int left) {
	return ast_add(op, left, -1);
}

static void pushop(operation_s op, int value) {
	operations.add(ast_add(op, value));
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

static void parse_type() {
	last_type = -1;
	last_flags = 0;
	if(!isidentifier())
		return;
	auto p_push = p;
	parse_flags();
	parse_identifier();
	auto ids = strings.add(last_string);
	last_type = findsym(ids, Modules, 0);
	if(last_type == -1) {
		p = p_push;
		return;
	}
}

static bool parse_member_declaration() {
	auto push_p = p;
	parse_flags();
	parse_type();
	if(last_type==-1) {
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
	while(*p && (ischa(*p) || isnum(*p) || *p == '_' || *p=='.'))
		sb.add(*p++);
	skipws();
}

static int getscope() {
	if(scopes)
		return scopes[scopes.count - 1];
	return 0;
}

static int create_symbol(int id, int parent, int type, unsigned flags, int scope = -1) {
	auto p = bsdata<symboli>::add();
	p->ids = id;
	p->parent = parent;
	if(scope == -1)
		scope = getscope();
	p->scope = scope;
	p->type = i32;
	p->flags = flags;
	p->value = -1;
	return p - bsdata<symboli>::begin();
}

static int getop(int top) {
	return operations.data[operations.count - top];
}

static void unary_operation(operation_s v) {
	if(operations.count < 1) {
		error("Unary operations stack corupt");
		return;
	}
	auto p1 = getop(-1);
	operations.data[operations.count - 1] = ast_add(v, p1);
}

static void binary_operation(operation_s v) {
	if(operations.count < 2) {
		error("Binary operations stack corupt");
		return;
	}
	auto p1 = getop(-1);
	auto p2 = getop(-2);
	operations.data[operations.count - 2] = ast_add(v, p1, p2);
	operations.count--;
}

static void postfix() {
	while(*p) {
		if(match("(")) {
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
		sid = findsym(ids, This, scopes.data[i]);
	if(sid == -1)
		sid = findsym(ids, This, 0);
	if(sid == -1)
		sid = findsym(ids, Modules, 0);
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
		if(last_type != -1 && *p==')') {
			skip(")");
			auto type = last_type;
			unary();
			pushop(Cast, type);
		} else {
			parse_expression();
			skip(")");
		}
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
		if(isidentifier()) {
			parse_identifier();
			auto ids = strings.add(last_string);
			auto sid = find_variable(ids);
			if(sid == -1)
				sid = create_symbol(ids, This, i32, 0); // TODO: error if exist
			pushop(Identifier, sid);
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

static void parse_statement() {
	if(match("{")) {
		scopes.add(getmoduleposition());
		auto push_p = p;
		while(*p && *p != '}') {
			parse_statement();
			if(p == push_p)
				break;
		}
		scopes.count--;
		skip("}");
	} else if(match(";")) {
		// Empthy statement
	} else if(match("if")) {
		skip("(");
		expression();
		skip(")");
		parse_statement();
	} else if(match("swith")) {
		skip("(");
		expression();
		skip(")");
		parse_statement();
	} else if(match("while")) {
		skip("(");
		expression();
		skip(")");
		parse_statement();
	} else if(match("for")) {
		skip("(");
		expression();
		skip(";");
		expression();
		skip(";");
		expression();
		skip(")");
		parse_statement();
	} else if(match("return")) {
		expression();
		skip(";");
	} else if(match("case")) {
		expression();
		skip(":");
	} else if(match("break")) {
		skip(";");
	} else if(match("continue")) {
		skip(";");
	} else {
		parse_assigment();
		skip(";");
	}
}

static void parse_enum() {
	if(!word("enum"))
		return;
	auto index = 0;
	skip("{");
	while(*p) {
		parse_identifier();
		if(!match(","))
			break;
	}
	skip("}");
	skip(";");
}

static int match_type() {
	auto id = strings.add(last_string);
	auto sid = findsym(id, Modules, 0);
	return sid;
}

static void parse_declaration() {
	if(!parse_member_declaration())
		return;
	auto type = last_type;
	auto flags = last_flags;
	while(isidentifier()) {
		parse_identifier();
		auto ids = strings.add(last_string);
		if(match("(")) {
			auto sid = create_symbol(ids, This, type, flags, -1);
			// TODO: parameter list
			skip(")");
			if(match(";"))
				break; // TODO: forward declaration
			parse_statement();
			break;
		} else {
			auto sid = create_symbol(ids, This, type, flags, -1);
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
	if(!word("import"))
		return;
	parse_url_identifier();
	skip(";");
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
		error("Can't find file `%1`", url);
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