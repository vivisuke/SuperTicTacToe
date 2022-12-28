extends Node2D

const N_VERT = 9
const N_HORZ = 9
const NEXT_LOCAL_BOARD = 0
const MARU = 1
const BATSU = 0

var BOARD_ORG_X
var BOARD_ORG_Y
var BOARD_ORG
var pressedPos = Vector2(0, 0)
var next_color = MARU

func _ready():
	BOARD_ORG_X = $Board/TileMapLocal.global_position.x
	BOARD_ORG_Y = $Board/TileMapLocal.global_position.y
	BOARD_ORG = Vector2(BOARD_ORG_X, BOARD_ORG_Y)
	init_board()
	#put(2, 2, MARU)
	pass # Replace with function body.

func init_board():
	next_color = MARU
	for y in range(N_VERT):
		for x in range(N_HORZ):
			$Board/TileMapLocal.set_cell(x, y, -1)
	for y in range(N_VERT/3):
		for x in range(N_HORZ/3):
			$Board/TileMapBG.set_cell(x, y, NEXT_LOCAL_BOARD)
			$Board/TileMapGlobal.set_cell(x, y, -1)
	pass

func can_put_local(x : int, y : int):
	return $Board/TileMapBG.get_cell(x, y) == NEXT_LOCAL_BOARD
func is_empty(x : int, y : int):
	return $Board/TileMapLocal.get_cell(x, y) == -1
func put(x : int, y : int, col):
	if !is_empty(x, y): return
	$Board/TileMapLocal.set_cell(x, y, col)
	var x3 : int = x % 3
	var y3 : int = y % 3
	for gy in range(N_VERT/3):
		for gx in range(N_HORZ/3):
			var c = NEXT_LOCAL_BOARD if gx == x3 && gy == y3 else -1
			$Board/TileMapBG.set_cell(gx, gy, c)

func _input(event):
	if event is InputEventMouseButton:
		#print(event.position)
		#print($Board/TileMapLocal.world_to_map(event.position - BOARD_ORG))
		var pos = $Board/TileMapLocal.world_to_map(event.position - BOARD_ORG)
		#print("mouse button")
		if event.is_pressed():
			print("pressed")
			pressedPos = pos
		elif pos == pressedPos:
			print("released")
			if !is_empty(pos.x, pos.y): return
			var gx = int(pos.x) / 3
			var gy = int(pos.y) / 3
			if !can_put_local(gx, gy): return
			put(pos.x, pos.y, next_color)
			next_color = (MARU + BATSU) - next_color
	pass
