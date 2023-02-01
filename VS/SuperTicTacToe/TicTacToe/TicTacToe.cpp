#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
	Board bd;
	bd.print();
#if 0
	bd.put(1, 2, WHITE);
	bd.put(2, 1, BLACK);
	bd.print();
	cout << (bd.is_game_over() ? "Game Over.\n" : "Not Game Over.\n");
	bd.put(0, 1, WHITE);
	bd.put(1, 1, WHITE);
	bd.put(2, 1, WHITE);
	bd.print();
	cout << (bd.is_game_over() ? "Game Over.\n" : "Not Game Over.\n");
#endif
#if 0
	auto mv = bd.sel_move_random();
	bd.put(mv, WHITE);
	bd.print();
#endif
#if 0
	while( !bd.is_game_over() ) {
		auto mv = bd.sel_move_random();
		bd.put(mv, bd.next_color());
		bd.print();
	}
#endif
	const int N = 1000;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			//if( is_empty(x, y) ) {
			//}
			Board bd;
			bd.put(x, y, WHITE);
			auto r = bd.playout_random(N);
			cout << (double)r/N << "\t";
		}
		cout << "\n";
	}
	//
    std::cout << "\nOK.\n";
}
