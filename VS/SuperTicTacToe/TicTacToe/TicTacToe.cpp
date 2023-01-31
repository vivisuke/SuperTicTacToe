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
	auto mv = bd.sel_move_random();
	bd.put(mv, WHITE);
	bd.print();
	//
    std::cout << "\nOK.\n";
}
