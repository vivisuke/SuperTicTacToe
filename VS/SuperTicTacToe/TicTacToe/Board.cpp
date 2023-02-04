//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		31-1-2023
//			Author:			�Óc�L�G
//			Description:
//
//----------------------------------------------------------------------

#include <iostream>
#include <random>
#include <vector>
#include "Board.h"

using namespace std;

std::random_device g_rnd;         // �񌈒�I�ȗ���������
//std::mt19937 g_mt(0);       // �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h
std::mt19937 g_mt(g_rnd());       // �����Z���k�E�c�C�X�^��32�r�b�g�ŁA�����͏����V�[�h

Board::Board() {
	init();
}
Board::Board(const Board& x)
	: m_game_over(x.m_game_over)
	, m_next_color(x.m_next_color)
	, m_winner(x.m_winner)
{
	for(int i = 0; i != BD_SIZE; ++i) {
		m_board[i] = x.m_board[i];
	}
	m_stack = x.m_stack;
}
void Board::init() {
	m_game_over = false;
	m_winner = EMPTY;
	m_next_color = WHITE;
	m_stack.clear();
	for(int i = 0; i != BD_SIZE; ++i) m_board[i] = EMPTY;
}
void Board::print() const {
	int ix = 0;
	cout << "  ������\n";
	cout << " +------+\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << (y+1) << "|";
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "�E"; break;
			case WHITE:	cout << "�n"; break;
			case BLACK:	cout << "�w"; break;
			}
		}
		cout << "|\n";
	}
	cout << " +------+\n";
	cout << "\n";
}
void Board::put(int x, int y, char col) {
	m_board[xyToIndex(x, y)] = col;
	//	�I�ǃ`�F�b�N
	if( m_board[xyToIndex(0, y)] + m_board[xyToIndex(1, y)] + m_board[xyToIndex(2, y)] == col * 3 ) {
		m_game_over = true;
		m_winner = col;
	} else if( m_board[xyToIndex(x, 0)] + m_board[xyToIndex(x, 1)] + m_board[xyToIndex(x, 2)] == col * 3 ) {
		m_game_over = true;
		m_winner = col;
	} else if( x == y && m_board[xyToIndex(0, 0)] + m_board[xyToIndex(1, 1)] + m_board[xyToIndex(2, 2)] == col * 3 ) {
		m_game_over = true;
		m_winner = col;
	} else if( x == 2 - y && m_board[xyToIndex(2, 0)] + m_board[xyToIndex(1, 1)] + m_board[xyToIndex(0, 2)] == col * 3 ) {
		m_game_over = true;
		m_winner = col;
	}
	//
	m_stack.push_back(HistItem(x, y, col));
	if( m_stack.size() == BD_SIZE )
		m_game_over = true;
	change_color();		//	��Ԍ��
}
void Board::undo_put() {
	const auto &item = m_stack.back();
	m_board[xyToIndex(item.m_x, item.m_y)] = EMPTY;
	m_stack.pop_back();
	change_color();		//	��Ԍ��
}
//	col �̎�ԂŁA�O�ڕ��ׂ邱�Ƃ��o���钅���T��
//	return: ����𔭌��ł������H
bool Board::find_make_three(Move& mv, char col) {
	for(int y = 0; y != N_VERT; ++y) {
		if( get_color(0, y) + get_color(1, y) + get_color(2, y) == col * 2 ) {
			if( get_color(0, y) == EMPTY )
				mv = Move(0, y);
			else if( get_color(1, y) == EMPTY )
				mv = Move(1, y);
			else
				mv = Move(2, y);
			return true;
		}
	}
	for(int x = 0; x != N_HORZ; ++x) {
		if( get_color(x, 0) + get_color(x, 1) + get_color(x, 2) == col * 2 ) {
			if( get_color(x, 0) == EMPTY )
				mv = Move(x, 0);
			else if( get_color(x, 1) == EMPTY )
				mv = Move(x, 1);
			else
				mv = Move(x, 2);
			return true;
		}
	}
	if( get_color(0, 0) + get_color(1, 1) + get_color(2, 2) == col * 2 ) {
		if( get_color(0, 0) == EMPTY )
			mv = Move(0, 0);
		else if( get_color(1, 1) == EMPTY )
			mv = Move(1, 1);
		else
			mv = Move(2, 2);
		return true;
	}
	if( get_color(2, 0) + get_color(1, 1) + get_color(0, 2) == col * 2 ) {
		if( get_color(2, 0) == EMPTY )
			mv = Move(2, 0);
		else if( get_color(1, 1) == EMPTY )
			mv = Move(1, 1);
		else
			mv = Move(0, 2);
		return true;
	}
	return false;
}
Move Board::sel_move_random() {
	vector<Move> lst;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			if( is_empty(x, y) )
				lst.push_back(Move(x, y));
		}
	}
	return lst[g_mt() % lst.size()];
}
int Board::playout_random(bool prioThree) {
	if( !is_game_over() ) {
		for(;;) {
			Move mv(-1, -1);
			if( prioThree ) {
				if( find_make_three(mv, next_color()) )	//	�O�ڍ���ꍇ
					return next_color();
				if( find_make_three(mv, op_color()) )		//	���肪�O�ڍ���ꍇ
				    put(mv, next_color());
			}
			if( mv.m_x < 0 )
			    mv = sel_move_random();
		    put(mv, next_color());
		    //print();
		    if( is_game_over() )
	            break;
		}
	}
	return m_winner;
}
int Board::playout_random(int N_PLAYOUT, bool prioThree) {
	if( is_game_over() )
		return N_PLAYOUT * m_winner;
	int sum = 0;
	for(int i = 0; i != N_PLAYOUT; ++i) {
		Board bd(*this);
		sum += bd.playout_random(prioThree);
		//bd.print();
	}
	return sum;
}
//	���������e�J�����@�ɂ�钅�茈��
Move Board::sel_move_PMC() {
	const int N_PLAYOUT = 1000;
	int mx = -N_PLAYOUT;
	Move mv;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			if( is_empty(x, y) ) {
				Board bd(*this);
				//Board bd;
				bd.put(x, y, m_next_color);
				auto r = bd.playout_random(N_PLAYOUT);
				//cout << (double)r/N_PLAYOUT << "\t";	//	for Debug
				r *= m_next_color;		//	�O��FWHITE:1, BLACK:-1
				if( r > mx ) {
					mx = r;
					mv = Move(x, y);
				}
			} else {
				//cout << ".\t";			//	for Debug
			}
		}
		//cout << "\n";
	}
	return mv;
}
//	�O�ڕ��Ԏ��D�悷�郂���e�J�����@�ɂ�钅�茈��
Move Board::sel_move_MC(bool prioThree) {
	Move mv;
	if( prioThree ) {
		if( find_make_three(mv, next_color()) )		//	�O�ڕ��ׂ邱�Ƃ��o����
			return mv;
		if( find_make_three(mv, op_color()) )		//	���肪�O�ڕ��ׂ邱�Ƃ��o����
			return mv;
	}
	const int N_PLAYOUT = 1000;
	int mx = -N_PLAYOUT;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			if( is_empty(x, y) ) {
				Board bd(*this);
				//Board bd;
				bd.put(x, y, m_next_color);
				auto r = bd.playout_random(N_PLAYOUT, prioThree);
				//cout << (double)r/N_PLAYOUT << "\t";	//	for Debug
				r *= m_next_color;		//	�O��FWHITE:1, BLACK:-1
				if( r > mx ) {
					mx = r;
					mv = Move(x, y);
				}
			} else {
				//cout << ".\t";			//	for Debug
			}
		}
		//cout << "\n";
	}
	return mv;
}
int Board::Q_table_inedx() const {
	int qix = 0;
	for(int i = 0; i != BD_SIZE; ++i) {
		qix *= 3;
		switch( m_board[i] ) {
		case WHITE: qix += 1;	break;
		case BLACK: qix += 2;	break;
		}
	}
	return qix;
}
