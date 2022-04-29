#include "bsreq.h"
#include "drawobject.h"
#include "main.h"
#include "strategy.h"

#ifdef _DEBUG

void util_main();

#endif // _DEBUG

void update_board();

void main_menu() {
	update_board();
	answers an;
	an.header = "�����������";
	for(auto& e : bsdata<locationi>())
		an.add(&e, getnm(e.id));
	an.chooseui("������� ���� �� ������ �������������", "������");
}

static void initialization() {
	bsreq::read("rules/Worldmap.txt");
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	draw::object::initialize();
	return draw::strategy(main_menu, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}