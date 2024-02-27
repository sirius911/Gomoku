import tkinter as tk
from tkinter import messagebox
from constants import *
from dialogs import EndGameDialog, CustomDialog
from game_logic import GomokuLogic

class GomokuGUI:
    def __init__(self, master, game_logic, size=19, cell_size=30, margin=20):
        self.master = master
        self.game_logic = game_logic
        self.size = size
        self.cell_size = cell_size
        self.margin = margin
        self.canvas = tk.Canvas(master, width=self.pixel_size, height=self.pixel_size)
        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.pack()
        self.captures_labels = {
            "black": tk.Label(master, text="Captures Black: 0"),
            "white": tk.Label(master, text="Captures White: 0")
        }
        self.captures_labels["black"].pack()
        self.captures_labels["white"].pack()
        self.end_game_dialog = None

    @property
    def pixel_size(self):
        return self.size * self.cell_size + self.margin * 2

    def on_canvas_click(self, event):
        x, y = self.convert_pixel_to_grid(event.x, event.y)
        status = self.game_logic.play(x, y)
        if status == INVALID_MOVE or status == FORBIDDEN_MOVE:
            CustomDialog(parent=self.master, title='Invalide Move',message=status['message'], alert_type=status['alert_type'])
        else:
            self.draw_stones()
            if status == WIN_GAME:
                self.end_game_dialog = EndGameDialog(self.master, f"{self.game_logic.current_player} a gagné ! Voulez-vous rejouer ?", self.replay_game, self.quit_game)
            else:
                self.draw_stones()
                self.update_captures_display(self.game_logic.captures)
                self.game_logic.switch_player()

    def draw_board(self):
        for i in range(self.size):
            x = self.margin + i * self.cell_size
            self.canvas.create_line(x, self.margin, x, self.margin + self.cell_size * (self.size - 1))
            y = self.margin + i * self.cell_size
            self.canvas.create_line(self.margin, y, self.margin + self.cell_size * (self.size - 1), y)

    def draw_stones(self):
        self.canvas.delete("stone")  # Remove existing stones
        for (x, y), color in self.game_logic.board.items():
            self.draw_stone(x, y, color)

    def draw_stone(self, x, y, color):
        radius = self.cell_size // 2 - 2
        center_x = self.margin + x * self.cell_size
        center_y = self.margin + y * self.cell_size
        self.canvas.create_oval(center_x - radius, center_y - radius, center_x + radius, center_y + radius, fill=color, tags="stone")

    def convert_pixel_to_grid(self, pixel_x, pixel_y):
        grid_x = (pixel_x - self.margin + self.cell_size // 2) // self.cell_size
        grid_y = (pixel_y - self.margin + self.cell_size // 2) // self.cell_size
        return grid_x, grid_y
    
    def update_captures_display(self, captures):
        self.captures_labels["black"].config(text=f"Captures Black: {captures['black']}")
        self.captures_labels["white"].config(text=f"Captures White: {captures['white']}")


    def show_winner(self, winner):
        messagebox.showinfo("Game Over", f"{winner} wins!")

    def show_invalide(self, text="Invalide Move",type = "warning"):
        if type == "warning":
            messagebox.showwarning("Invalide Move", text)
        elif type == "error":
            messagebox.showerror("Invalide Move", text)
        else:
            messagebox.INFO("Info", text)

    def replay_game(self):
        if self.end_game_dialog:
            self.end_game_dialog.destroy()
            self.end_game_dialog = None
        # Réinitialiser la logique de jeu
        self.game_logic = GomokuLogic()

        # Effacer le plateau de jeu dans l'interface graphique
        self.canvas.delete("all")

        # Mettre à jour l'affichage des captures, scores, etc. si nécessaire
        self.update_captures_display({ "black": 0, "white": 0 })
        # self.update_captures_display(self.game_logic.captures)

        

         # Redessiner le plateau de jeu
        self.draw_board()

    def quit_game(self):
        self.master.destroy()
