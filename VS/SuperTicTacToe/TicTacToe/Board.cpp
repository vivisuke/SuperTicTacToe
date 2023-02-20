//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		31-1-2023
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <numeric>
#include "Board.h"

const double GAMMA = 0.99;		//	評価値割引率
const double BETA = 0.95;		//	Q値積和時割引率

using namespace std;

std::random_device g_rnd;         // 非決定的な乱数生成器
//std::mt19937 g_mt(0);       // メルセンヌ・ツイスタの32ビット版、引数は初期シード
std::mt19937 g_mt(g_rnd());       // メルセンヌ・ツイスタの32ビット版、引数は初期シード

int g_count = 0;			//	for Debug
Board g_bd;				//	for Debug

//typedef int[8] Int8;
struct RTable {
	RTable() {
		for(int i = 0; i != BD_SIZE; ++i) m_rv[i] = 0;
	}
	int	m_rv[BD_SIZE];		//	後悔値テーブル
};
struct QTable {
	QTable() {
		for(int i = 0; i != BD_SIZE; ++i) m_q[i] = 0.0;
	}
	double	m_q[BD_SIZE];		//	Ｑ値テーブル
};

unordered_map<int, double> g_vtable;		//	局面ハッシュキー→局面評価値
unordered_map<int, RTable> g_rtable;	//	局面ハッシュキー→後悔値テーブル
unordered_map<int, QTable> g_qtable;	//	局面ハッシュキー→Ｑ値テーブル

//const double GAMMA = 0.95;

