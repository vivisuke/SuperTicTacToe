//----------------------------------------------------------------------
//
//			File:			"Board.h"
//			Created:		31-1-2023
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

#define		N_HORZ		3
#define		N_VERT		3
#define		BD_SIZE		(N_HORZ*N_VERT)

#define		EMPTY		0
#define		WHITE		1
#define		BLACK		(-1)

class Board {
public:
	Board();
public:
	static int xyToIndex(int x, int y) { return x + y*N_HORZ; }

	bool	is_game_over() const { return m_game_over; }		//	終局状態か？
	void	init();
	void	print() const;
	void	put(int x, int y, char col);
private:
	bool	m_game_over;				//	終局状態か？
	char	m_board[BD_SIZE];
};
