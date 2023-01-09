extends Node2D

const N_VERT = 9
const N_HORZ = 9
const NEXT_LOCAL_BOARD = 0
#const MARU = 1
#const BATSU = 0
const WAIT = 6*3
const ev_table = [	# 空き箇所に打った場合の利得
	1,  	# ・・・
	8,  	# ・・Ｘ
	8,  	# ・・Ｏ
	8,  	# ・Ｘ・
	32,  	# ・ＸＸ
	0,  	# ・ＸＯ
	8,  	# ・Ｏ・
	0,  	# ・ＯＸ
	32,  	# ・ＯＯ
	8,		# Ｘ・・
	32,  	# Ｘ・Ｘ
	0,  	# Ｘ・Ｏ
	32,		# ＸＸ・
	0,  	# ＸＸＸ
	0,  	# ＸＸＯ
	0,  	# ＸＯ・
	0,  	# ＸＯＸ
	0,  	# ＸＯＯ
	8,		# Ｏ・・
	0,  	# Ｏ・Ｘ
	32,  	# Ｏ・Ｏ
	0,  	# ＯＸ・
	0,  	# ＯＸＸ
	0,  	# ＯＸＯ
	32,		# ＯＯ・
	0,  	# ＯＯＸ
	0,  	# ＯＯＯ
]
#const ev_table = [	# 空き箇所に打った場合の利得
#	[ 1,  1,  1],	# ・・・
#	[ 8,  8,  0],	# ・・Ｘ
#	[ 8,  8,  0],	# ・・Ｏ
#	[ 8,  0,  8],	# ・Ｘ・
#	[32,  0,  0],	# ・ＸＸ
#	[ 0,  0,  0],	# ・ＸＯ
#	[ 8,  0,  8],	# ・Ｏ・
#	[ 0,  0,  0],	# ・ＯＸ
#	[32,  0,  0],	# ・ＯＯ
#	[ 0,  8,  8],	# Ｘ・・
#	[ 0, 32,  0],	# Ｘ・Ｘ
#	[ 0,  0,  0],	# Ｘ・Ｏ
#	[ 0,  0, 32],	# ＸＸ・
#	[ 0,  0,  0],	# ＸＸＸ
#	[ 0,  0,  0],	# ＸＸＯ
#	[ 0,  0,  0],	# ＸＯ・
#	[ 0,  0,  0],	# ＸＯＸ
#	[ 0,  0,  0],	# ＸＯＯ
#	[ 0,  8,  8],	# Ｏ・・
#	[ 0,  0,  0],	# Ｏ・Ｘ
#	[ 0, 32,  0],	# Ｏ・Ｏ
#	[ 0,  0,  0],	# ＯＸ・
#	[ 0,  0,  0],	# ＯＸＸ
#	[ 0,  0,  0],	# ＯＸＯ
#	[ 0,  0, 32],	# ＯＯ・
#	[ 0,  0,  0],	# ＯＯＸ
#	[ 0,  0,  0],	# ＯＯＯ
#]
const mb_str = ["Ｘ", "Ｏ"]
enum {
	EMPTY = -1,
	BATSU = 0, MARU,
	HUMAN = 0, AI_RANDOM, AI_LEVEL_1,
}
class Board:
	var n_put = 0				# 着手数
	var is_game_over			# 終局状態か？
	var winner					# 勝者
	var next_board = -1			# 着手可能ローカルボード [0, 9)、-1 for 全ローカルボードに着手可能
	var next_color
	var l_board
	var g_board
	var n_put_local = []		# 各ローカルボードの着手数
	var three_lined_up = []		# 各ローカルボード：三目並んだか？
	var rng = RandomNumberGenerator.new()
	func _init():
		rng.randomize()		# Setups a time-based seed
		#rng.seed = 0			# 固定乱数系列
		n_put = 0
		is_game_over = false
		winner = -1
		next_color = MARU
		n_put_local = [0, 0, 0, 0, 0, 0, 0, 0, 0]
		three_lined_up = [false, false, false, false, false, false, false, false, false]
		next_board = -1
		l_board = []
		for ix in range(N_HORZ*N_VERT): l_board.push_back(EMPTY)
		g_board = []
		for ix in range(N_HORZ*N_VERT/9): g_board.push_back(EMPTY)
		#print(ev_table)
		pass
	func print():
		var txt = "  abc def ghi\n"
		txt += " +---+---+---+\n"
		for y in range(N_VERT):
			txt += "%d|" % (y+1)
			for x in range(N_HORZ):
				txt += ".XO"[l_board[x + y*N_HORZ]+1]
				if x % 3 == 2: txt += "|"
			txt += "\n"
			if y % 3 == 2: txt += " +---+---+---+\n"
		txt += "\n"
		for y in range(N_VERT/3):
			for x in range(N_HORZ/3):
				txt += ".XO"[g_board[x + y*(N_HORZ/3)]+1]
			txt += "\n"
		txt += "\n"
		if is_game_over:
			if winner == MARU: txt += "O won.\n"
			elif winner == BATSU: txt += "X won.\n"
			else: txt += "draw.\n"
		else:
			txt += "next turn color: %s\n" % ("O" if next_color == MARU else "X")
		print(txt)
	func is_empty(x : int, y : int):			# ローカルボード内のセル状態取得
		return l_board[x + y*N_HORZ] == EMPTY
	func get_color(x : int, y : int):			# ローカルボード内のセル状態取得
		return l_board[x + y*N_HORZ]
	func get_color_g(gx : int, gy : int):		# グローバルボード内のセル状態取得
		return g_board[gx + gy*(N_HORZ/3)]
	func update_next_board(x : int, y : int):	# 次に着手可能なローカルボード設定
		var x3 = x % 3
		var y3 = y % 3
		next_board = x3 + y3 * 3
		if three_lined_up[next_board] || n_put_local[next_board] == 9:
			next_board = -1			# 全ローカルボードに着手可能
	func put(x : int, y : int, col):
		n_put += 1					# トータル着手数
		l_board[x + y*N_HORZ] = col
		var gx = x / 3
		var gy = y / 3
		var ix = gx + gy*3
		n_put_local[ix] += 1		# 各ローカルボードの着手数
		if !three_lined_up[ix] && is_three_stones(x, y):	# 三目並んだ→グローバルボード更新
			g_board[ix] = col
			three_lined_up[ix] = true
			if is_three_stones_global(gx, gy):
				is_game_over = true
				winner = col
		if !is_game_over && n_put == N_HORZ*N_VERT:
			is_game_over = true
		update_next_board(x, y)					# next_board 設定
	func change_turn():				# 手番交代
		next_color = (MARU + BATSU) - next_color
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
	func is_three_stones_global(gx : int, gy : int):		# グローバルボードで三目並んだか？
		if get_color_g(0, gy) == get_color_g(1, gy) && get_color_g(0, gy) == get_color_g(2, gy):
			return true;			# 横方向に三目並んだ
		if get_color_g(gx, 0) == get_color_g(gx, 1) && get_color_g(gx, 0) == get_color_g(gx, 2):
			return true;			# 縦方向に三目並んだ
		if gx == gy:		# ＼斜め方向チェック
			if get_color_g(0, 0) == get_color_g(1, 1) && get_color_g(0, 0) == get_color_g(2, 2):
				return true;			# ＼斜め方向に三目並んだ
		if gx == 2 - gy:		# ／斜め方向チェック
			if get_color_g(0, 2) == get_color_g(1, 1) && get_color_g(0, 2) == get_color_g(2, 0):
				return true;			# ／斜め方向に三目並んだ
	func select_random():
		if n_put == 0:		# 初期状態
			return [rng.randi_range(0, N_HORZ-1), rng.randi_range(0, N_VERT-1)]
		elif next_board < 0:	# 全てのローカルボードに着手可能
			var lst = []
			for y in range(N_VERT):
				for x in range(N_HORZ):
					if is_empty(x, y): lst.push_back([x, y])
			return lst[rng.randi_range(0, lst.size() - 1)]
		else:
			var x0 = (next_board % 3) * 3
			var y0 = (next_board / 3) * 3
			var lst = []
			for v in range(3):
				for h in range(3):
					if is_empty(x0+h, y0+v):
						lst.push_back([x0+h, y0+v])
			return lst[rng.randi_range(0, lst.size() - 1)]
	func select_depth_1():		# １手先読み（着手評価のみ）で着手決定
		if next_board < 0:	# 全てのローカルボードに着手可能
			# undone: 未実装
			return select_random()
		else:
			var p = [-1, -1]
			var mx = -9999
			var x0 = (next_board % 3) * 3
			var y0 = (next_board / 3) * 3
			var txt = ""
			for v in range(3):
				for h in range(3):
					var ev = eval_put(x0+h, y0+v)
					if ev > mx:
						mx = ev
						p = [x0+h, y0+v]
			return p
	func eval_put(x: int, y: int):		# (x, y) への着手を評価
		if !is_empty(x, y): return -1
		var x0 = x - x % 3
		var y0 = y - y % 3
		var ix = 0
		for h in range(N_HORZ/3):			# 横方向
			ix = ix * 3 + get_color(x0 + h, y) + 1
		var ev = ev_table[ix]
		ix = 0
		for v in range(N_HORZ/3):			# 縦方向
			ix = ix * 3 + get_color(x, y0 + v) + 1
		ev += ev_table[ix]
		if x%3 == y%3:					
			ix = 0
			for h in range(N_HORZ/3):		# ＼方向
				ix = ix * 3 + get_color(x0 + h, y0 + h) + 1
			ev += ev_table[ix]
		if x%3 == (2 - y%3):					
			ix = 0
			for h in range(N_HORZ/3):		# ／方向
				ix = ix * 3 + get_color(x0 + h, y0 + 2 - h) + 1
			ev += ev_table[ix]
		return ev

