#include <iostream>
#include <chrono>
#include <assert.h>
#include "Board.h"

using namespace std;

extern int g_count;
extern int g_eval_count;

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
    bd.put(0, 0, WHITE);
	auto mv = bd.sel_move_AlphaBeta(3);
	cout << (int)mv.m_x << ", " << (int)mv.m_y << "\n";
#if 0
    auto ev1 = bd.eval();
    auto ev2 = bd.eval_index();
    auto ev3 = bd.eval_diff_index();
    assert( ev1 == ev2 );
    assert( ev1 == ev3 );
    bd.put(0, 0, WHITE);
    ev1 = bd.eval();
    ev2 = bd.eval_index();
    ev3 = bd.eval_diff_index();
    assert( ev1 == ev2 );
    assert( ev1 == ev3 );
#endif
#if 0
    auto start = std::chrono::system_clock::now();      // 計測スタート時刻を保存
    int wwin = 0, bwin = 0;
    const int LOOP = 100;
    Move mv;
    for(int i = 0; i != LOOP; ++i) {
    	cout << ".";
	    Board bd;
		while( !bd.is_game_over() ) {
			if( bd.is_white_turn() ) {
				mv = bd.sel_move_random();
				//mv = bd.sel_move_MinMax(3);
				//mv = bd.sel_move_AlphaBeta(9);
				//if( mv.m_x < 0 )
				//	mv = bd.sel_move_AlphaBeta(5);
			} else {
				//mv = bd.sel_move_random();
				//mv = bd.sel_move_MinMax(3);
				mv = bd.sel_move_AlphaBeta(7);
				//if( mv.m_x < 0 )
				//	mv = bd.sel_move_AlphaBeta(5);
			}
			bd.put(mv, bd.next_color());
		}
		if( bd.winner() == WHITE ) ++wwin;
		else if( bd.winner() == BLACK ) ++bwin;
    }
    auto end = std::chrono::system_clock::now();       // 計測終了時刻を保存
    auto dur = end - start;        // 要した時間を計算
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
   	cout << "\n";
    cout << "white won: " << (100.0)*wwin/LOOP << "%\n";
    cout << "black won: " << (100.0)*bwin/LOOP << "%\n";
    cout << "draw: " << (100.0)*(LOOP - wwin - bwin)/LOOP << "%\n";
    cout << "\ndur = " << msec << "millisec\n";
#endif

#if 0
    bd.print();
    Move mv;
	while( !bd.is_game_over() ) {
	    auto ev1 = bd.eval();
	    auto ev2 = bd.eval_index();
	    auto ev3 = bd.eval_diff_index();
	    assert( ev1 == ev2 );
	    assert( ev1 == ev3 );
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
			//mv = bd.sel_move_random();
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
