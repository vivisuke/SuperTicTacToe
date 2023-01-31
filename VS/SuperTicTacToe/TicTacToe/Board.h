//----------------------------------------------------------------------
//
//			File:			"Board.h"
//			Created:		31-1-2023
//			Author:			í√ìcêLèG
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
	void	init();
	void	print() const;
	void	put(int x, int y, char col);
private:
	char	m_board[BD_SIZE];
};