var BOARD_ORG_X
var BOARD_ORG_Y
var BOARD_ORG
var n_put = 0					# 着手数
var n_put_board = []			# 各ローカルボードの着手数
var three_lined_up = []			# 各ローカルボード：三目並んだか？
var put_pos = [-1, -1]			# 直前着手位置
var AI_thinking = false
var waiting = 0;				# ウェイト中カウンタ
var game_started = false				# ゲーム中
var next_color = MARU
var maru_player = HUMAN
var batsu_player = HUMAN
#var next_logical_board = [-1, -1]	# 着手可能ロジカルボード
var next_board = -1				# [0, 9): 着手可能ロジカルボード、-1 for すべてのボードに着手可能
var pressedPos = Vector2(0, 0)
var g_bd
var rng = RandomNumberGenerator.new()

func _ready():
	print(Time.get_ticks_usec())
	#
	g_bd = Board.new()
	#print(bd.l_board)
	#print(bd.g_board)
	#bd.put(0, 0, MARU)
	#bd.put(1, 1, MARU)
	#bd.put(2, 2, MARU)
	g_bd.print()
	#
	BOARD_ORG_X = $Board/TileMapLocal.global_position.x
	BOARD_ORG_Y = $Board/TileMapLocal.global_position.y
	BOARD_ORG = Vector2(BOARD_ORG_X, BOARD_ORG_Y)
	setup_player_option_button($MaruPlayer/OptionButton)
	setup_player_option_button($BatsuPlayer/OptionButton)
	$BatsuPlayer/OptionButton.selected = AI_RANDOM
	batsu_player = AI_RANDOM
	rng.randomize()		# Setups a time-based seed
	#rng.seed = 0			# 固定乱数系列
	init_board()
	update_next_label()
	#put(2, 2, MARU)
	$MessLabel.text = "【Start Game】を押してください。"
	print(Time.get_ticks_usec())
	pass # Replace with function body.
