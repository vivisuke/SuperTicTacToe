//----------------------------------------------------------------------
//
//			File:			"Board.h"
//			Created:		31-1-2023
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#include <vector>
#include <unordered_map>

#define		N_HORZ		3
#define		N_VERT		3
#define		BD_SIZE		(N_HORZ*N_VERT)

#define		EMPTY		0
#define		WHITE		1			//	Ｏ、先手番
#define		BLACK		(-1)		//	Ｘ、後手番

void init_vtable();
void print_next(class Board &bd);
void learn_CFR();
void learn_QPlus();		//	全アクションを試すQ学習？
//void learn_MinMaxQ();	//	最大Ｑ値行動を選択、Ｑ値は割引率γの min-max 評価値 [-1, +1] とする
void learn_MinMaxQ(class Board &bd, bool=false);
void learn_MinMaxQ_untilEnd(const class Board &bd, bool=false);		//	最大Ｑ値行動で終局状態まで学習

struct Move {
public:
	Move(char x = 0, char y = 0)
		: m_x(x), m_y(y)
	{
	}
public:
	char	m_x;
	char	m_y;
};

struct HistItem {
public:
	HistItem(char x = 0, char y = 0, char col = 0 /*, bool linedup = false*/)
		: m_x(x), m_y(y), m_col(col)
		//, m_linedup(linedup)
	{
	}
public:
	char	m_x;
	char	m_y;
	char	m_col;
	//bool	m_linedup;
};

class Board {
public:
	Board();
	Board(const Board&);
public:
	static int xyToIndex(int x, int y) { return x + y*N_HORZ; }

	bool	operator==(const Board&) const;
	bool	is_game_over() const { return m_game_over; }		//	終局状態か？
	char	winner() const { return m_winner; }
	char	next_color() const { return m_next_color; }			//	次の手番
	char	op_color() const { return (WHITE + BLACK) - m_next_color; }			//	相手の手番
	void	change_color() { m_next_color = op_color(); }
	char	get_color(int x, int y) const { return m_board[xyToIndex(x, y)]; }
	char	get_color(int ix) const { return m_board[ix]; }
	bool	is_empty(int x, int y) const { return get_color(x, y) == EMPTY; }
	bool	is_empty(int ix) const { return m_board[ix] == EMPTY; }
	void	init();
	void	print() const;
	void	print_rtable() const;
	void	print_qtable() const;
	int		eval() const;
	void	put(int ix, char col) { put(ix%N_HORZ, ix/N_HORZ, col); }
	void	put(int x, int y, char col);
	void	put(Move& mv, char col) { put(mv.m_x, mv.m_y, col); }
	void	undo_put();
	bool	find_make_three(Move&, char col);		//	col の手番で、三目並べることが出来る着手を探す
	Move	sel_move_random();
	Move	sel_move_PMC();				//	純粋モンテカルロ法による着手決定
	Move	sel_move_MC(bool=false);				//	三目並ぶ手を優先するモンテカルロ法による着手決定
	Move	sel_move_perfect();			//	完全読み結果を参照するAI
	Move	sel_move_CFR();						//	CFRアルゴリズム AI
	Move	sel_move_MinMax(int depth = 3);		//	評価関数＋ミニマックス AI
	int		playout_random(bool=false);			//	現状態から終局までランダム対局、return WHITE | EMPTY | BLACK
	int		playout_random(int N, bool=false);		//	N 回プレイアウトを行い、結果合計 [-N, N] を返す
	//int		playout_three_random();			//	現状態から終局までランダム対局、return WHITE | EMPTY | BLACK
	//int		playout_three_random(int N);	//	N 回プレイアウトを行い、結果合計 [-N, N] を返す
	int		Q_table_inedx() const;
	int		hash() const;				//	ハッシュ値計算、対称形考慮
	int		hash_asym() const;			//	ハッシュ値計算、対称形非考慮
private:
	bool	m_game_over;				//	終局状態か？
	char	m_winner;					//	勝者：WHITE | EMPTY | BLACK
	char	m_next_color;				//	次の手番
	char	m_board[BD_SIZE];
	std::vector<HistItem>	m_stack;
};
