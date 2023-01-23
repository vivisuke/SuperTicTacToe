#include <iostream>
#include "Board.h"

using namespace std;

int main()
{
    Board bd;
#if 0
    bd.print();
    bd.put(0, 0, MARU);
    //bd.print();
    bd.put(1, 1, MARU);
    //bd.print();
    bd.put(2, 2, MARU);
    bd.print();
    bd.put(3, 3, BATSU);
    bd.print();
    bd.put(4, 3, BATSU);
    bd.put(5, 3, BATSU);
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
	auto w = bd.playout_random();
    bd.print();
    //cout << "winner = " << (int)w << "\n";
    cout << "winner = " << (w==MARU?"O":w==BATSU?"X":"none") << "\n";
    //
    std::cout << "\nOK.\n";
}