func setup_player_option_button(ob):
	ob.add_item(": Human", 0)	
	ob.add_item(": AI Random", 1)	
	ob.add_item(": AI Level 1", 2)	# １手先読み
func init_board():
	n_put = 0
	put_pos = [-1, -1]
	n_put_board = [0, 0, 0, 0, 0, 0, 0, 0, 0]
	three_lined_up = [false, false, false, false, false, false, false, false, false]
	next_color = MARU
	for y in range(N_VERT):
		for x in range(N_HORZ):
			$Board/TileMapLocal.set_cell(x, y, -1)
			$Board/TileMapCursor.set_cell(x, y, -1)
	for y in range(N_VERT/3):
		for x in range(N_HORZ/3):
			$Board/TileMapBG.set_cell(x, y, NEXT_LOCAL_BOARD)
			$Board/TileMapGlobal.set_cell(x, y, -1)
	pass
func update_next_label():
	$MessLabel.text = "次は%sの手番です。" % ("Ｏ" if next_color == MARU else "Ｘ")
	$MaruPlayer/Underline.visible = next_color == MARU
	$BatsuPlayer/Underline.visible = next_color == BATSU
func can_put_local(x : int, y : int):
	return $Board/TileMapBG.get_cell(x, y) == NEXT_LOCAL_BOARD
