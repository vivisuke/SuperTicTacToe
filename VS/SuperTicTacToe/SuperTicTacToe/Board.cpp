//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		22-1-2023
//			Author:			�Óc�L�G
//			Description:
//
//----------------------------------------------------------------------

#include <random>
#include <iostream>
#include "Board.h"

using namespace std;

std::random_device g_rnd;         // �񌈒�I�ȗ���������
std::mt19937 g_mt(g_rnd());       // �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h

Board::Board() {
	init();
}
void Board::init() {
	m_next_color = MARU;
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
const char* digstr[] = { "�P", "�Q", "�R", "�S", "�T", "�U", "�V", "�W", "�X", };
void Board::print() const {
	//	�S���[�J���{�[�h�\��
	int ix = 0, gx = 0;
	cout << "�@�@�������@�������@������\n";
	cout << "�@�{�|�|�|�{�|�|�|�{�|�|�|�{�@�{�|�|�|�{\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << digstr[y] << "�b";
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
				cout << "�@�{�|�|�|�{�|�|�|�{�|�|�|�{�@�{�|�|�|�{\n";
			else
				cout << "�@�{�|�|�|�{�|�|�|�{�|�|�|�{\n";
	}
	cout << "next_board = " << (int)m_next_board << "\n";
	if( !m_stack.empty() ) {
		const auto &item = m_stack.back();
		cout << "did put(" << (int)item.m_x << ", " << (int)item.m_y << ")\n";
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
//char Board::get_color(int x, int y) const {
//}
bool Board::is_linedup(int x, int y) const {
	int x3 = x % 3;		//	���[�J���{�[�h���ʒu
	int x0 = x - x3;	// ���[�J���{�[�h�����[���W
	int y3 = y % 3;		//	���[�J���{�[�h���ʒu
	int y0 = y - y3;	// ���[�J���{�[�h����[���W
	if( get_color(x0, y) == get_color(x0+1, y) && get_color(x0, y) == get_color(x0+2, y) )
		return true;			// �������ɎO�ڕ���
	if( get_color(x, y0) == get_color(x, y0+1) && get_color(x, y0) == get_color(x, y0+2) )
		return true;			// �c�����ɎO�ڕ���
	if( x3 == y3 )		// �_�΂ߕ����`�F�b�N
		if( get_color(x0, y0) == get_color(x0+1, y0+1) && get_color(x0, y0) == get_color(x0+2, y0+2) )
			return true;		// �_�΂ߕ����ɎO�ڕ���
	if( x3 == 2 - y3 )		// �^�΂ߕ����`�F�b�N
		if( get_color(x0, y0+2) == get_color(x0+1, y0+1) && get_color(x0, y0+2) == get_color(x0+2, y0) )
			return true;		// �^�΂ߕ����ɎO�ڕ���
	return false;
}
void Board::update_next_board(int x, int y) {
	int x3 = x % 3;
	int y3 = y % 3;
	m_next_board = x3 + y3 * 3;
	if( m_linedup[m_next_board] || m_nput[m_next_board] == 9 )	//	���łɎO�ڕ���ł��� or �󂫂�����
		m_next_board = -1;			// �S���[�J���{�[�h�ɒ���\
}
void Board::put(int x, int y, char col) {
	m_board[x + y*N_HORZ] = col;
	//int gx = x / 3;
	//int gy = y / 3;
	//int gix = gx + gy*3;		//	�O���[�o���{�[�h���C���f�b�N�X
	auto gix = xyToGix(x, y);
	m_nput[gix] += 1;		// �e���[�J���{�[�h�̒��萔
	bool linedup = false;	// ���[�J���{�[�h���ŎO�ڕ��񂾂��H
	if( !m_linedup[gix] && is_linedup(x, y) ) {
		linedup = true;		//	���̒���ŎO�ڕ���
		m_linedup[gix] = true;
		m_gboard[gix] = col;
	}
	update_next_board(x, y);
	m_stack.push_back(HistItem(x, y, col, linedup));
	m_next_color = (MARU + BATSU) - m_next_color;		//	��Ԍ��
}
void Board::undo_put() {
	const auto &item = m_stack.back();
	m_board[xyToIndex(item.m_x, item.m_y)] = EMPTY;
	auto gix = xyToGix(item.m_x, item.m_y);
	m_nput[gix] -= 1;		// �e���[�J���{�[�h�̒��萔
	if( item.m_linedup ) {	//	���̒���ŎO�ڕ���ł��ꍇ
		m_linedup[gix] = false;
		m_gboard[gix] = EMPTY;
	}
	m_stack.pop_back();
	m_next_color = (MARU + BATSU) - m_next_color;		//	��Ԍ��
}
Move Board::sel_move_random() {
	if( m_stack.empty() ) {		//	����������������̏ꍇ
		return Move(g_mt() % N_HORZ, g_mt() % N_VERT);
	}
	vector<Move> lst;
	if( m_next_board < 0 ) {	//	�S���[�J���{�[�h�ɒ���\�ȏꍇ
		for(int y = 0; y != N_VERT; ++y) {
			for(int x = 0; x != N_HORZ; ++x) {
				if( is_empty(x, y) )
					lst.push_back(Move(x, y));
			}
		}
	} else {		//	next_board ���[�J���{�[�h�ɂ̂ݒ���\�ȏꍇ
		//int gx = m_next_board % 3;
		//int gy = m_next_board / 3;
		int x0 = (m_next_board % 3) * 3;
		int y0 = (m_next_board / 3) * 3;
		for(int v = 0; v != 3; ++v) {
			for(int h = 0; h != 3; ++h) {
				if( is_empty(x0 + h, y0 + v) )
					lst.push_back(Move(x0 + h, y0 + v));
			}
		}
	}
	return lst[g_mt() % lst.size()];
}
