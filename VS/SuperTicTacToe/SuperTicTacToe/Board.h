//----------------------------------------------------------------------
//
//			File:			"Board.h"
//			Created:		22-1-2023
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

#pragma once

//	全盤面サイズ
#define		N_HORZ		9
#define		N_VERT		9
#define		BD_SIZE		(N_HORZ*N_VERT)
#define		GBD_SIZE	(N_HORZ/3*N_VERT/3)

#define		EMPTY		0
#define		MARU		1
#define		BATSU		(-1)

class Board {
public:
	Board();
public:
	void	init();
	void	print() const;
private:
	char	m_board[BD_SIZE];
	char	m_gboard[GBD_SIZE];
};
