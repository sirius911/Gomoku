import time
import tkinter as tk
from tkinter import messagebox


game_over = False
board = {}
captures = {"black": 0, "white": 0}
current_player = ["black"]
IA = ""

def draw_board(canvas):
    canvas.delete("all")
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

def capture(board, x, y, direction):
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
        if (ret[0] == ret[3] and ret[1] == ret[2] and ret[0] != ret[1]):
            to_del1 = (x+dx1, y+dy1)
            to_del2 = (x+dx2, y+dy2)
            return (to_del1, to_del2) 
    return None

def check_capture(canvas, board, player, x, y, captures):
    captures_made = 0
    directions = [(1, 0),(-1, 0),(0, 1), (0, -1),(1, 1),(-1, -1),(-1, 1),(1, -1)]
    for dx, dy in directions:
        captured = capture(board, x, y, (dx,dy))
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

def is_free(board, player, x, y, direction, length=6):

    dx, dy = direction
    sequence = []
    three_free=[[0,1,1,1,0,0],[0,1,1,1,0,1],[0,1,1,1,0,2],[0,1,0,1,1,0],[0,1,1,0,1,0]]
    for i in range(-1, length-1):  # Ajustez selon la longueur de la séquence que vous voulez vérifier
        nx, ny = x + dx * i, y + dy * i
        if 0 <= nx < 19 and 0 <= ny < 19:  # Assure que la position est dans les limites du plateau
            stone = board.get((nx, ny))
            if stone and stone['color'] == player:
                sequence.append(1)  # Pierres de la même couleur
            elif stone:
                sequence.append(2)  # Autre couleur
            else:
                sequence.append(0)  # Aucune pierre ou pierre de couleur différente
        else:
            sequence.append(0)  # Hors du plateau pour les positions hors limites
    return sequence in three_free
        

def check_double_three(board, player, x, y):
    directions = [(1, 0), (0, 1), (-1, 0), (0, -1),(1, 1),(-1, -1)]
    three_count = 0
    for d in directions:
        if is_free(board, player, x, y, d):
            three_count += 1
            if three_count > 1:
                canvas.delete(board[x,y]['id'])
                del board[x,y]
                return True
    return False

def restart_game():
    global board, captures, current_player, game_over
    board.clear()
    captures = {"black": 0, "white": 0}
    current_player = ["black"]
    game_over = False
    draw_board(canvas)
    update_player_label()
    update_capture_labels()

def show_double_three_dialog():
    dialog = tk.Toplevel(window)
    dialog.title("Invalid Move")
    dialog.geometry("300x100")  # Taille de la fenêtre
    # Positionnement au centre de la fenêtre principale
    window_x = window.winfo_x()
    window_y = window.winfo_y()
    window_width = window.winfo_width()
    window_height = window.winfo_height()
    dialog_x = window_x + (window_width - 300) // 2
    dialog_y = window_y + (window_height - 100) // 2
    dialog.geometry(f"+{dialog_x}+{dialog_y}")

    tk.Label(dialog, text="Double three detected. Move not allowed.", font=("Arial", 12)).pack(pady=10)

    # Bouton pour fermer la boîte de dialogue et continuer le jeu
    tk.Button(dialog, text="Continue", command=dialog.destroy).pack()

    dialog.transient(window)  # Rend la boîte de dialogue modale
    dialog.grab_set()  # Empêche l'interaction avec la fenêtre principale
    window.wait_window(dialog)  # Attend que la boîte de dialogue soit fermée


