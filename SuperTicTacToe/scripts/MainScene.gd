extends Node2D

const N_VERT = 9
const N_HORZ = 9
const NEXT_LOCAL_BOARD = 0
const MARU = 1
const BATSU = 0
const WAIT = 6
enum {
	HUMAN = 0, AI,
}

var BOARD_ORG_X
var BOARD_ORG_Y
var BOARD_ORG
var n_put = 0					# 着手数
var put_pos = [-1, -1]			# 直前着手位置
var AI_thinking = false
var waiting = 0;				# ウェイト中カウンタ
var in_game = false				# ゲーム中
var next_color = MARU
var maru_player = AI
var batsu_player = HUMAN
var next_logical_board = [-1, -1]	# 着手可能ロジカルボード
var pressedPos = Vector2(0, 0)
var rng = RandomNumberGenerator.new()

func _ready():
	BOARD_ORG_X = $Board/TileMapLocal.global_position.x
	BOARD_ORG_Y = $Board/TileMapLocal.global_position.y
	BOARD_ORG = Vector2(BOARD_ORG_X, BOARD_ORG_Y)
	rng.randomize()
	init_board()
	update_next_label()
	#put(2, 2, MARU)
	pass # Replace with function body.

func init_board():
	n_put = 0
	next_color = MARU
	for y in range(N_VERT):
		for x in range(N_HORZ):
			$Board/TileMapLocal.set_cell(x, y, -1)
	for y in range(N_VERT/3):
		for x in range(N_HORZ/3):
			$Board/TileMapBG.set_cell(x, y, NEXT_LOCAL_BOARD)
			$Board/TileMapGlobal.set_cell(x, y, -1)
	pass
func update_next_label():
	$MessLabel.text = "次は%sの手番です。" % ("Ｏ" if next_color == MARU else "Ｘ")
func can_put_local(x : int, y : int):
	return $Board/TileMapBG.get_cell(x, y) == NEXT_LOCAL_BOARD
func is_empty(x : int, y : int):
	return $Board/TileMapLocal.get_cell(x, y) == -1
func put(x : int, y : int, col):
	if !is_empty(x, y): return
	n_put += 1
	put_pos = [x, y]
	$Board/TileMapLocal.set_cell(x, y, col)
	# 次着手可能ローカルボード強調
	var x3 : int = x % 3
	var y3 : int = y % 3
	for gy in range(N_VERT/3):
		for gx in range(N_HORZ/3):
			#var c = NEXT_LOCAL_BOARD if gx == x3 && gy == y3 else -1
			var c = -1
			if gx == x3 && gy == y3:
				c = NEXT_LOCAL_BOARD
				next_logical_board = [gx, gy]
			$Board/TileMapBG.set_cell(gx, gy, c)
func get_color(x : int, y : int):
	return $Board/TileMapLocal.get_cell(x, y)
func is_three_stones(x : int, y : int):		# 三目並んだか？
	var x3 : int = x % 3
	var x0 : int = x - x3		# ローカルボード内左端座標
	var y3 : int = y % 3
	var y0 : int = y - y3		# ローカルボード内上端座標
	if get_color(x0, y) == get_color(x0+1, y) && get_color(x0, y) == get_color(x0+2, y):
		return true;			# 横方向に三目並んだ
	if get_color(x, y0) == get_color(x, y0+1) && get_color(x, y0) == get_color(x, y0+2):
		return true;			# 縦方向に三目並んだ
	if x3 == y3:		# ＼斜め方向チェック
		if get_color(x0, y0) == get_color(x0+1, y0+1) && get_color(x0, y0) == get_color(x0+2, y0+2):
			return true;			# ＼斜め方向に三目並んだ
	if x3 == 2 - y3:		# ／斜め方向チェック
		if get_color(x0, y0+2) == get_color(x0+1, y0+1) && get_color(x0, y0+2) == get_color(x0+2, y0):
			return true;			# ／斜め方向に三目並んだ
func AI_think_random():
	if n_put == 0:		# 初手
		return [rng.randi_range(0, N_HORZ-1), rng.randi_range(0, N_VERT-1)]
	else:
		var x0 = next_logical_board[0] * 3
		var y0 = next_logical_board[1] * 3
		return [x0 + rng.randi_range(0, 2), y0 + rng.randi_range(0, 2)]
func _process(delta):
	if waiting > 0:
		waiting -= 1
	elif in_game && !AI_thinking && next_color == MARU:
		AI_thinking = true
		put_pos = AI_think_random()
		print("AI put ", put_pos)
		put(put_pos[0], put_pos[1], next_color)
		#if $Board/TileMapGlobal.get_cell(gx, gy) == -1 && is_three_stones(pos.x, pos.y):
		#	$Board/TileMapGlobal.set_cell(gx, gy, next_color)
		next_color = (MARU + BATSU) - next_color
		update_next_label()
		AI_thinking = false
		waiting = WAIT*10
	pass
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
			if n_put == 0:
				in_game = true
				return
			if pos.x < 0 || pos.x >= N_HORZ || pos.y < 0 || pos.y > N_VERT: return
			if !is_empty(pos.x, pos.y): return
			var gx = int(pos.x) / 3
			var gy = int(pos.y) / 3
			if !can_put_local(gx, gy): return
			put(pos.x, pos.y, next_color)
			if $Board/TileMapGlobal.get_cell(gx, gy) == -1 && is_three_stones(pos.x, pos.y):
				$Board/TileMapGlobal.set_cell(gx, gy, next_color)
			next_color = (MARU + BATSU) - next_color
			update_next_label()
			waiting = WAIT
	pass