double gen_all_position(Board &bd) {
	auto hv = bd.hash();
	if( g_vtable.find(hv) == g_vtable.end() ) {		//	未登録の場合
		if( bd.is_game_over() ) {
			return g_vtable[hv] = bd.winner() * 1;
		} else {
			double v = bd.next_color() * -DBL_MAX;
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
	cout << "g_vtable.size() = " << g_vtable.size() << "\n\n";
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
string dig_str(int x, int nc) {
	auto txt = to_string(x);
	if (txt.size() < nc)
		txt = string(nc - txt.size(), ' ') + txt;
	return txt;
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
bool Board::operator==(const Board& x) const {
	//if( m_stack.size() != x.m_stack.size() )
	//	return false;
	for(int i = 0; i != BD_SIZE; ++i)
		if( m_board[i] != x.m_board[i] )
			return false;
	return true;
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
	cout << "  ａｂｃ\n";
	cout << " +------+\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << (y+1) << "|";
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "・"; break;
			case WHITE:	cout << "Ｏ"; break;
			case BLACK:	cout << "Ｘ"; break;
			}
		}
		cout << "|\n";
	}
	cout << " +------+\n";
	cout << "\n";
}
void Board::print_rtable() const {
	const auto& elm = g_rtable[hash_asym()];
	int ix = 0;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x, ++ix) {
			if( is_empty(x, y) ) {
				if( elm.m_rv[ix] > 0 )
					cout << dig_str(elm.m_rv[ix], 2) << " ";
				else
					cout << "-- ";
			} else {
				switch( m_board[ix] ) {
				//case EMPTY:	cout << "・\t"; break;
				case WHITE:	cout << "Ｏ "; break;
				case BLACK:	cout << "Ｘ "; break;
				}
			}
		}
		cout << "\n";
	}
	cout << "\n";
}
void Board::print_qtable() const {
	const auto& elm = g_qtable[hash_asym()];
	int ix = 0;
	for(int y = 0; y != N_VERT; ++y) {
		for(int x = 0; x != N_HORZ; ++x, ++ix) {
			if( is_empty(x, y) ) {
				//if( elm.m_q[ix] != 0.0 )
					cout << dig_str((int)(elm.m_q[ix]*100), 3) << " ";
				//else
				//	cout << "-- ";
			} else {
				switch( m_board[ix] ) {
				//case EMPTY:	cout << "・\t"; break;
				case WHITE:	cout << "Ｏ  "; break;
				case BLACK:	cout << "Ｘ  "; break;
				}
			}
		}
		cout << "\n";
	}
	cout << "\n";
}
int eval(char c1, char c2, char c3) {
	const int LINED3 = 100;				//	3目並んだ
	const int LINED2 = 8;				//	2目並んだ
	const int LINED1 = 1;				//	1目のみ
	int sum = c1 + c2 + c3;
	if( sum == WHITE * 3 ) return LINED3;
	if( sum == BLACK * 3 ) return -LINED3;
	if( sum == WHITE * 2 ) return LINED2;
	if( sum == BLACK * 2 ) return -LINED2;
	int n = c1*c1 + c2*c2 + c3*c3;		//	置かれた石数
	if( n == 1 ) {
		if( sum == WHITE ) return LINED1;
		if( sum == BLACK ) return -LINED1;
	}
	return 0;
}
int Board::eval() const {
	int ev = 0;
	for(int y = 0; y != N_VERT; ++y)
		ev += ::eval(get_color(0, y), get_color(1, y), get_color(2, y));
	for(int x = 0; x != N_VERT; ++x)
		ev += ::eval(get_color(x, 0), get_color(x, 1), get_color(x, 2));
	ev += ::eval(get_color(0, 0), get_color(1, 1), get_color(2, 2));
	ev += ::eval(get_color(2, 0), get_color(1, 1), get_color(0, 2));
	return ev;
}
void Board::put(int x, int y, char col) {
	m_board[xyToIndex(x, y)] = col;
	//	終局チェック
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
	change_color();		//	手番交代
}
void Board::undo_put() {
	const auto &item = m_stack.back();
	m_board[xyToIndex(item.m_x, item.m_y)] = EMPTY;
	m_stack.pop_back();
	m_game_over = false;	//	ゲームオーバーから着手することは無い
	m_winner = EMPTY;
	change_color();			//	手番交代
}
//	col の手番で、三目並べることが出来る着手を探す
//	return: 着手を発見できたか？
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
				if( find_make_three(mv, next_color()) )	//	三目作れる場合
					return next_color();
				if( find_make_three(mv, op_color()) )		//	相手が三目作れる場合
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
//	純粋モンテカルロ法による着手決定
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
				r *= m_next_color;		//	前提：WHITE:1, BLACK:-1
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
//	三目並ぶ手を優先するモンテカルロ法による着手決定
Move Board::sel_move_MC(bool prioThree) {
	Move mv;
	if( prioThree ) {
		if( find_make_three(mv, next_color()) )		//	三目並べることが出来る
			return mv;
		if( find_make_three(mv, op_color()) )		//	相手が三目並べることが出来る
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
				r *= m_next_color;		//	前提：WHITE:1, BLACK:-1
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
//	完全読み結果を参照するAI
Move Board::sel_move_perfect() {
	if( g_vtable.empty() ) init_vtable();
	Move mv;
	double v = next_color() * -DBL_MAX;
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
//	状態 bd からQ値がmax/minの行動を選択し、各行動のQ値を更新
//	Q(s) = βQ(s) + MinMaxQ(s')
//		※ 正規化？されたQ値は Q(s)*(1-β)、β：[0, 1)
double MinMaxQ(Board& bd, bool verbose) {
	if( bd.is_game_over() ) {
		//bd.print();
		return bd.winner();
	}
	auto key = bd.hash_asym();
	if( g_qtable.find(key) == g_qtable.end() )
		g_qtable[key] = QTable();
	auto& elm = g_qtable[key];
	auto sum = std::accumulate(begin(elm.m_q), end(elm.m_q), 0);
	int rix = 0;
	if( sum == 0 ) {		//	Ｑ値が全て0の場合：
		vector<int> lst;
		for(int i = 0; i != BD_SIZE; ++i) {
			if( bd.is_empty(i) )
				lst.push_back(i);
		}
		rix = lst[g_mt() % lst.size()];		//	着手箇所をランダムに選択
	} else {
		//	最大/最小Ｑ値のアクションを選択
		double m = bd.next_color() == WHITE ? -DBL_MAX : DBL_MAX;
		for(int i = 0; i != BD_SIZE; ++i) {
			if( bd.is_empty(i) ) {
				if( bd.next_color() == WHITE && elm.m_q[i] > m ||
					bd.next_color() != WHITE && elm.m_q[i] < m )
				{
					m = elm.m_q[i];
					rix = i;
				}
			}
		}
	}
	bd.put(rix, bd.next_color());
	auto r = MinMaxQ(bd, verbose) * GAMMA;
	bd.undo_put();
	return r;
}
void learn_MinMaxQ(Board& bd, bool verbose) {
	auto key = bd.hash_asym();
	if( g_qtable.find(key) == g_qtable.end() )
		g_qtable[key] = QTable();
	auto& elm = g_qtable[key];
	for(int i = 0; i != BD_SIZE; ++i) {
		if( bd.is_empty(i) ) {
			bd.put(i, bd.next_color());
			auto r = MinMaxQ(bd, verbose) * GAMMA;
			bd.undo_put();
			elm.m_q[i] = elm.m_q[i] * BETA + r;
		}
	}
}
void learn_MinMaxQ_untilEnd(const Board& bd0, bool verbose) {
	Board bd(bd0);
	while( !bd.is_game_over() ) {
		learn_MinMaxQ(bd, verbose);
		if( verbose ) bd.print_qtable();
		auto key = bd.hash_asym();
		auto& elm = g_qtable[key];
		double m = bd.next_color() == WHITE ? -DBL_MAX : DBL_MAX;
		int ix = -1;
		for(int i = 0; i != BD_SIZE; ++i) {
			if( bd.is_empty(i) ) {
				if( bd.next_color() == WHITE && elm.m_q[i] > m ||
					bd.next_color() == BLACK && elm.m_q[i] < m )
				{
					m = elm.m_q[i];
					ix = i;
				}
			}
		}
		bd.put(ix, bd.next_color());
	}
}

double learn_QPlus(Board& bd, bool learning, bool verbose) {
	if( bd.is_game_over() ) {
		//bd.print();
		return bd.winner();
	}
	auto& elm = g_qtable[bd.hash_asym()];
	auto sum = std::accumulate(begin(elm.m_q), end(elm.m_q), 0);
	int rix = 0;
	if( sum == 0 ) {		//	Ｑ値が全て0の場合：
		vector<int> lst;
		for(int i = 0; i != BD_SIZE; ++i) {
			if( bd.is_empty(i) )
				lst.push_back(i);
		}
		rix = lst[g_mt() % lst.size()];		//	着手箇所をランダムに選択
	} else {
		//	最大Ｑ値のアクションを選択
		double m = -DBL_MAX;
		for(int i = 0; i != BD_SIZE; ++i)
			if( elm.m_q[i] > m ) {
				m = elm.m_q[i];
				rix = i;
			}
	}
	bd.put(rix, bd.next_color());
	auto r = learn_QPlus(bd, learning, verbose);
	bd.undo_put();
	if( learning ) {	//	学習フラグON → 後悔値テーブル更新
		for(int rx = 0; rx != BD_SIZE; ++rx) {
			if( rx != rix && bd.is_empty(rx) ) {	//	反事実箇所に着手可能な場合
				bd.put(rx, bd.next_color());
				auto r2 = learn_QPlus(bd, false, verbose);
				bd.undo_put();
				if( bd.next_color() == WHITE )
					elm.m_q[rx] = std::max(0.0, elm.m_q[rx] + (r2 - r));
				else
					elm.m_q[rx] = std::max(0.0, elm.m_q[rx] + (r - r2));
			}
		}
		if( verbose )
			bd.print_qtable();
	}
	return r;
}
int learn_CFR(Board& bd, bool learning, bool verbose) {
	if( bd.is_game_over() ) {
		//bd.print();
		return bd.winner();
	}
	//if(learning && g_count >= 1600 && bd == g_bd ) {
	//	g_bd.print_rtable();
	//}
	auto& elm = g_rtable[bd.hash_asym()];
	auto sum = std::accumulate(begin(elm.m_rv), end(elm.m_rv), 0);
	int rix = 0;
	if( sum == 0 ) {		//	後悔値テーブルが全て0の場合：
		vector<int> lst;
		for(int i = 0; i != BD_SIZE; ++i) {
			if( bd.is_empty(i) )
				lst.push_back(i);
		}
		rix = lst[g_mt() % lst.size()];		//	着手箇所をランダムに選択
	} else {		//	後悔値の大きい箇所に高確率で着手
		int t = g_mt() % sum + 1;		//	[1, sum]
		for(;;) {
			if( (t -= elm.m_rv[rix]) <= 0 )
				break;
			++rix;
		}
	}
	bd.put(rix, bd.next_color());
	auto r = learn_CFR(bd, learning, verbose);
	bd.undo_put();
	if( learning ) {	//	学習フラグON → 後悔値テーブル更新
		//if (g_count >= 1600 && bd == g_bd) {
		//	g_bd.print_rtable();
		//}
		//bd.print();
		for(int rx = 0; rx != BD_SIZE; ++rx) {
			if( rx != rix && bd.is_empty(rx) ) {	//	反事実箇所に着手可能な場合
				bd.put(rx, bd.next_color());
				auto r2 = learn_CFR(bd, false, verbose);
				bd.undo_put();
				if( bd.next_color() == WHITE )
					elm.m_rv[rx] = std::max(0, elm.m_rv[rx] + (r2 - r));
				else
					elm.m_rv[rx] = std::max(0, elm.m_rv[rx] + (r - r2));
			}
		}
		if( verbose )
			bd.print_rtable();
		//for (int i = 0; i != BD_SIZE; ++i) cout << elm.m_rv[i] << " ";
		//cout << "\n";
		//if (g_count >= 1600 && bd == g_bd) {
		//	g_bd.print_rtable();
		//}
	}
	return r;
}
void learn_CFR() {
	g_rtable.clear();
	Board bd;
	//Board bd2;
	//bd2.put(0, 0, WHITE);
	g_bd.init();
	bd.put(0, 0, WHITE);
	bd.put(1, 0, BLACK);
	bd.put(2, 2, WHITE);
	bd.put(1, 2, BLACK);
	bd.put(0, 1, WHITE);
	bd.put(2, 1, BLACK);
	g_count = 0;
	for(int i = 0; i != 100; ++i, ++g_count) {
		learn_CFR(bd, true, false);
		if( (i + 1) % 10 == 0 || i < 10 ) {
			cout << (i+1) << ":\n";
			bd.print_rtable();
		}
	}
	//learn_CFR(bd, true, true);
	//learn_CFR(bd, true, true);
	//learn_CFR(bd, true, true);
	//learn_CFR(bd, true, true);
	//learn_CFR(bd, true, true);
}
Move Board::sel_move_CFR() {
	//auto hv = hash_asym();
	auto& ri = g_rtable[hash_asym()];
	auto sum = std::accumulate(begin(ri.m_rv), end(ri.m_rv), 0);
	if( sum == 0 ) {

	}
	Move mv;
	return mv;
}
void learn_QPlus() {
}
Move Board::sel_move_MinMax(int depth) {
	Move mv;
	int mm = next_color() == WHITE ? -INT_MAX : INT_MAX;
	for(int ix = 0; ix != BD_SIZE; ++ix) {
		if( is_empty(ix) ) {
			put(ix, next_color());
			auto ev = eval();
			undo_put();
			if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
				mm = ev;
				mv.m_x = ix % 3;
				mv.m_y = ix / 3;
			}
		}
	}
	return mv;
}
