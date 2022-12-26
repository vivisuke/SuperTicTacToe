extends ColorRect

const BD_WIDTH = 500
const N_HORZ = 9
const N_VERT = 9
const FRAME_WD = 20.0
const CELL_WD = (BD_WIDTH - FRAME_WD*2) / N_HORZ

func _ready():
	for x in range(N_HORZ):
		var l = Label.new()
		l.text = "abcdefghi"[x]
		l.set_position(Vector2(FRAME_WD-2+(x+0.5)*CELL_WD, 8))
		add_child(l)
	for y in range(N_VERT):
		var l = Label.new()
		l.text = "123456789"[y]
		#l.set_position(Vector2(8, 23+(y+0.5)*75))
		l.set_position(Vector2(8, FRAME_WD-2+(y+0.5)*CELL_WD))
		add_child(l)
	pass # Replace with function body.

func _draw():
	draw_rect(Rect2(0, 0, 500, 500), Color.black)
	draw_rect(Rect2(25, 25, 75*6, 75*6), Color.silver)
	for x in range(1, N_HORZ):
		var wd = 3 if x%3 == 0 else 1
		draw_line(Vector2(FRAME_WD+x*CELL_WD, FRAME_WD), 
					Vector2(FRAME_WD+x*CELL_WD, FRAME_WD+CELL_WD*N_VERT), 
					Color.black, wd)
		draw_line(Vector2(FRAME_WD, FRAME_WD+x*CELL_WD), 
					Vector2(FRAME_WD+CELL_WD*N_HORZ, FRAME_WD+x*CELL_WD), 
					Color.black, wd)
