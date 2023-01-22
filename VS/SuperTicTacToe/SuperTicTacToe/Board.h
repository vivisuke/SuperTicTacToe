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
#define		N_HORZ		9
#define		N_VERT		9
#define		BD_SIZE		(N_HORZ*N_VERT)
#define		GBD_SIZE	(N_HORZ/3*N_VERT/3)

#define		EMPTY		0
#define		MARU		1
#define		BATSU		(-1)

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
public:
	void	init();
	void	print() const;
	static int xyToIndex(int x, int y) { return x + y*N_HORZ; }
	static int xyToGix(int x, int y) { return (x/3) + (y/3)*(N_HORZ/3); }
	char	get_color(int x, int y) const {
		return m_board[x + y*N_HORZ];
	}
	void	update_next_board(int x, int y);
	bool	is_linedup(int x, int y) const;
	void	put(int x, int y, char col);
	void	undo_put();
private:
	char	m_next_board;				//	次に着手可能なローカルボード、-1 for 全ボードに着手可能
	char	m_board[BD_SIZE];
	char	m_gboard[GBD_SIZE];
	char	m_nput[GBD_SIZE];				//	各ローカルボード着手数
	bool	m_linedup[GBD_SIZE];			//	各ローカルボード：三目並んでいる
	std::vector<HistItem>		m_stack;
};
