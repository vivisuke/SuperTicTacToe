//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		22-1-2023
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
	for(int i = 0; i != BD_SIZE; ++i)
		m_board[i] = EMPTY;
	for(int i = 0; i != GBD_SIZE; ++i)
		m_gboard[i] = EMPTY;
}
void Board::print() const {
	//	ëSÉçÅ[ÉJÉãÉ{Å[Éhï\é¶
	int ix = 0, gx = 0;
	cout << "Å{Å|Å|Å|Å{Å|Å|Å|Å{Å|Å|Å|Å{Å@Å{Å|Å|Å|Å{\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << "Åb";
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "ÅE"; break;
			case MARU:	cout << "Çn"; break;
			case BATSU:	cout << "Çw"; break;
			}
			if( x%3 == 2 ) cout << "Åb";
		}
		if( y < N_VERT/3 ) {
			cout << "Å@Åb";
			for(int x = 0; x != N_HORZ/3; ++x) {
				switch( m_gboard[gx++] ) {
				case EMPTY:	cout << "ÅE"; break;
				case MARU:	cout << "Çn"; break;
				case BATSU:	cout << "Çw"; break;
				}
			}
			cout << "Åb";
		}
		cout << "\n";
		if( y%3 == 2 )
			if( y == 2 )
				cout << "Å{Å|Å|Å|Å{Å|Å|Å|Å{Å|Å|Å|Å{Å@Å{Å|Å|Å|Å{\n";
			else
				cout << "Å{Å|Å|Å|Å{Å|Å|Å|Å{Å|Å|Å|Å{\n";
	}
	cout << "\n";
#if 0
	//	ÉOÉçÅ[ÉoÉãÉ{Å[Éhï\é¶
	ix = 0;
	for(int y = 0; y != N_VERT/3; ++y) {
		for(int x = 0; x != N_HORZ/3; ++x) {
			switch( m_gboard[ix++] ) {
			case EMPTY:	cout << "ÅE"; break;
			case MARU:	cout << "Çn"; break;
			case BATSU:	cout << "Çw"; break;
			}
		}
		cout << "\n";
	}
	cout << "\n";
#endif
}
