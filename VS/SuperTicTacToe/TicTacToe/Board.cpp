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
	m_game_over = false;
	for(int i = 0; i != BD_SIZE; ++i) m_board[i] = EMPTY;
}
void Board::print() const {
	int ix = 0;
	cout << "  ÇÅÇÇÇÉ\n";
	cout << " +------+\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << (y+1) << "|";
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "ÅE"; break;
			case WHITE:	cout << "Çn"; break;
			case BLACK:	cout << "Çw"; break;
			}
		}
		cout << "|\n";
	}
	cout << " +------+\n";
	cout << "\n";
}
void Board::put(int x, int y, char col) {
	m_board[xyToIndex(x, y)] = col;
	//	èIã«É`ÉFÉbÉN
	if( m_board[xyToIndex(0, y)] + m_board[xyToIndex(1, y)] + m_board[xyToIndex(2, y)] == col * 3 )
		m_game_over = true;
	else if( m_board[xyToIndex(y, 0)] + m_board[xyToIndex(x, 1)] + m_board[xyToIndex(x, 2)] == col * 3 )
		m_game_over = true;
	else if( x == y && m_board[xyToIndex(0, 0)] + m_board[xyToIndex(1, 1)] + m_board[xyToIndex(2, 2)] == col * 3 )
		m_game_over = true;
	else if( x == 2 - y && m_board[xyToIndex(2, 0)] + m_board[xyToIndex(1, 1)] + m_board[xyToIndex(0, 2)] == col * 3 )
		m_game_over = true;
}
