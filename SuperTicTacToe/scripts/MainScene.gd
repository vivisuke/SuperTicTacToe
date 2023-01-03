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
var rng = RandomNumberGenerator.new()

func _ready():
	BOARD_ORG_X = $Board/TileMapLocal.global_position.x
	BOARD_ORG_Y = $Board/TileMapLocal.global_position.y
	BOARD_ORG = Vector2(BOARD_ORG_X, BOARD_ORG_Y)
	setup_player_option_button($MaruPlayer/OptionButton)
	setup_player_option_button($BatsuPlayer/OptionButton)
	rng.randomize()
	init_board()
	update_next_label()
	#put(2, 2, MARU)
	$MessLabel.text = "【Start Game】を押してください。"
	pass # Replace with function body.
func setup_player_option_button(ob):
	ob.add_item(": Human", 0)	
	ob.add_item(": Random", 1)	
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
func is_game_over(gx : int, gy : int):		# グローバルボードで三目並んだか？
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
	else:
		var x0 = (next_board % 3) * 3
		var y0 = (next_board / 3) * 3
		#return [x0 + rng.randi_range(0, 2), y0 + rng.randi_range(0, 2)]
		var lst = []
		for v in range(3):
			for h in range(3):
				if is_empty(x0+h, y0+v): lst.push_back([x0+h, y0+v])
		return lst[rng.randi_range(0, lst.size() - 1)]
func put_and_post_proc(x : int, y : int):	# 着手処理とその後処理
	put(x, y, next_color)
	var gx = int(x) / 3
	var gy = int(y) / 3
	if $Board/TileMapGlobal.get_cell(gx, gy) == -1 && is_three_stones(x, y):
		# ローカルボード内で三目並んだ場合
		three_lined_up[gx + gy*3] = true
		$Board/TileMapGlobal.set_cell(gx, gy, next_color)
		if is_game_over(gx, gy):
			game_started = false
			$MessLabel.text = "Human won."		# undone: AI が勝った場合対応
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
			next_color == MARU && maru_player == AI || next_color == BATSU && batsu_player == AI):
		AI_thinking = true
		var pos = AI_think_random()
		print("AI put ", pos)
		put_and_post_proc(pos[0], pos[1])
		waiting = WAIT*10
		AI_thinking = false
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


func _on_StartStopButton_pressed():
	game_started = !game_started
	if game_started:
		$MaruPlayer/OptionButton.disabled = true
		$BatsuPlayer/OptionButton.disabled = true
		init_board()
		$StartStopButton.text = "Stop Game"
		$MessLabel.text = "次の手番はＯです。"
	else:
		$MaruPlayer/OptionButton.disabled = false
		$BatsuPlayer/OptionButton.disabled = false
		$StartStopButton.text = "Start Game"
		$MessLabel.text = ""
	pass # Replace with function body.


func _on_MaruOptionButton_item_selected(index):
	maru_player = index
	pass # Replace with function body.


func _on_BatsuOptionButton_item_selected(index):
	batsu_player = index
	pass # Replace with function body.