def show_custom_winner_dialog(winner):
    global game_over
    game_over = True
    dialog = tk.Toplevel(window)  # Crée une nouvelle fenêtre au-dessus de la fenêtre principale
    dialog.title("Partie terminée")
    dialog.geometry("300x100")  # Taille de la boîte de dialogue

    # Calcul pour centrer la boîte de dialogue par rapport à la fenêtre principale
    window_x = window.winfo_x()
    window_y = window.winfo_y()
    window_width = window.winfo_width()
    window_height = window.winfo_height()
    dialog_x = window_x + (window_width - 300) // 2
    dialog_y = window_y + (window_height - 100) // 2
    dialog.geometry(f"+{dialog_x}+{dialog_y}")

    tk.Label(dialog, text=f"{winner.capitalize()} a gagné! \nVoulez-vous jouer une nouvelle partie?", font=("Arial", 12)).pack(pady=10)
    tk.Button(dialog, text="Oui", command=lambda: [restart_game(), dialog.destroy()]).pack(side="left", padx=(50, 20), pady=10)
    tk.Button(dialog, text="Non", command=window.destroy).pack(side="right", padx=(20, 50), pady=10)

    dialog.transient(window)  # Indique que c'est une fenêtre secondaire
    dialog.grab_set()  # Empêche l'interaction avec la fenêtre principale tant que la boîte de dialogue est ouverte
    window.wait_window(dialog)  # Attend que la boîte de dialogue soit fermée


def update_player_label():
    player_label.config(text=f"Current Player: {current_player[0].capitalize()}")

def update_capture_labels():
    black_captures_label.config(text=f"Black captures pairs\n {captures['black'] // 2} / 5")
    white_captures_label.config(text=f"White captures pairs\n {captures['white'] // 2} / 5")

def on_resize(event):
    # Cette fonction est appelée chaque fois que la fenêtre est redimensionnée.
    print("Taille actuelle : {}x{}".format(event.width, event.height))

def ai_move(board, player):
    best_move = None
    # TODO
    return best_move



def play(grid_x, grid_y):
    if grid_x < 19 and grid_y < 19 and (grid_x, grid_y) not in board:
        place_stone(canvas, board, grid_x, grid_y, current_player[0])
        if not check_double_three(board, current_player[0], grid_x, grid_y):
            if check_capture(canvas, board, current_player[0], grid_x, grid_y, captures):
                update_capture_labels()
                if captures[current_player[0]] >= 10:
                    show_custom_winner_dialog(current_player[0])
                    return
            if check_win(board, current_player[0], grid_x, grid_y):
                show_custom_winner_dialog(current_player[0])
                return
            current_player[0] = "white" if current_player[0] == "black" else "black"
            update_player_label()
        else:
            show_double_three_dialog()

def canvas_click_handler(event):
    if game_over or current_player[0] == IA:  # Supposons que le joueur noir est humain et le blanc est l'IA:
        return
    margin = 20
    cell_size = 30
    grid_x = round((event.x - margin) / cell_size)
    grid_y = round((event.y - margin) / cell_size)
    play(grid_x, grid_y)
    
    if current_player[0] == IA:  # Tour de l'IA
        ai_x, ai_y = ai_move(board, "white")
        if ai_x is not None and ai_y is not None:
            place_stone(canvas, board, ai_x, ai_y, current_player[0])

def main():
    global window, canvas, player_label, black_captures_label, white_captures_label
    window = tk.Tk()
    window.title("Gomoku Game")
    # window.bind("<Configure>", on_resize)
    window.geometry("757x646")
    window.resizable(False, False)

    canvas = tk.Canvas(window, width=600, height=600)
    canvas.grid(row=0, column=0, rowspan=4)
    canvas.bind("<Button-1>", canvas_click_handler)

    black_captures_label = tk.Label(window, text="Black captures pairs\n 0 / 5")
    white_captures_label = tk.Label(window, text="White captures pairs\n 0 / 5")
    black_captures_label.grid(row=0, column=1, sticky="w", padx=(10,0))
    white_captures_label.grid(row=1, column=1, sticky="w")

    player_label = tk.Label(window, text="Current Player: Black", font=("Arial", 16))
    player_label.grid(row=4, column=0, sticky="n")

    draw_board(canvas)
    window.mainloop()

if __name__ == "__main__":
    main()