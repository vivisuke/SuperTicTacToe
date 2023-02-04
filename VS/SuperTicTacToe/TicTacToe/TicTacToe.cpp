#include <iostream>
#include "Board.h"

using namespace std;

unordered_map<int, int> g_map;

void gen_all_position(Board &bd) {
	auto hv = bd.hash();
	if( g_map.find(hv) == g_map.end() ) {		//	未登録の場合
		if( bd.is_game_over() ) {
			g_map[hv] = bd.winner();
		} else {
			for(int y = 0; y != N_VERT; ++y) {
				for(int x = 0; x != N_HORZ; ++x) {
					if( bd.is_empty(x, y) ) {
						bd.put(x, y, bd.next_color());
						gen_all_position(bd);
						bd.undo_put();
					}
				}
			}
		}
	}
}

int main()
{
	Board bd;
	bd.print();
	gen_all_position(bd);
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
#if 0
	while( !bd.is_game_over() ) {
		auto mv = bd.sel_move_MC(true);
		cout << "(" << (int)mv.m_x << ", " << (int)mv.m_y << ")\n\n";
		bd.put(mv, bd.next_color());
		bd.print();
		cout << "\n";
	}
#endif
#if 0
	const int N_GAME = 1000;
	int ww = 0, bw = 0;
	Move mv;
	for(int i = 0; i != N_GAME; ++i) {
		if( (i % 100) == 0 ) cout << ".";
		Board bd;
		while( !bd.is_game_over() ) {
			//mv = bd.sel_move_random();
			//mv = bd.sel_move_MC(false);
			//mv = bd.sel_move_MC(true);
			if( bd.next_color() == WHITE ) {
				//mv = bd.sel_move_random();
				//mv = bd.sel_move_MC(false);
				mv = bd.sel_move_MC(true);
			} else {
				//mv = bd.sel_move_random();
				mv = bd.sel_move_MC(false);
				//mv = bd.sel_move_MC(true);
			}
			//cout << "(" << (int)mv.m_x << ", " << (int)mv.m_y << ")\n\n";
			bd.put(mv, bd.next_color());
		}
		if( bd.winner() == WHITE ) ++ww;
		else if( bd.winner() == BLACK ) ++bw;
	}
	cout << "\n";
	cout << "white won: " << ww*100.0/N_GAME << "%\n";
	cout << "black won: " << bw*100.0/N_GAME << "%\n";
	cout << "draw: " << (N_GAME-ww-bw)*100.0/N_GAME << "%\n";
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
#if 0
	cout << "hash = " << bd.hash() << "\n";
	bd.put(0, 0, WHITE);
	bd.print();
	cout << "hash = " << bd.hash() << "\n";
	bd.undo_put();
	bd.put(2, 0, WHITE);
	bd.print();
	cout << "hash = " << bd.hash() << "\n";
	bd.undo_put();
	bd.put(0, 2, WHITE);
	bd.print();
	cout << "hash = " << bd.hash() << "\n";
	bd.undo_put();
	bd.put(2, 2, WHITE);
	bd.print();
	cout << "hash = " << bd.hash() << "\n";
	//bd.put(2, 1, BLACK);
	//bd.print();
	//cout << "hash = " << bd.hash() << "\n";
	bd.undo_put();
	bd.put(1, 0, WHITE);
	bd.print();
	cout << "hash = " << bd.hash() << "\n";
	bd.undo_put();
	bd.put(0, 1, WHITE);
	bd.print();
	cout << "hash = " << bd.hash() << "\n";
#endif
	//
    std::cout << "\nOK.\n";
}
