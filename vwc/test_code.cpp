#include "code_rule.h"
#include "logparse.h"

using namespace code;

//static ruleop operator_or[] = {{"||", operation::Or}};
//static ruleop operator_and[] = {{"&&", operation::And}};
//static ruleop operator_shift[] = {{">>", operation::ShiftRight}, {"<<", operation::ShiftLeft}};
//static ruleop operator_bor[] = {{"|", operation::BinaryOr}};
//static ruleop operator_bxor[] = {{"^", operation::BinaryÕîr}};
//static ruleop operator_band[] = {{"&", operation::BinaryAnd}};
//static ruleop operator_cond[] = {{">=", operation::GreaterEqual}, {">", operation::Greater},
//	{"<=", operation::LessEqual}, {"<", operation::Less},
//	{"!=", operation::NotEqual}, {"==", operation::Equal}
//};
//static ruleop operator_add[] = {{"+", operation::Plus}, {"-", operation::Minus}};
//static ruleop operator_mul[] = {{"*", operation::Mul}, {"/", operation::Div}, {"%", operation::DivRest}};

//static rule c2_grammar[] = {
//	{"global", {"^%import", "%enum", "%declare_function", "%declare_variable"}},
//	{"number", {"@number"}},
//	{"string", {"@string"}},
//	{"identifier", {"@identifier"}},
//
//	{"import", {"import", "@declaration", "@set_url", ". %identifier", "?%pseudoname", ";", "@add_type"}},
//	{"pseudoname", {"as", "%identifier"}},
//
//	{"enum", {"enum", "{", ", %enum_value", "}", ";"}},
//	{"enum_value", {"%identifier", "?%enum_assign"}},
//	{"enum_assign", {"=", "%constant"}},
//
//	{"constant", {"%expression"}},
//
//	{"type_reference", {".*", "@type_reference"}},
//	{"type", {"%identifier", "@set_type", "%?type_reference"}},
//	{"initialization", {"=", "%expression", "@set_symbol_ast"}},
//	{"static", {"static", "@set_static"}},
//	{"public", {"public", "@set_public"}},
//	{"parameter", {"%type", "%identifier"}},
//
//	{"declare_function", {"@declaration", "?%static", "?%public", "%type", "%identifier", "(", "@set_function", "@add_member", ", ?%parameter", ")", "%block_statements", "@set_symbol_ast"}},
//	{"declare_variable_loop", {"%identifier", "@add_member", "?%array_scope", "?%initialization"}},
//	{"declare_variable", {"@declaration", "?%static", "?%public", "%type", ", %declare_variable_loop", ";"}},
//	{"declare_local", {"@declaration", "?%static", "%type", ", %declare_variable_loop", ";"}},
//
//	{"call", {"(", "?, %expression", ")"}},
//	{"variable", {"%identifier", "@add_variable"}},
//	{"sizeof", {"sizeof", "(", "%expression", ")"}},
//	{"array_scope", {"[", "%expression", "]"}},
//	{"unary", {"^%number", "%string", "%sizeof", "%variable"}},
//	{"indirection", {"\\.", "%identifier"}},
//	{"postfix", {"^\\++", "\\--", "%indirection", "%call", "%array_scope"}},
//	{"postfix", {"%unary", "?.%postfix_op"}},
//	{"prefix_op", {"^?\\++", "\\--", "\\&"}},
//	{"prefix", {"?.%prefix_op", "%unary"}},
//	{"multiplication_op", {"^\\/", "\\*"}, set_operation},
//	{"multiplication_op_state", {"%multiplication_op", "%postfix"}},
//	{"multiplication", {"%postfix", "?.%multiplication_op_state"}},
//	{"addiction_op", {"^\\+", "\\-"}, set_operation},
//	{"addiction_op_state", {"%addiction_op", "%multiplication"}},
//	{"addiction", {"%multiplication", "?.%addiction_op_state"}},
//	{"binary_op", {"^\\|", "\\&", "\\^"}, set_operation},
//	{"binary_op_state", {"%binary_op", "%addiction"}},
//	{"binary", {"%addiction", "?.%binary_op_state"}},
//	{"conditional_op", {"^\\<=", "\\>=", "\\==", "\\!=", "\\>", "\\<"}, set_operation},
//	{"conditional_op_state", {"%conditional_op", "%binary"}},
//	{"conditional", {"%binary", "?.%conditional_op_state"}},
//	{"logical_op", {"^\\||", "\\&&"}, set_operation},
//	{"logical_op_state", {"%logical_op", "%conditional"}},
//	{"logical", {"%conditional", "?.%logical_op_state"}},
//	{"expression", {"%logical"}},
//
//	{"expression", {"@expression"}},
//	{"assigment", {"%variable", "=", "%expression", ";"}},
//	{"statement_expression", {"%expression", ";"}},
//
//	{"break", {"break", ";"}},
//	{"default", {"default", ":"}},
//	{"case", {"case", "%expression", ":"}},
//	{"while", {"while", "(", "%expression", ")", "%single_statement"}},
//	{"if", {"if", "(", "%expression", ")", "%single_statement"}},
//	{"switch", {"switch", "(", "%expression", ")", "%block_statements"}},
//	{"return", {"return", "?%expression", ";"}},
//
//	{"statement", {"^%return", "%if", "%while", "%switch", "%break", "%case", "%default", "%declare_local", "%assigment", "%statement_expression"}},
//	{"block_statements", {"{", "@push_locals", ".?%single_statement", "@pop_locals", "}"}},
//	{"single_statement", {"^%block_statements", ";", "%statement"}},
//};