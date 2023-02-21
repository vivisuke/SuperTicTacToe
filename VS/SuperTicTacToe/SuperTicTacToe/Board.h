//----------------------------------------------------------------------
//
//			File:			"Board.h"
//			Created:		22-1-2023
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#include <vector>

//	全盤面サイズ
#define		N_HORZ3		3
#define		N_VERT3		3
#define		N_HORZ		9
#define		N_VERT		9
#define		BD_SIZE		(N_HORZ*N_VERT)
#define		GBD_SIZE	(N_HORZ3*N_VERT3)

#define		EMPTY		0
#define		WHITE		1
#define		BLACK		(-1)

void build_3x3_eval_table();
int	get_3x3_eval(int ix);

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

typedef std::vector<Move> Moves;

struct HistItem {
public:
	HistItem(char x = 0, char y = 0, char col = 0, bool linedup = false)
		: m_x(x), m_y(y), m_col(col), m_linedup(linedup)
	{
	}
public:
	char	m_x;
	char	m_y;
	char	m_col;
	bool	m_linedup;
};

class Board {
public:
	Board();
	Board(const Board&);
public:
	void	init();
	char	next_board() const { return m_next_board; }
	bool	is_game_over() const { return m_game_over; }		//	終局状態か？
	char	next_color() const { return m_next_color; }			//	次の手番
	void	print() const;
	static int xyToIndex(int x, int y) { return x + y*N_HORZ; }
	static int xyToGix(int x, int y) { return (x/3) + (y/3)*(N_HORZ/3); }
	char	get_color(int x, int y) const {
		return m_board[x + y*N_HORZ];
	}
	char	get_gcolor(int x, int y) const {
		return m_gboard[x + y*3];
	}
	bool	is_empty(int x, int y) const {
		return get_color(x, y) == EMPTY;
	}
	bool	is_empty(int ix) const { return m_board[ix] == EMPTY; }
	void	update_next_board(int x, int y);
	bool	is_linedup(int x, int y) const;
	bool	is_game_over(int x, int y) const;	//	終局（空欄無し or 三目並んだatグローバルボード）か？
	int		eval() const;
	void	put(const Move &mv, char col) { put(mv.m_x, mv.m_y, col); }
	void	put(int ix, char col) { put(ix % N_HORZ, ix / N_HORZ, col); }
	void	put(int x, int y, char col);
	void	undo_put();
	void	gen_moves(Moves&);
	Move	sel_move_random();
	Move	sel_move_Depth1();			//	評価関数＋１手先読み
	int		minmax(int depth);
	Move	sel_move_MinMax(int depth=3);			//	評価関数＋depth先読み
	int		playout_random();			//	現状態から終局までランダム対局、return WHITE | EMPTY | BLACK
	int		playout_random(int N);		//	
private:
	bool	m_game_over;				//	終局状態か？
	char	m_next_color;				//	次の手番
	char	m_next_board;				//	次に着手可能なローカルボード、-1 for 全ボードに着手可能
	char	m_winner;					//	勝者：WHITE | BLACK | EMPTY
	char	m_board[BD_SIZE];
	char	m_gboard[GBD_SIZE];
	char	m_nput[GBD_SIZE];				//	各ローカルボード着手数
	bool	m_linedup[GBD_SIZE];			//	各ローカルボード：三目並んでいる
	std::vector<HistItem>		m_stack;
};