func is_empty(x : int, y : int):
	return $Board/TileMapLocal.get_cell(x, y) == -1
func put(x : int, y : int, col):
	if !is_empty(x, y): return
	n_put += 1
	var gix = x/3 + (y/3)*3
	n_put_board[gix] += 1
	print("n = ", n_put_board[gix])
	if put_pos[0] >= 0 && put_pos[1] >= 0:
		$Board/TileMapCursor.set_cell(put_pos[0], put_pos[1], -1)
	put_pos = [x, y]
	$Board/TileMapCursor.set_cell(put_pos[0], put_pos[1], 0)
	$Board/TileMapLocal.set_cell(x, y, col)
	if n_put == N_HORZ * N_VERT:	# 全て着手済み → 終局
		#game_started = false
		do_game_over()
		$MessLabel.text = "draw."
	else:
		# 次着手可能ローカルボード強調
		var x3 : int = x % 3
		var y3 : int = y % 3
		for gy in range(N_VERT/3):
			for gx in range(N_HORZ/3):
				#var c = NEXT_LOCAL_BOARD if gx == x3 && gy == y3 else -1
				var c = -1
				if gx == x3 && gy == y3:
					c = NEXT_LOCAL_BOARD
					#next_logical_board = [gx, gy]
					next_board = gx + gy * 3
				$Board/TileMapBG.set_cell(gx, gy, c)
func get_color(x : int, y : int):			# ローカルボード内のセル状態取得
	return $Board/TileMapLocal.get_cell(x, y)
func get_color_g(gx : int, gy : int):		# グローバルボード内のセル状態取得
	return $Board/TileMapGlobal.get_cell(gx, gy)
func is_three_stones_global(gx : int, gy : int):		# グローバルボードで三目並んだか？
	if get_color_g(0, gy) == get_color_g(1, gy) && get_color_g(0, gy) == get_color_g(2, gy):
		return true;			# 横方向に三目並んだ
	if get_color_g(gx, 0) == get_color_g(gx, 1) && get_color_g(gx, 0) == get_color_g(gx, 2):
		return true;			# 縦方向に三目並んだ
	if gx == gy:		# ＼斜め方向チェック
		if get_color_g(0, 0) == get_color_g(1, 1) && get_color_g(0, 0) == get_color_g(2, 2):
			return true;			# ＼斜め方向に三目並んだ
	if gx == 2 - gy:		# ／斜め方向チェック
		if get_color_g(0, 2) == get_color_g(1, 1) && get_color_g(0, 2) == get_color_g(2, 0):
			return true;			# ／斜め方向に三目並んだ
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
	elif next_board < 0:	# 全てのローカルボードに着手可能
		var lst = []
		for y in range(N_VERT):
			for x in range(N_HORZ):
				if is_empty(x, y): lst.push_back([x, y])
		return lst[rng.randi_range(0, lst.size() - 1)]
	else:
		var x0 = (next_board % 3) * 3
		var y0 = (next_board / 3) * 3
		var lst = []
		for v in range(3):
			for h in range(3):
				if is_empty(x0+h, y0+v):
					lst.push_back([x0+h, y0+v])
		return lst[rng.randi_range(0, lst.size() - 1)]
