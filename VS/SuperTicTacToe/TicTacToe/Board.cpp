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

//typedef int[8] Int8;
struct RTable {
	RTable() {
		for(int i = 0; i != BD_SIZE; ++i) m_rv[i] = 0;
	}
	int	m_rv[BD_SIZE];		//	����l�e�[�u��
};

unordered_map<int, float> g_vtable;		//	�ǖʃn�b�V���L�[���ǖʕ]���l
unordered_map<int, RTable> g_rtable;	//	�ǖʃn�b�V���L�[������l�e�[�u��

const float GAMMA = 0.95;

float gen_all_position(Board &bd) {
	auto hv = bd.hash();
	if( g_vtable.find(hv) == g_vtable.end() ) {		//	���o�^�̏ꍇ
		if( bd.is_game_over() ) {
			return g_vtable[hv] = bd.winner() * 1;
		} else {
			float v = bd.next_color() * -2.0;
			for(int y = 0; y != N_VERT; ++y) {
				for(int x = 0; x != N_HORZ; ++x) {
					if( bd.is_empty(x, y) ) {
						bd.put(x, y, bd.next_color());
						auto r = gen_all_position(bd) * GAMMA;
						bd.undo_put();
						v = bd.next_color() == WHITE ? std::max(v, r) : std::min(v, r);
					}
				}
			}
			return g_vtable[hv] = v;
		}
	} else
		return g_vtable[hv];
}
void init_vtable() {
	Board bd;
	gen_all_position(bd);
	cout << "g_vtable.size() = " << g_vtable.size() << "\n";
}
void print_next(Board &bd) {
	bd.print();
	cout << "v = " << g_vtable[bd.hash()] << "\n\n";
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			if( bd.is_empty(x, y) ) {
				bd.put(x, y, bd.next_color());
				bd.print();
				cout << "v = " << g_vtable[bd.hash()] << "\n\n";
				bd.undo_put();
			}
		}
	}
}

//--------------------------------------------------------------------------------
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
	m_game_over = false;	//	�Q�[���I�[�o�[���璅�肷�邱�Ƃ͖���
	m_winner = EMPTY;
	change_color();			//	��Ԍ��
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
int Board::hash() const {
	int v1 = 0, v2 = 0;
	int v3 = 0, v4 = 0;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			v1 *= 3;
			switch( get_color(x, y) ) {
			case WHITE:	v1 += 1; break;
			case BLACK:	v1 += 2; break;
			}
			v2 *= 3;
			switch( get_color(2-x, y) ) {
			case WHITE:	v2 += 1; break;
			case BLACK:	v2 += 2; break;
			}
			v3 *= 3;
			switch( get_color(x, 2-y) ) {
			case WHITE:	v3 += 1; break;
			case BLACK:	v3 += 2; break;
			}
			v4 *= 3;
			switch( get_color(2-x, 2-y) ) {
			case WHITE:	v4 += 1; break;
			case BLACK:	v4 += 2; break;
			}
		}
	}
	int v5 = 0, v6 = 0;
	int v7 = 0, v8 = 0;
	for(int x = 0; x != N_HORZ; ++x) {
		for(int y = 0; y != N_VERT; ++y) {
			v5 *= 3;
			switch( get_color(x, y) ) {
			case WHITE:	v5 += 1; break;
			case BLACK:	v5 += 2; break;
			}
			v6 *= 3;
			switch( get_color(2-x, y) ) {
			case WHITE:	v6 += 1; break;
			case BLACK:	v6 += 2; break;
			}
			v7 *= 3;
			switch( get_color(x, 2-y) ) {
			case WHITE:	v7 += 1; break;
			case BLACK:	v7 += 2; break;
			}
			v8 *= 3;
			switch( get_color(2-x, 2-y) ) {
			case WHITE:	v8 += 1; break;
			case BLACK:	v8 += 2; break;
			}
		}
	}
	//return v1;
	return std::min(std::min(std::min(v1, v2), std::min(v3, v4)),
					std::min(std::min(v5, v6), std::min(v7, v8)));
#if 0
	int hv = 0;
	for(int i = 0; i != BD_SIZE; ++i) {
		hv *= 3;
		switch( m_board[i] ) {
		case WHITE:	hv += 1; break;
		case BLACK:	hv += 2; break;
		}
	}
	return hv;
#endif
}
int Board::hash_asym() const {
	int v1 = 0;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			v1 *= 3;
			switch( get_color(x, y) ) {
			case WHITE:	v1 += 1; break;
			case BLACK:	v1 += 2; break;
			}
		}
	}
	return v1;
}
//	���S�ǂ݌��ʂ��Q�Ƃ���AI
Move Board::sel_move_perfect() {
	if( g_vtable.empty() ) init_vtable();
	Move mv;
	float v = next_color() * -2.0;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x) {
			if( is_empty(x, y) ) {
				put(x, y, next_color());
				auto r = g_vtable[hash()] * GAMMA;
				undo_put();
				if( next_color() == WHITE ) {
					if( r > v ) {
						v = r;
						mv = Move(x, y);
					}
				} else {
					if( r < v ) {
						v = r;
						mv = Move(x, y);
					}
				}
			}
		}
	}
	return mv;
}
Move Board::sel_move_CFR() {
	Move mv;
	return mv;
}
