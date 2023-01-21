//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		22-1-2023
//			Author:			í√ìcêLèG
//			Description:
//
//----------------------------------------------------------------------

#include "Board.h"

Board::Board() {
	init();
}
void Board::init() {
	for(int i = 0; i != BD_SIZE; ++i)
		m_board[i] = EMPTY;
	for(int i = 0; i != GBD_SIZE; ++i)
		m_gboard[i] = EMPTY;
}