func put_and_post_proc(x : int, y : int):	# 着手処理とその後処理
	put(x, y, next_color)
	var gx = int(x) / 3
	var gy = int(y) / 3
	if $Board/TileMapGlobal.get_cell(gx, gy) == -1 && is_three_stones(x, y):
		# ローカルボード内で三目並んだ場合
		three_lined_up[gx + gy*3] = true
		$Board/TileMapGlobal.set_cell(gx, gy, next_color)
		if is_three_stones_global(gx, gy):
			game_started = false
			$MessLabel.text = mb_str[next_color] + " won."		# 
			return true;		# ゲームオーバー
	if n_put_board[next_board] == 9 || three_lined_up[next_board]:	# 空欄が無い or すでに三目並んでいる
		next_board = -1
		for gy2 in range(N_VERT/3):
			for gx2 in range(N_HORZ/3):
				$Board/TileMapBG.set_cell(gx2, gy2, NEXT_LOCAL_BOARD)
	next_color = (MARU + BATSU) - next_color
	update_next_label()
	return false
func _process(delta):
	if waiting > 0:
		waiting -= 1
	elif( game_started && !AI_thinking &&
			next_color == MARU && maru_player >= AI_RANDOM || next_color == BATSU && batsu_player >= AI_RANDOM):
		AI_thinking = true
		var pos = AI_think_random()
		print("AI put ", pos)
		put_and_post_proc(pos[0], pos[1])
		waiting = WAIT
		AI_thinking = false
	pass
func _input(event):
	if event is InputEventMouseButton:
		#print(event.position)
		#print($Board/TileMapLocal.world_to_map(event.position - BOARD_ORG))
		var pos = $Board/TileMapLocal.world_to_map(event.position - BOARD_ORG)
		#print("mouse button")
		if event.is_pressed():
			#print("pressed")
			pressedPos = pos
		elif pos == pressedPos:
			#print("released")
			#if n_put == 0:
			#	game_started = true
			#	return
			if pos.x < 0 || pos.x >= N_HORZ || pos.y < 0 || pos.y > N_VERT: return
			if !is_empty(pos.x, pos.y): return
			var gx = int(pos.x) / 3
			var gy = int(pos.y) / 3
			if !can_put_local(gx, gy): return
			if put_and_post_proc(pos.x, pos.y): return
			waiting = WAIT
	pass

func do_game_over():
	$MaruPlayer/OptionButton.disabled = false
	$BatsuPlayer/OptionButton.disabled = false
	$StartStopButton.text = "Start Game"
func _on_StartStopButton_pressed():
	game_started = !game_started
	if game_started:
		$MaruPlayer/OptionButton.disabled = true
		$BatsuPlayer/OptionButton.disabled = true
		init_board()
		$StartStopButton.text = "Stop Game"
		$MessLabel.text = "次の手番はＯです。"
	else:
		$MessLabel.text = ""
		do_game_over()
	pass # Replace with function body.


func _on_MaruOptionButton_item_selected(index):
	maru_player = index
	pass # Replace with function body.


func _on_BatsuOptionButton_item_selected(index):
	batsu_player = index
	pass # Replace with function body.


func _on_TestButton_pressed():
	if g_bd.is_game_over: return
	#var p = g_bd.select_random()
	var p = g_bd.select_depth_1()
	print(p)
	g_bd.put(p[0], p[1], g_bd.next_color)
	g_bd.change_turn()
	g_bd.print()
	if g_bd.next_board >= 0:
		#print(g_bd.next_board)
		print("(%d, %d)" % [g_bd.next_board%3, g_bd.next_board/3])
		var x0 = (g_bd.next_board % 3) * 3
		var y0 = (g_bd.next_board / 3) * 3
		var txt = ""
		for v in range(3):
			for h in range(3):
				txt += "%d, " % g_bd.eval_put(x0+h, y0+v)
			txt += "\n"
		print(txt)
	pass # Replace with function body.
