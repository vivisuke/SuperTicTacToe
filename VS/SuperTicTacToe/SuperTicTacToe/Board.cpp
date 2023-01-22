//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		22-1-2023
//			Author:			�Óc�L�G
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
	for(int i = 0; i != BD_SIZE; ++i)
		m_board[i] = EMPTY;
	for(int i = 0; i != GBD_SIZE; ++i)
		m_gboard[i] = EMPTY;
}
void Board::print() const {
	//	�S���[�J���{�[�h�\��
	int ix = 0, gx = 0;
	cout << "�{�|�|�|�{�|�|�|�{�|�|�|�{�@�{�|�|�|�{\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << "�b";
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "�E"; break;
			case MARU:	cout << "�n"; break;
			case BATSU:	cout << "�w"; break;
			}
			if( x%3 == 2 ) cout << "�b";
		}
		if( y < N_VERT/3 ) {
			cout << "�@�b";
			for(int x = 0; x != N_HORZ/3; ++x) {
				switch( m_gboard[gx++] ) {
				case EMPTY:	cout << "�E"; break;
				case MARU:	cout << "�n"; break;
				case BATSU:	cout << "�w"; break;
				}
			}
			cout << "�b";
		}
		cout << "\n";
		if( y%3 == 2 )
			if( y == 2 )
				cout << "�{�|�|�|�{�|�|�|�{�|�|�|�{�@�{�|�|�|�{\n";
			else
				cout << "�{�|�|�|�{�|�|�|�{�|�|�|�{\n";
	}
	cout << "\n";
#if 0
	//	�O���[�o���{�[�h�\��
	ix = 0;
	for(int y = 0; y != N_VERT/3; ++y) {
		for(int x = 0; x != N_HORZ/3; ++x) {
			switch( m_gboard[ix++] ) {
			case EMPTY:	cout << "�E"; break;
			case MARU:	cout << "�n"; break;
			case BATSU:	cout << "�w"; break;
			}
		}
		cout << "\n";
	}
	cout << "\n";
#endif
}
