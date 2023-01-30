#include "code_package.h"
#include "io_stream.h"

using namespace code;

package* code::last_package;

bool isnostrictorder(operation id) {
	switch(id) {
	case operation::Plus:
	case operation::Mul:
		return true;
	default:
		return false;
	}
}

static void addg(package* p, const char* id) {
	p->add(p->add(id), This, This, 0, FG(Standart), 0);
}

static void addm(package* p, const char* id) {
	p->add(p->add(id), 0, Modules, 0, FG(Standart), 0);
}

void package::create(const char* name) {
	addm(this, name);
	addg(this, "Systems");
	addg(this, "Modules");
	addg(this, "Pointers");
	addm(this, "void");
	addm(this, "i8");
	addm(this, "u8");
	addm(this, "i16");
	addm(this, "u16");
	addm(this, "i32");
	addm(this, "u32");
	addm(this, "i64");
	reference(i8);
	reference(Void);
}

void package::clear() {
	strings.clear();
	symbols.clear();
	asts.clear();
}

pckh package::findsym(pckh id, pckh parent) const {
	for(auto& e : symbols) {
		if(e.id == id && e.parent == parent)
			return &e - symbols.begin();
	}
	return None;
}

pckh package::findsym(pckh id, pckh parent, pckh type) const {
	for(auto& e : symbols) {
		if(e.id == id && e.parent == parent && e.result == type)
			return &e - symbols.begin();
	}
	return None;
}

pckh package::findsymscope(pckh id, pckh parent, unsigned scope) const {
	for(auto& e : symbols) {
		if(e.id == id && e.parent == parent && e.scope==scope)
			return &e - symbols.begin();
	}
	return None;
}

pckh package::findsym(const char* id, pckh parent) const {
	auto nid = strings.find(id);
	if(nid == None)
		return None;
	return findsym(nid, parent);
}

pckh package::findast(operation type, pckh left, pckh right) const {
	for(auto& e : asts) {
		if(e.type == type && e.left == left && e.right == right)
			return &e - asts.begin();
	}
	return None;
}

pckh package::add(pckh id, pckh parent, pckh result, unsigned index, unsigned flags, unsigned scope) {
	pckh v = None;
	if(parent==Pointers)
		v = findsym(id, parent, result);
	else
		v = findsymscope(id, parent, scope);
	if(v == None) {
		auto p = symbols.add();
		p->id = id;
		p->parent = parent;
		p->result = result;
		p->index = index;
		p->flags = flags;
		p->scope = scope;
		p->ast = None;
		v = p - symbols.begin();
	}
	return v;
}

pckh package::add(operation type, pckh left, pckh right) {
	auto v = findast(type, left, right);
	if(v != None)
		return v;
	if(isnostrictorder(type)) {
		v = findast(type, right, left);
		if(v != None)
			return v;
	}
	auto p = asts.add();
	p->type = type;
	p->left = left;
	p->right = right;
	return p - asts.begin();
}

static void serialx(io::stream& file, array& e, bool write_mode) {
	if(write_mode) {
		file.write(&e.count, sizeof(e.count));
		if(e.count)
			file.write(e.begin(), e.count * e.getsize());
	} else {
		e.clear();
		unsigned v = 0;
		file.read(&v, sizeof(v));
		if(v) {
			e.reserve(v);
			e.setcount(v);
			file.read(e.begin(), e.count * e.getsize());
		}
	}
}

bool package::serial(const char* url, bool write_mode) {
	io::file file(url, write_mode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	if(!file.version("PKG", 0, TypePointer, write_mode))
		return false;
	serialx(file, strings, write_mode);
	serialx(file, symbols, write_mode);
	serialx(file, asts, write_mode);
	return true;
}

pckh package::reference(pckh type) {
	return add(0, Pointers, type, 0, 0, 0);
}