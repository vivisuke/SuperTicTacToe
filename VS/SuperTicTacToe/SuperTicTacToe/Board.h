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
	HistItem(char x = 0, char y = 0, char col = 0)
		: m_x(x), m_y(y), m_col(col)
	{
	}
public:
	char	m_x;
	char	m_y;
	char	m_col;
};

class Board {
public:
	Board();
public:
	void	init();
	void	print() const;
	void	put(int x, int y, char col);
private:
	char	m_board[BD_SIZE];
	char	m_gboard[GBD_SIZE];
	char	m_nput[GBD_SIZE];				//	各ローカルボード着手数
	std::vector<HistItem>		m_stack;
};
