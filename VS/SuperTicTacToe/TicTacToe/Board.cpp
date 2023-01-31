//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		31-1-2023
//			Author:			í√ìcêLèG
//			Description:
//
//----------------------------------------------------------------------

#include <iostream>
#include "Board.h"

using namespace std;

Board::Board() {
	init();
}
void Board::init() {
	for(int i = 0; i != BD_SIZE; ++i) m_board[i] = EMPTY;
}
void Board::print() const {
	int ix = 0;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "ÅE"; break;
			case WHITE:	cout << "Çn"; break;
			case BLACK:	cout << "Çw"; break;
			}
		}
		cout << "\n";
	}
	cout << "\n";
}
