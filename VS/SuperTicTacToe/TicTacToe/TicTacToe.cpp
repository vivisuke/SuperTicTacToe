﻿#include <iostream>
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
#if 0
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
	cout << "\n";
#endif
#if 0
	cout << "bd.sel_move_PMC():\n";
	auto mv = bd.sel_move_PMC();
	cout << "(" << (int)mv.m_x << ", " << (int)mv.m_y << ")\n\n";
	bd.put(mv, bd.next_color());
	bd.print();
	mv = bd.sel_move_PMC();
	cout << "(" << (int)mv.m_x << ", " << (int)mv.m_y << ")\n\n";
#endif
#if 1
	while( !bd.is_game_over() ) {
		auto mv = bd.sel_move_MC(true);
		cout << "(" << (int)mv.m_x << ", " << (int)mv.m_y << ")\n\n";
		bd.put(mv, bd.next_color());
		bd.print();
		cout << "\n";
	}
#endif
#if		0
	auto mv = bd.sel_move_random();
	bd.put(mv, bd.next_color());
	bd.print();
	mv = bd.sel_move_random();
	bd.put(mv, bd.next_color());
	bd.print();
	bd.undo_put();
	bd.print();
	bd.undo_put();
	bd.print();
#endif
	//
    std::cout << "\nOK.\n";
}
