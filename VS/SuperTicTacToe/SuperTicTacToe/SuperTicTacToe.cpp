#include <iostream>
#include "Board.h"

using namespace std;

extern int g_count;

void do_search(Board &bd, int depth) {
	if( bd.is_game_over() || depth == 0 ) {
		++g_count;
		if( g_count == 1 ) bd.print();
		return;
	}
	Moves mvs;
	bd.gen_moves(mvs);
	for(int i = 0; i != mvs.size(); ++i) {
		bd.put(mvs[i], bd.next_color());
		do_search(bd, depth-1);
		bd.undo_put();
	}
}

int main()
{
	build_3x3_eval_table();
	//
    Board bd;
#if 1
    bd.print();
    Move mv;
	while( !bd.is_game_over() ) {
		cout << "eval = " << bd.eval() << "\n\n";
		//auto mv = bd.sel_move_random();
		//auto mv = bd.sel_move_Depth1();
		//auto mv = bd.sel_move_MinMax(3);
		if( bd.is_white_turn() ) {
			//mv = bd.sel_move_random();
			//mv = bd.sel_move_MinMax(3);
			mv = bd.sel_move_AlphaBeta(9);
			//if( mv.m_x < 0 )
			//	mv = bd.sel_move_AlphaBeta(5);
		} else {
			//mv = bd.sel_move_MinMax(3);
			mv = bd.sel_move_AlphaBeta(9);
			//if( mv.m_x < 0 )
			//	mv = bd.sel_move_AlphaBeta(5);
		}
		bd.put(mv, bd.next_color());
		bd.print();
	}
	cout << "eval = " << bd.eval() << "\n\n";
#endif
#if 0
    g_count = 0;
    do_search(bd, 3);
    cout << "g_count = " << g_count << "\n";
#endif
#if 0
    bd.print();
    bd.put(0, 0, WHITE);
    //bd.print();
    bd.put(1, 1, WHITE);
    //bd.print();
    bd.put(2, 2, WHITE);
    bd.print();
    bd.put(3, 3, BLACK);
    bd.print();
    bd.put(4, 3, BLACK);
    bd.put(5, 3, BLACK);
    bd.print();
    bd.undo_put();
    bd.print();
#endif
#if 0
    for(int i = 0; i != 100; ++i) {
	    auto mv = bd.sel_move_random();
	    bd.put(mv.m_x, mv.m_y, bd.next_color());
	    bd.print();
	    if( bd.is_game_over() )
            break;
    }
#endif
#if	0
	auto w = bd.playout_random();
    bd.print();
    //cout << "winner = " << (int)w << "\n";
    cout << "winner = " << (w==WHITE?"O":w==BLACK?"X":"none") << "\n";
#endif
#if	0
    const int N = 10000;
    for(int y = 0; y != N_VERT; ++y) {
	    for(int x = 0; x != N_HORZ; ++x) {
		    bd.put(x, y, WHITE);
		    auto sum = bd.playout_random(N);
		    //cout << "sum = " << sum << "\n";
		    //cout << sum << " ";
		    cout << (double)sum/N << " ";
		    bd.undo_put();
	    }
	    cout << "\n";
    }
#endif
    //
    std::cout << "\nOK.\n";
}
