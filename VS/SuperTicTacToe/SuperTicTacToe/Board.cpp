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
	m_next_board = -1;
	m_stack.clear();
	for(int i = 0; i != BD_SIZE; ++i)
		m_board[i] = EMPTY;
	for(int i = 0; i != GBD_SIZE; ++i) {
		m_gboard[i] = EMPTY;
		m_nput[i] = 0;
		m_linedup[i] = false;
	}
}
const char* digstr[] = { "ÇP", "ÇQ", "ÇR", "ÇS", "ÇT", "ÇU", "ÇV", "ÇW", "ÇX", };
void Board::print() const {
	//	ëSÉçÅ[ÉJÉãÉ{Å[Éhï\é¶
	int ix = 0, gx = 0;
	cout << "Å@Å@ÇÅÇÇÇÉÅ@ÇÑÇÖÇÜÅ@ÇáÇàÇâ\n";
	cout << "Å@Å{Å|Å|Å|Å{Å|Å|Å|Å{Å|Å|Å|Å{Å@Å{Å|Å|Å|Å{\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << digstr[y] << "Åb";
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
				cout << "Å@Å{Å|Å|Å|Å{Å|Å|Å|Å{Å|Å|Å|Å{Å@Å{Å|Å|Å|Å{\n";
			else
				cout << "Å@Å{Å|Å|Å|Å{Å|Å|Å|Å{Å|Å|Å|Å{\n";
	}
	cout << "next_board = " << (int)m_next_board << "\n";
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
//char Board::get_color(int x, int y) const {
//}
bool Board::is_linedup(int x, int y) const {
	int x3 = x % 3;		//	ÉçÅ[ÉJÉãÉ{Å[Éhì‡à íu
	int x0 = x - x3;	// ÉçÅ[ÉJÉãÉ{Å[Éhì‡ç∂í[ç¿ïW
	int y3 = y % 3;		//	ÉçÅ[ÉJÉãÉ{Å[Éhì‡à íu
	int y0 = y - y3;	// ÉçÅ[ÉJÉãÉ{Å[Éhì‡è„í[ç¿ïW
	if( get_color(x0, y) == get_color(x0+1, y) && get_color(x0, y) == get_color(x0+2, y) )
		return true;			// â°ï˚å¸Ç…éOñ⁄ï¿ÇÒÇæ
	if( get_color(x, y0) == get_color(x, y0+1) && get_color(x, y0) == get_color(x, y0+2) )
		return true;			// ècï˚å¸Ç…éOñ⁄ï¿ÇÒÇæ
	if( x3 == y3 )		// Å_éŒÇﬂï˚å¸É`ÉFÉbÉN
		if( get_color(x0, y0) == get_color(x0+1, y0+1) && get_color(x0, y0) == get_color(x0+2, y0+2) )
			return true;		// Å_éŒÇﬂï˚å¸Ç…éOñ⁄ï¿ÇÒÇæ
	if( x3 == 2 - y3 )		// Å^éŒÇﬂï˚å¸É`ÉFÉbÉN
		if( get_color(x0, y0+2) == get_color(x0+1, y0+1) && get_color(x0, y0+2) == get_color(x0+2, y0) )
			return true;		// Å^éŒÇﬂï˚å¸Ç…éOñ⁄ï¿ÇÒÇæ
	return false;
}
void Board::update_next_board(int x, int y) {
	int x3 = x % 3;
	int y3 = y % 3;
	m_next_board = x3 + y3 * 3;
	if( m_linedup[m_next_board] || m_nput[m_next_board] == 9 )	//	Ç∑Ç≈Ç…éOñ⁄ï¿ÇÒÇ≈Ç¢ÇÈ or ãÛÇ´Ç™ñ≥Ç¢
		m_next_board = -1;			// ëSÉçÅ[ÉJÉãÉ{Å[ÉhÇ…íÖéËâ¬î\
}
void Board::put(int x, int y, char col) {
	m_board[x + y*N_HORZ] = col;
	//int gx = x / 3;
	//int gy = y / 3;
	//int gix = gx + gy*3;		//	ÉOÉçÅ[ÉoÉãÉ{Å[Éhì‡ÉCÉìÉfÉbÉNÉX
	auto gix = xyToGix(x, y);
	m_nput[gix] += 1;		// äeÉçÅ[ÉJÉãÉ{Å[ÉhÇÃíÖéËêî
	bool linedup = false;	// ÉçÅ[ÉJÉãÉ{Å[Éhì‡Ç≈éOñ⁄ï¿ÇÒÇæÇ©ÅH
	if( !m_linedup[gix] && is_linedup(x, y) ) {
		linedup = true;		//	Ç±ÇÃíÖéËÇ≈éOñ⁄ï¿ÇÒÇæ
		m_linedup[gix] = true;
		m_gboard[gix] = col;
	}
	update_next_board(x, y);
	m_stack.push_back(HistItem(x, y, col, linedup));
}
void Board::undo_put() {
	const auto &item = m_stack.back();
	m_board[xyToIndex(item.m_x, item.m_y)] = EMPTY;
	auto gix = xyToGix(item.m_x, item.m_y);
	m_nput[gix] -= 1;		// äeÉçÅ[ÉJÉãÉ{Å[ÉhÇÃíÖéËêî
	if( item.m_linedup ) {	//	Ç±ÇÃíÖéËÇ≈éOñ⁄ï¿ÇÒÇ≈ÇΩèÍçá
		m_linedup[gix] = false;
		m_gboard[gix] = EMPTY;
	}
	m_stack.pop_back();
}
