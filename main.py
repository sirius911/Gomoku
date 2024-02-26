import tkinter as tk

def draw_board(canvas):
    board_size = 19
    margin = 20
    cell_size = 30
    for i in range(board_size):
        x = margin + i * cell_size
        canvas.create_line(x, margin, x, margin + cell_size * (board_size - 1))
        y = margin + i * cell_size
        canvas.create_line(margin, y, margin + cell_size * (board_size - 1), y)

def check_win(board, player, x, y):
    directions = [(1, 0), (0, 1), (1, 1), (1, -1)]
    for dx, dy in directions:
        count = 1
        i, j = x + dx, y + dy
        while 0 <= i < 19 and 0 <= j < 19 and board.get((i, j), {}).get('color') == player:
            count += 1
            i, j = i + dx, j + dy
        i, j = x - dx, y - dy
        while 0 <= i < 19 and 0 <= j < 19 and board.get((i, j), {}).get('color') == player:
            count += 1
            i, j = i - dx, j - dy
        if count >= 5:
            return True
    return False

def place_stone(canvas, board, x, y, player):
    margin = 20
    cell_size = 30
    radius = cell_size // 2 - 2
    center_x = margin + x * cell_size
    center_y = margin + y * cell_size
    stone_id = canvas.create_oval(center_x - radius, center_y - radius, center_x + radius, center_y + radius, fill=player)
    board[(x, y)] = {'color': player, 'id': stone_id}

def triple(board, x, y, direction):
    """
        return les stones capturés dans la direction demandé ou None
    """
    ret = []
    dx1 = direction[0]
    dx2 = direction[0] * 2
    dx3 = direction[0] * 3
    dy1 = direction[1]
    dy2 = direction[1] * 2
    dy3 = direction[1] * 3
    if (x, y) in board:
        if ((x + dx1, y + dy1)) in board:
            if ((x + dx2, y + dy2)) in board:
                if ((x + dx3, y + dy3)) in board:
                    ret = [board[x,y]['color'], board[x+dx1,y+dy1]['color'], board[x+dx2,y+dy2]['color'], board[x+dx3,y+dy3]['color']]
    if (len(ret) == 4):
        if (ret[0] == ret[3] and ret[1] == ret[2]):
            to_del1 = (x+dx1, y+dy1)
            to_del2 = (x+dx2, y+dy2)
            return (to_del1, to_del2) 
    return None

def check_capture(canvas, board, player, x, y, captures):
    captures_made = 0
    directions = [(1, 0),(-1, 0),(0, 1), (0, -1),(1, 1),(-1, -1),(-1, 1),(1, -1)]
    for dx, dy in directions:
        captured = triple(board, x, y, (dx,dy))
        if captured:
            stone1 = captured[0]
            stone2 = captured[1]
            print(f"Capture détectée à ({stone1[0]}, {stone1[1]}) et ({stone2[0]}, {stone2[1]})")
            canvas.delete(board[stone1]['id'])
            canvas.delete(board[stone2]['id'])
            del board[stone1]
            del board[stone2]
            captures[player] += 2
            captures_made += 2

    return captures_made


def check_sequence(board, player, x, y, dx, dy):
    """Vérifie une séquence de trois pierres dans une direction donnée."""
    count = 0
    for step in range(-3, 4):  # Vérifie 3 positions dans chaque direction à partir du point
        nx, ny = x + dx * step, y + dy * step
        if board.get((nx, ny), {}).get('color') == player:
            count += 1
            if count == 3:  # Trois pierres consécutives trouvées
                print("sequence de trois")
                return True
        else:
            count = 0  # Réinitialiser le compteur si la séquence est interrompue
    return False

def check_double_three(board, player, x, y):
    """Vérifie si le placement simulé crée un double-trois."""
    directions = [(1, 0), (0, 1), (1, 1), (1, -1)]
    # print(f"Check avec player = {player}, x = {x}, y = {y}")
    three_count = 0
    board[(x, y)] = {'color': player}  # Simule le placement de la pierre
    for dx, dy in directions:
        if check_sequence(board, player, x, y, dx, dy):
            three_count += 1
            if three_count > 1:
                del board[(x, y)]  # Supprime la simulation
                return True
    del board[(x, y)]  # Supprime la simulation si non double-trois
    return False


def show_winner(canvas, winner):
    canvas.create_text(400, 400, text=f"{winner.capitalize()} wins!", font=("Arial", 24), fill="red")

def on_resize(event):
    # Cette fonction est appelée chaque fois que la fenêtre est redimensionnée.
    print("Taille actuelle : {}x{}".format(event.width, event.height))

def main():
    window = tk.Tk()
    window.title("Gomoku Game")
    # window.bind("<Configure>", on_resize)
    window.geometry("757x646")
    # Empêcher le redimensionnement de la fenêtre
    window.resizable(False, False)
    # Configurez la colonne 0 (où se trouve le canevas) pour qu'elle prenne tout l'espace supplémentaire
    window.columnconfigure(0, weight=1)

    # Configurez la colonne 1 (où se trouvent les labels) pour qu'elle ne s'étende pas inutilement, ce qui peut aider à rapprocher les labels de la grille
    window.columnconfigure(1, weight=0)
    canvas = tk.Canvas(window, width=800, height=800)
    canvas.grid(row=0, column=0, rowspan=4)
    
    captures = {"black": 0, "white": 0}
    black_captures_label = tk.Label(window, text="Black captures pairs: 0 / 5")
    white_captures_label = tk.Label(window, text="White captures pairs: 0 / 5")
    black_captures_label.grid(row=0, column=1, sticky="w", padx=(10,0))
    white_captures_label.grid(row=1, column=1, sticky="w")
    
    board = {}
    current_player = ["black"]
    player_label = tk.Label(window, text=f"Current Player: {current_player[0].capitalize()}", font=("Arial", 16))
    player_label.grid(row=3, column=0, sticky="n")
    draw_board(canvas)

    def update_player_label():
        player_label.config(text=f"Current Player: {current_player[0].capitalize()}")

    def update_capture_labels():
        black_captures_label.config(text=f"Black captures pairs: {captures['black'] // 2} / 5")
        white_captures_label.config(text=f"White captures pairs: {captures['white'] // 2} / 5")

    def canvas_click_handler(event):
        margin = 20
        cell_size = 30
        grid_x = round((event.x - margin) / cell_size)
        grid_y = round((event.y - margin) / cell_size)
        
        if (grid_x, grid_y) not in board:
            if not check_double_three(board, current_player[0], grid_x, grid_y):
                place_stone(canvas, board, grid_x, grid_y, current_player[0])
                if check_capture(canvas, board, current_player[0], grid_x, grid_y, captures):
                    update_capture_labels()
                    if captures[current_player[0]] >= 10:
                        show_winner(canvas, current_player[0])
                        return
                if check_win(board, current_player[0], grid_x, grid_y):
                    show_winner(canvas, current_player[0])
                    return
                current_player[0] = "white" if current_player[0] == "black" else "black"
                update_player_label()
    canvas.bind("<Button-1>", canvas_click_handler)
    window.mainloop()

if __name__ == "__main__":
    main()
