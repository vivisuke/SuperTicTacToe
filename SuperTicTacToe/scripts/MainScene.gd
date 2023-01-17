extends Node2D

const N_VERT = 9
const N_HORZ = 9
const NEXT_LOCAL_BOARD = 0
#const MARU = 1
#const BATSU = 0
const WAIT = 6*3
const ev_put_table = [	# 空き箇所に打った場合の利得
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
const ev_pat_table = [	# ○から見たパターン評価値
	0,  	# ・・・
	-1,  	# ・・Ｘ
	1,  	# ・・Ｏ
	-1,  	# ・Ｘ・
	-8,  	# ・ＸＸ
	0,  	# ・ＸＯ
	1,  	# ・Ｏ・
	0,  	# ・ＯＸ
	8,  	# ・ＯＯ
	-1,		# Ｘ・・
	-8,  	# Ｘ・Ｘ
	0,  	# Ｘ・Ｏ
	-8,		# ＸＸ・
	-32,  	# ＸＸＸ
	0,  	# ＸＸＯ
	0,  	# ＸＯ・
	0,  	# ＸＯＸ
	0,  	# ＸＯＯ
	1,		# Ｏ・・
	0,  	# Ｏ・Ｘ
	8,  	# Ｏ・Ｏ
	0,  	# ＯＸ・
	0,  	# ＯＸＸ
	0,  	# ＯＸＯ
	8,		# ＯＯ・
	0,  	# ＯＯＸ
	32,  	# ＯＯＯ
]
const mb_str = ["Ｘ", "Ｏ"]
enum {
	EMPTY = -1,
	BATSU = 0, MARU,
	HUMAN = 0, AI_RANDOM, AI_DEPTH_1, AI_DEPTH_3,
}
class HistItem:
	var x:int				# 着手位置
	var y:int
	var linedup:bool		# 着手により、ローカルボード内で三目並んだ
	#var is_game_over		# 着手により、グローバルボード内で三目並んだ
	var next_board:int		# （着手前）着手可能ローカルボード [0, 9)
	func _init(px, py, lu, nb):
		x = px
		y = py
		linedup = lu
		next_board = nb
class Board:
	var n_put = 0				# 着手数
	#var last_put_pos = [-1, -1]	# 直前着手箇所
	var is_game_over			# 終局状態か？
	var winner					# 勝者
	var next_board = -1			# 着手可能ローカルボード [0, 9)、-1 for 全ローカルボードに着手可能
	var next_color
	var l_board
	var g_board
	var n_put_local = []		# 各ローカルボードの着手数
	var three_lined_up = []		# 各ローカルボード：三目並んだか？
	var stack = []				# 要素：HistItem
	var rng = RandomNumberGenerator.new()
	func _init():
		rng.randomize()		# Setups a time-based seed
		#rng.seed = 0			# 固定乱数系列
		init()
		#print(ev_put_table)
		pass
	func init():
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
		stack = []
	func copy(s : Board):
		is_game_over = s.is_game_over
		winner = s.winner
		next_board = s.next_board
		next_color = s.next_color
		l_board = s.l_board.duplicate()
		g_board = s.g_board.duplicate()
		n_put_local = s.n_put_local.duplicate()
		three_lined_up = s.three_lined_up.duplicate()
		stack = s.stack.duplicate()
		pass
	func last_put_pos():
		if stack.empty(): return [-1, -1]
		else: return [stack.back().x, stack.back().y]
	func print():
		var txt = "  abc def ghi\n"
		txt += " +---+---+---+\n"
		for y in range(N_VERT):
			txt += "%d|" % (y+1)
			for x in range(N_HORZ):
				if last_put_pos() != [x, y]:
					txt += ".XO"[l_board[x + y*N_HORZ]+1]
				else:
					txt += ".#C"[l_board[x + y*N_HORZ]+1]
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
			txt += "next board = %d\n" % next_board
		txt += "last_put_pos = [%d, %d]\n" % last_put_pos()
		print(txt)
		#print("last_put_pos = ", last_put_pos())
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
		#last_put_pos = [x, y]
		n_put += 1					# トータル着手数
		l_board[x + y*N_HORZ] = col
		var gx = x / 3
		var gy = y / 3
		var ix = gx + gy*3
		n_put_local[ix] += 1		# 各ローカルボードの着手数
		var linedup = false			# ローカルボード内で三目ならんだか？
		#var igo = is_game_over		# グローバルボード内で三目ならんだか？
		if !three_lined_up[ix] && is_three_stones(x, y):	# 三目並んだ→グローバルボード更新
			linedup = true
			g_board[ix] = col
			three_lined_up[ix] = true
			if is_three_stones_global(gx, gy):
				is_game_over = true
				winner = col
		if !is_game_over && n_put == N_HORZ*N_VERT:
			is_game_over = true
		stack.push_back(HistItem.new(x, y, linedup, next_board))
		next_color = (MARU + BATSU) - next_color	# 手番交代
		update_next_board(x, y)					# next_board 設定
	func unput():
		if stack.empty(): return
		next_color = (MARU + BATSU) - next_color	# 手番交代
		var itm = stack.pop_back()
		l_board[itm.x + itm.y*N_HORZ] = EMPTY
		if itm.linedup:				# 着手で三目並んだ場合
			var gx = itm.x / 3
			var gy = itm.y / 3
			g_board[gx + gy*3] = EMPTY
			is_game_over = false
		next_board = itm.next_board
		pass
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
		var p = [-1, -1]
		var mx = -9999
		if next_board < 0:	# 全てのローカルボードに着手可能
			for y in range(N_VERT):
				for x in range(N_HORZ):
					if is_empty(x, y):
						var ev = eval_put(x, y)
						if ev > mx:
							mx = ev
							p = [x, y]
		else:
			var x0 = (next_board % 3) * 3
			var y0 = (next_board / 3) * 3
			#var txt = ""
			for v in range(3):
				for h in range(3):
					if is_empty(x0+h, y0+v):
						var ev = eval_put(x0+h, y0+v)
						#txt += "%d, " % ev
						if ev > mx:
							mx = ev
							p = [x0+h, y0+v]
				#txt += "\n"
			#print(txt)
		return p
	func alpha_bata(alpha, beta, depth):
		if depth == 0: return eval_board()
		var x0
		var y0
		var NH = 3
		var NV = 3
		if next_board < 0:		# 全ローカルボードに着手可能
			x0 = 0
			y0 = 0
			NH = N_HORZ
			NV = N_VERT
		else:
			x0 = (next_board % 3) * 3
			y0 = (next_board / 3) * 3
		for v in range(NV):
			for h in range(NH):
				if is_empty(x0+h, y0+v):
					put(x0+h, y0+v, next_color)
					if is_game_over:
						unput()
						if next_color == MARU:
							return 9000
						else:
							return -9000
					var ev = alpha_bata(alpha, beta, depth-1)
					unput()
					if next_color == MARU:
						alpha = max(ev, alpha)
						if alpha >= beta: return alpha
					else:
						beta = min(ev, beta)
						if alpha >= beta: return beta
		if next_color == MARU:
			return alpha
		else:
			return beta
	func select_depth_3():		# ３手先読み（着手評価のみ）で着手決定
		var bd = Board.new()
		bd.copy(self)
		var DEPTH = 3
		var ps;
		var alpha = -9999
		var beta = 9999
		var x0
		var y0
		var NH = 3
		var NV = 3
		if next_board < 0:		# 全ローカルボードに着手可能
			x0 = 0
			y0 = 0
			NH = N_HORZ
			NV = N_VERT
		else:
			x0 = (next_board % 3) * 3
			y0 = (next_board / 3) * 3
		for v in range(NV):
			for h in range(NH):
				if bd.is_empty(x0+h, y0+v):
					bd.put(x0+h, y0+v, next_color)
					var ev = bd.alpha_bata(alpha, beta, DEPTH)
					bd.unput()
					if next_color == MARU:
						if ev > alpha:
							alpha = ev
							ps = [x0+h, y0+v]
					else:
						if ev < beta:
							beta = ev
							ps = [x0+h, y0+v]
		return ps
	func select_pure_MC():		# 純粋モンテカルロ法による着手決定
		pass
	func eval_board():	# 現局面を（○から見た）評価
		var ev = 0
		for gy in range(3):
			var y0 = gy * 3
			for gx in range(3):
				var x0 = gx * 3
				var i3 = 0		# ＼対角線
				var i4 = 0		# ／対角線
				for h in range(N_HORZ/3):
					var ix = 0
					var i2 = 0
					for v in range(N_HORZ/3):
						ix = ix * 3 + get_color(x0 + h, y0 + v) + 1
						i2 = i2 * 3 + get_color(x0 + v, y0 + h) + 1
					ev += ev_pat_table[ix] + ev_pat_table[i2]
					i3 = i3 * 3 + get_color(x0 + h, y0 + h) + 1
					i4 = i4 * 3 + get_color(x0 + 2 - h, y0 + h) + 1
				ev += ev_pat_table[i3] + ev_pat_table[i4]
		var i3 = 0		# ＼対角線
		var i4 = 0		# ／対角線
		for g in range(3):
			var ix = 0
			var i2 = 0
			for h in range(3):
				ix = ix * 3 + get_color_g(g, h) + 1
				i2 = i2 * 3 + get_color_g(h, g) + 1
			ev += (ev_pat_table[ix] + ev_pat_table[i2]) * 32
			i3 = i3 * 3 + get_color_g(g, g) + 1
			i4 = i4 * 3 + get_color_g(2 - g, g) + 1
		ev += (ev_pat_table[i3] + ev_pat_table[i4]) * 32
		if next_board < 0:		# 全ボードに着手可能
			if next_color == MARU:
				ev += 16
			else:
				ev -= 16
		return ev
	func can_lined_up(gx: int, gy: int):		# グローバルボード gx, gy で三目作れるか？
		# 前提条件：NOT(でに三目並んでいる or 空きが無い) とする
		#print("can_lined_up(%d, %d)" % [gx, gy])
		var x0 = gx * 3
		var y0 = gy * 3
		var i3 = 0		# ＼対角線
		var i4 = 0		# ／対角線
		for h in range(N_HORZ/3):
			var ix = 0
			var i2 = 0
			for v in range(N_HORZ/3):
				ix = ix * 3 + get_color(x0 + h, y0 + v) + 1
				i2 = i2 * 3 + get_color(x0 + v, y0 + h) + 1
			if ev_put_table[ix] == 32 || ev_put_table[i2] == 32: return true
			i3 = i3 * 3 + get_color(x0 + h, y0 + h) + 1
			i4 = i4 * 3 + get_color(x0 + 2 - h, y0 + h) + 1
		return ev_put_table[i3] == 32 || ev_put_table[i4] == 32
	func eval_put(x: int, y: int):		# (x, y) への着手を評価
		if !is_empty(x, y): return -1
		var ev = 0
		var x3 = x % 3
		var y3 = y % 3
		if three_lined_up[x3 + y3*3] || n_put_local[x3 + y3*3] == 9:
			ev = -32		# 転送先がすでに三目並んでいる or 空きが無い
		elif can_lined_up(x3, y3):
			#print("*** can line up!")
			ev = -24		# 転送先で三目ができる場合
		var gx = x / 3
		var gy = y / 3
		if three_lined_up[gx + gy*3]: return 0
		var x0 = x - x % 3
		var y0 = y - y % 3
		var ix = 0
		for h in range(N_HORZ/3):			# 横方向
			ix = ix * 3 + get_color(x0 + h, y) + 1
		ev += ev_put_table[ix]
		ix = 0
		for v in range(N_HORZ/3):			# 縦方向
			ix = ix * 3 + get_color(x, y0 + v) + 1
		ev += ev_put_table[ix]
		if x3 == y3:					
			ix = 0
			for h in range(N_HORZ/3):		# ＼方向
				ix = ix * 3 + get_color(x0 + h, y0 + h) + 1
			ev += ev_put_table[ix]
		if x3 == (2 - y3):					
			ix = 0
			for h in range(N_HORZ/3):		# ／方向
				ix = ix * 3 + get_color(x0 + h, y0 + 2 - h) + 1
			ev += ev_put_table[ix]
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
	g_bd.print()
	print("eval_board = ", g_bd.eval_board())
	g_bd.put(0, 0, MARU)
	g_bd.print()
	print("eval_board = ", g_bd.eval_board())
	g_bd.put(1, 1, BATSU)
	g_bd.print()
	print("eval_board = %d\n" % g_bd.eval_board())
	g_bd.unput()
	g_bd.print()
	g_bd.unput()
	g_bd.print()
	#
	BOARD_ORG_X = $Board/TileMapLocal.global_position.x
	BOARD_ORG_Y = $Board/TileMapLocal.global_position.y
	BOARD_ORG = Vector2(BOARD_ORG_X, BOARD_ORG_Y)
	setup_player_option_button($MaruPlayer/OptionButton)
	setup_player_option_button($BatsuPlayer/OptionButton)
	$MaruPlayer/OptionButton.selected = maru_player
	$BatsuPlayer/OptionButton.selected = batsu_player
	#$MaruPlayer/Underline.visible = false
	#$BatsuPlayer/Underline.visible = false
	#rng.randomize()		# Setups a time-based seed
	rng.seed = 0			# 固定乱数系列
	init_board()
	update_next_label()
	#update_next_underline()
	#put(2, 2, MARU)
	$MessLabel.text = "【Start Game】を押してください。"
	print(Time.get_ticks_usec())
	pass # Replace with function body.
func setup_player_option_button(ob):
	ob.add_item(": Human", 0)	
	ob.add_item(": AI Random", 1)	
	ob.add_item(": AI Depth 1", 2)	# １手先読み
	ob.add_item(": AI Depth 3", 3)	# ３手先読み
func update_board_tilemaps():		# g_bd の状態から TileMap たちを設定
	for y in range(N_VERT):
		for x in range(N_HORZ):
			$Board/TileMapLocal.set_cell(x, y, g_bd.get_color(x, y))
			$Board/TileMapCursor.set_cell(x, y, 0 if g_bd.last_put_pos() == [x, y] else -1)
	var ix = 0
	for y in range(N_VERT/3):
		for x in range(N_HORZ/3):
			var c = -1 if g_bd.next_board >= 0 && ix != g_bd.next_board else NEXT_LOCAL_BOARD
			$Board/TileMapBG.set_cell(x, y, c)
			$Board/TileMapGlobal.set_cell(x, y, g_bd.get_color_g(x, y))
			ix += 1
	pass
func init_board():
	g_bd.init()
	n_put = 0
	put_pos = [-1, -1]
	n_put_board = [0, 0, 0, 0, 0, 0, 0, 0, 0]
	three_lined_up = [false, false, false, false, false, false, false, false, false]
	next_color = MARU
	#for y in range(N_VERT):
	#	for x in range(N_HORZ):
	#		$Board/TileMapLocal.set_cell(x, y, -1)
	#		$Board/TileMapCursor.set_cell(x, y, -1)
	#for y in range(N_VERT/3):
	#	for x in range(N_HORZ/3):
	#		$Board/TileMapBG.set_cell(x, y, NEXT_LOCAL_BOARD)
	#		$Board/TileMapGlobal.set_cell(x, y, -1)
	update_board_tilemaps()		# g_bd の状態から TileMap たちを設定
	pass
func update_next_label():
	if game_started:
		$MessLabel.text = "次は%sの手番です。" % ("Ｏ" if next_color == MARU else "Ｘ")
	update_next_underline()
func update_next_underline():
	$MaruPlayer/Underline.visible = game_started && next_color == MARU
	$BatsuPlayer/Underline.visible = game_started && next_color == BATSU
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
	g_bd.put(x, y, next_color)
	put(x, y, next_color)
	var gx = int(x) / 3
	var gy = int(y) / 3
	if $Board/TileMapGlobal.get_cell(gx, gy) == -1 && is_three_stones(x, y):
		# ローカルボード内で三目並んだ場合
		three_lined_up[gx + gy*3] = true
		$Board/TileMapGlobal.set_cell(gx, gy, next_color)
		if is_three_stones_global(gx, gy):
			print("*** Game Over")
			#game_started = false
			$MessLabel.text = mb_str[next_color] + " won."		# 
			do_game_over()
			return true;		# ゲームオーバー
	if n_put_board[next_board] == 9 || three_lined_up[next_board]:	# 空欄が無い or すでに三目並んでいる
		next_board = -1
		for gy2 in range(N_VERT/3):
			for gx2 in range(N_HORZ/3):
				$Board/TileMapBG.set_cell(gx2, gy2, NEXT_LOCAL_BOARD)
	next_color = (MARU + BATSU) - next_color
	update_next_label()
	#update_next_underline()
	update_board_tilemaps()
	return false
func _process(delta):
	if waiting > 0:
		waiting -= 1
	elif( game_started && !AI_thinking &&
			(next_color == MARU && maru_player >= AI_RANDOM || next_color == BATSU && batsu_player >= AI_RANDOM) ):
		#if !game_started:
		#	print("??? game_started = ", game_started)
		AI_thinking = true
		#var pos = AI_think_random()
		var typ = maru_player if next_color == MARU else batsu_player
		var pos = (g_bd.select_random() if typ == AI_RANDOM else
					g_bd.select_depth_1() if typ == AI_DEPTH_1 else
					g_bd.select_depth_3())
		#print("game_started = ", game_started)
		print("AI put ", pos)
		if !is_empty(pos[0], pos[1]):
			pos = (g_bd.select_random() if typ == AI_RANDOM else
					g_bd.select_depth_1() if typ == AI_DEPTH_1 else
					g_bd.select_depth_3())
		assert(is_empty(pos[0], pos[1]))
		put_and_post_proc(pos[0], pos[1])
		waiting = WAIT
		AI_thinking = false
	pass
func _input(event):
	if !game_started: return
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
	game_started = false
	$MaruPlayer/OptionButton.disabled = false
	$BatsuPlayer/OptionButton.disabled = false
	$StartStopButton.text = "Start Game"
	update_board_tilemaps()
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
	update_next_underline()
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
	#g_bd.change_turn()
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
