//----------------------------------------------------------------------
//
//			File:			"Board.cpp"
//			Created:		22-1-2023
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#include <random>
#include <iostream>
#include "Board.h"

using namespace std;

std::random_device g_rnd;         // 非決定的な乱数生成器
//std::mt19937 g_mt(2);       // メルセンヌ・ツイスタの32ビット版、引数は初期シード
std::mt19937 g_mt(g_rnd());       // メルセンヌ・ツイスタの32ビット版、引数は初期シード

vector<int> g_eval;				//	盤面インデックス → 評価値テーブル

//----------------------------------------------------------------------
Board::Board() {
	init();
}
Board::Board(const Board& x)
	: m_game_over(x.m_game_over)
	, m_next_color(x.m_next_color)
	, m_next_board(x.m_next_board)
{
	for(int i = 0; i != BD_SIZE; ++i) {
		m_board[i] = x.m_board[i];
	}
	for(int i = 0; i != GBD_SIZE; ++i) {
		m_gboard[i] = x.m_gboard[i];
		m_nput[i] = x.m_nput[i];
		m_linedup[i] = x.m_linedup[i];
	}
	m_stack = x.m_stack;
}
void Board::init() {
	m_game_over = false;
	m_next_color = WHITE;
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
const char* digstr[] = { "１", "２", "３", "４", "５", "６", "７", "８", "９", };
void Board::print() const {
	//	全ローカルボード表示
	int ix = 0, gx = 0;
	cout << "　　ａｂｃ　ｄｅｆ　ｇｈｉ\n";
	cout << "　＋－－－＋－－－＋－－－＋　＋－－－＋\n";
	for(int y = 0; y != N_VERT; ++y) {
		cout << digstr[y] << "｜";
		for(int x = 0; x != N_HORZ; ++x) {
			switch( m_board[ix++] ) {
			case EMPTY:	cout << "・"; break;
			case WHITE:	cout << "Ｏ"; break;
			case BLACK:	cout << "Ｘ"; break;
			}
			if( x%3 == 2 ) cout << "｜";
		}
		if( y < N_VERT/3 ) {
			cout << "　｜";
			for(int x = 0; x != N_HORZ/3; ++x) {
				switch( m_gboard[gx++] ) {
				case EMPTY:	cout << "・"; break;
				case WHITE:	cout << "Ｏ"; break;
				case BLACK:	cout << "Ｘ"; break;
				}
			}
			cout << "｜";
		}
		cout << "\n";
		if( y%3 == 2 )
			if( y == 2 )
				cout << "　＋－－－＋－－－＋－－－＋　＋－－－＋\n";
			else
				cout << "　＋－－－＋－－－＋－－－＋\n";
	}
	cout << "next_board = " << (int)m_next_board << "\n";
	if( !m_stack.empty() ) {
		const auto &item = m_stack.back();
		cout << "did put(" << (int)item.m_x << ", " << (int)item.m_y << ")\n";
	}
	cout << "\n";
#if 0
	//	グローバルボード表示
	ix = 0;
	for(int y = 0; y != N_VERT/3; ++y) {
		for(int x = 0; x != N_HORZ/3; ++x) {
			switch( m_gboard[ix++] ) {
			case EMPTY:	cout << "・"; break;
			case WHITE:	cout << "Ｏ"; break;
			case BLACK:	cout << "Ｘ"; break;
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
	int x3 = x % 3;		//	ローカルボード内位置
	int x0 = x - x3;	// ローカルボード内左端座標
	int y3 = y % 3;		//	ローカルボード内位置
	int y0 = y - y3;	// ローカルボード内上端座標
	if( get_color(x0, y) == get_color(x0+1, y) && get_color(x0, y) == get_color(x0+2, y) )
		return true;			// 横方向に三目並んだ
	if( get_color(x, y0) == get_color(x, y0+1) && get_color(x, y0) == get_color(x, y0+2) )
		return true;			// 縦方向に三目並んだ
	if( x3 == y3 )		// ＼斜め方向チェック
		if( get_color(x0, y0) == get_color(x0+1, y0+1) && get_color(x0, y0) == get_color(x0+2, y0+2) )
			return true;		// ＼斜め方向に三目並んだ
	if( x3 == 2 - y3 )		// ／斜め方向チェック
		if( get_color(x0, y0+2) == get_color(x0+1, y0+1) && get_color(x0, y0+2) == get_color(x0+2, y0) )
			return true;		// ／斜め方向に三目並んだ
	return false;
}
bool Board::is_game_over(int x, int y) const {		//	終局（空欄無し or 三目並んだatグローバルボード）か？
	//if( m_stack.size() == BD_SIZE ) return true;
	int gx = x / 3;
	int gy = y / 3;
	if( get_gcolor(gx, gy) == EMPTY ) return false;		//	念のため、ほんとは必要ないはず
	if( get_gcolor(0, gy) == get_gcolor(1, gy) && get_gcolor(0, gy) == get_gcolor(+2, gy) )
		return true;			// 横方向に三目並んだ
	if( get_gcolor(gx, 0) == get_gcolor(gx, 1) && get_gcolor(gx, 0) == get_gcolor(gx, 2) )
		return true;			// 縦方向に三目並んだ
	if( gx == gy )		// ＼斜め方向チェック
		if( get_gcolor(0, 0) == get_gcolor(1, 1) && get_gcolor(0, 0) == get_gcolor(2, 2) )
			return true;		// ＼斜め方向に三目並んだ
	if( gx == 2 - gy )		// ／斜め方向チェック
		if( get_gcolor(0, 2) == get_gcolor(1, 1) && get_gcolor(0, 2) == get_gcolor(2, 0) )
			return true;		// ／斜め方向に三目並んだ
	return false;
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
	for(int gy = 0; gy != N_VERT3; ++gy) {
		for(int gx = 0; gx != N_HORZ3; ++gx) {
			if( !m_linedup[gy*N_HORZ3 + gx] ) {	//	ローカルボードで三目並んでいない
				int x0 = gx * N_HORZ3;
				int y0 = gy * N_VERT3;
				for(int i = 0; i != N_VERT3; ++i) {
					ev += ::eval(get_color(x0, y0+i), get_color(x0+1, y0+i), get_color(x0+2, y0+i));
					ev += ::eval(get_color(x0+i, y0), get_color(x0+i, y0+1), get_color(x0+i, y0+2));
				}
				ev += ::eval(get_color(x0, y0), get_color(x0+1, y0+1), get_color(x0+2, y0+2));
				ev += ::eval(get_color(x0+2, y0), get_color(x0+1, y0+1), get_color(x0, y0+2));
			}
		}
	}
	const int GVAL = 100;
	for(int i = 0; i != N_VERT3; ++i) {
		ev += ::eval(get_gcolor(0, i), get_gcolor(1, i), get_gcolor(2, i)) * GVAL;
		ev += ::eval(get_gcolor(i, 0), get_gcolor(i, 1), get_gcolor(i, 2)) * GVAL;
	}
	ev += ::eval(get_gcolor(0, 0), get_gcolor(1, 1), get_gcolor(2, 2)) * GVAL;
	ev += ::eval(get_gcolor(2, 0), get_gcolor(1, 1), get_gcolor(0, 2)) * GVAL;
	return ev;
}
void Board::update_next_board(int x, int y) {
	int x3 = x % 3;
	int y3 = y % 3;
	m_next_board = x3 + y3 * 3;
	if( m_linedup[m_next_board] || m_nput[m_next_board] == 9 )	//	すでに三目並んでいる or 空きが無い
		m_next_board = -1;			// 全ローカルボードに着手可能
}
void Board::put(int x, int y, char col) {
	m_board[x + y*N_HORZ] = col;
	//int gx = x / 3;
	//int gy = y / 3;
	//int gix = gx + gy*3;		//	グローバルボード内インデックス
	auto gix = xyToGix(x, y);
	m_nput[gix] += 1;		// 各ローカルボードの着手数
	bool linedup = false;	// ローカルボード内で三目並んだか？
	if( !m_linedup[gix] && is_linedup(x, y) ) {
		linedup = true;		//	この着手で三目並んだ
		m_linedup[gix] = true;
		m_gboard[gix] = col;
	}
	update_next_board(x, y);
	m_stack.push_back(HistItem(x, y, col, linedup));
	if (m_stack.size() == BD_SIZE ) {		//	空欄無し
		m_game_over = true;
		m_winner = EMPTY;
	} else if( linedup && is_game_over(x, y) ) {		//	終局チェック
		//is_game_over(x, y);
		m_game_over = true;
		m_winner = m_next_color;
	}
	m_next_color = (WHITE + BLACK) - m_next_color;		//	手番交代
}
void Board::undo_put() {
	const auto &item = m_stack.back();
	m_board[xyToIndex(item.m_x, item.m_y)] = EMPTY;
	auto gix = xyToGix(item.m_x, item.m_y);
	m_nput[gix] -= 1;		// 各ローカルボードの着手数
	if( item.m_linedup ) {	//	この着手で三目並んでた場合
		m_linedup[gix] = false;
		m_gboard[gix] = EMPTY;
	}
	m_stack.pop_back();
	m_game_over = false;
	m_next_color = (WHITE + BLACK) - m_next_color;		//	手番交代
}
void Board::gen_moves(Moves& mvs) {
	mvs.clear();
	if( m_next_board < 0 ) {	//	全ローカルボードに着手可能な場合
		for(int y = 0; y != N_VERT; ++y) {
			for(int x = 0; x != N_HORZ; ++x) {
				if( is_empty(x, y) )
					mvs.push_back(Move(x, y));
			}
		}
	} else {		//	next_board ローカルボードにのみ着手可能な場合
		int x0 = (m_next_board % 3) * 3;
		int y0 = (m_next_board / 3) * 3;
		for(int v = 0; v != 3; ++v) {
			for(int h = 0; h != 3; ++h) {
				if( is_empty(x0 + h, y0 + v) )
					mvs.push_back(Move(x0 + h, y0 + v));
			}
		}
	}
}
Move Board::sel_move_random() {
	if( m_stack.empty() ) {		//	履歴が無い＝初手の場合
		return Move(g_mt() % N_HORZ, g_mt() % N_VERT);
	}
	vector<Move> lst;
	if( m_next_board < 0 ) {	//	全ローカルボードに着手可能な場合
		for(int y = 0; y != N_VERT; ++y) {
			for(int x = 0; x != N_HORZ; ++x) {
				if( is_empty(x, y) )
					lst.push_back(Move(x, y));
			}
		}
	} else {		//	next_board ローカルボードにのみ着手可能な場合
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
Move Board::sel_move_Depth1() {
	Move mmv(-1, -1);
	int mm = next_color() == WHITE ? -INT_MAX : INT_MAX;
	if( next_board() < 0 ) {	//	全ローカルボードに打てる場合
		for(int ix = 0; ix != BD_SIZE; ++ix) {
			//if (ix == 54)
			//	cout << "ix == 54\n";
			if( is_empty(ix) ) {
				Move mv(ix % 9, ix / 9);
				put(mv, next_color());
				auto ev = eval();
				undo_put();
				if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
					mm = ev;
					mmv = mv;
				}
			}
		}
	} else {
		int x0 = (m_next_board % 3) * 3;
		int y0 = (m_next_board / 3) * 3;
		for(int v = 0; v != 3; ++v) {
			for(int h = 0; h != 3; ++h) {
				if( is_empty(x0 + h, y0 + v) ) {
					Move mv(x0 + h, y0 + v);
					put(mv, next_color());
					auto ev = eval();
					undo_put();
					if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
						mm = ev;
						mmv = mv;
					}
				}
			}
		}
	}
	return mmv;
}
int Board::minmax(int depth) {
	if( depth <= 0 )
		return eval();
	int mm = next_color() == WHITE ? -INT_MAX : INT_MAX;
	if( next_board() < 0 ) {	//	全ローカルボードに打てる場合
		for(int ix = 0; ix != BD_SIZE; ++ix) {
			if( is_empty(ix) ) {
				Move mv(ix % 9, ix / 9);
				put(mv, next_color());
				auto ev = minmax(depth-1);
				undo_put();
				if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
					mm = ev;
				}
			}
		}
	} else {
		int x0 = (m_next_board % 3) * 3;
		int y0 = (m_next_board / 3) * 3;
		for(int v = 0; v != 3; ++v) {
			for(int h = 0; h != 3; ++h) {
				if( is_empty(x0 + h, y0 + v) ) {
					Move mv(x0 + h, y0 + v);
					put(mv, next_color());
					auto ev = minmax(depth-1);
					undo_put();
					if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
						mm = ev;
					}
				}
			}
		}
	}
	return mm;
}
Move Board::sel_move_MinMax(int depth) {
	Move mmv(-1, -1);
	int mm = next_color() == WHITE ? -INT_MAX : INT_MAX;
	if( next_board() < 0 ) {	//	全ローカルボードに打てる場合
		for(int ix = 0; ix != BD_SIZE; ++ix) {
			if( is_empty(ix) ) {
				Move mv(ix % 9, ix / 9);
				put(mv, next_color());
				auto ev = minmax(depth - 1);
				undo_put();
				if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
					mm = ev;
					mmv = mv;
				}
			}
		}
	} else {
		int x0 = (m_next_board % 3) * 3;
		int y0 = (m_next_board / 3) * 3;
		for(int v = 0; v != 3; ++v) {
			for(int h = 0; h != 3; ++h) {
				if( is_empty(x0 + h, y0 + v) ) {
					Move mv(x0 + h, y0 + v);
					put(mv, next_color());
					auto ev = minmax(depth - 1);
					undo_put();
					if( (next_color() == WHITE && ev > mm) || (next_color() != WHITE && ev < mm)) {
						mm = ev;
						mmv = mv;
					}
				}
			}
		}
	}
	cout << "MinMaxVal = " << mm << "\n";
	cout << "move = (" << (int)mmv.m_x << ", " << (int)mmv.m_y << ")\n";
	return mmv;
}
int Board::playout_random() {
	for(;;) {
	    auto mv = sel_move_random();
	    put(mv.m_x, mv.m_y, next_color());
	    //print();
	    if( is_game_over() )
            break;
	}
	return m_winner;
}
int Board::playout_random(int N) {
	int sum = 0;
	for(int i = 0; i != N; ++i) {
		Board bd(*this);
		sum += bd.playout_random();
		//bd.print();
	}
	return sum;
}
void build_3x3_eval_table() {
	g_eval.clear();
	g_eval.resize(3*3*3*3*3*3*3*3*3);
}
