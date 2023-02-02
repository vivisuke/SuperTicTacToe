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

#define		N_HORZ		3
#define		N_VERT		3
#define		BD_SIZE		(N_HORZ*N_VERT)

#define		EMPTY		0
#define		WHITE		1			//	Ｏ、先手番
#define		BLACK		(-1)		//	Ｘ、後手番

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

	bool	is_game_over() const { return m_game_over; }		//	終局状態か？
	char	next_color() const { return m_next_color; }			//	次の手番
	void	change_color() { m_next_color = (WHITE + BLACK) - m_next_color; }
	char	get_color(int x, int y) const {
		return m_board[xyToIndex(x, y)];
	}
	bool	is_empty(int x, int y) const {
		return get_color(x, y) == EMPTY;
	}
	void	init();
	void	print() const;
	void	put(int x, int y, char col);
	void	put(Move& mv, char col) { put(mv.m_x, mv.m_y, col); }
	void	undo_put();
	Move	sel_move_random();
	Move	sel_move_PMC();				//	純粋モンテカルロ法による着手決定
	int		playout_random();			//	現状態から終局までランダム対局、return WHITE | EMPTY | BLACK
	int		playout_random(int N);		//	N 回プレイアウトを行い、結果合計 [-N, N] を返す
	int		Q_table_inedx() const;
private:
	bool	m_game_over;				//	終局状態か？
	char	m_winner;					//	勝者：WHITE | EMPTY | BLACK
	char	m_next_color;				//	次の手番
	char	m_board[BD_SIZE];
	std::vector<HistItem>	m_stack;
};
