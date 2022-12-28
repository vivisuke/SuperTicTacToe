extends Node2D

const N_VERT = 9
const N_HORZ = 9
const NEXT_LOCAL_BOARD = 0
const MARU = 1
const BATSU = 0

func _ready():
	#init_board()
	#put(2, 2, MARU)
	pass # Replace with function body.

func init_board():
	for y in range(N_VERT):
		for x in range(N_HORZ):
			$Board/TileMapLocal.set_cell(x, y, -1)
	for y in range(N_VERT/3):
		for x in range(N_HORZ/3):
			$Board/TileMapBG.set_cell(x, y, NEXT_LOCAL_BOARD)
			$Board/TileMapGlobal.set_cell(x, y, -1)
	pass

func put(x, y, col):
	$Board/TileMapLocal.set_cell(x, y, col)
	var x3 : int = x % 3
	var y3 : int = y % 3
	for gy in range(N_VERT/3):
		for gx in range(N_HORZ/3):
			var c = NEXT_LOCAL_BOARD if gx == x3 && gy == y3 else -1
			$Board/TileMapBG.set_cell(gx, gy, c)
