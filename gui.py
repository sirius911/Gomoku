import tkinter as tk
from tkinter import messagebox
from constants import *
from dialogs import EndGameDialog, CustomDialog
from game_logic import GomokuLogic
from tkinter import filedialog
from constants import VERSION

class GomokuGUI:
    def __init__(self, master, game_logic, size=19, cell_size=30, margin=20, top_margin = 50):
        self.master = master
        self.master.geometry("584x670")
        self.game_logic = game_logic
        self.size = size
        self.cell_size = cell_size
        self.margin = margin
        self.top_margin = top_margin
        self.canvas = tk.Canvas(master, width=self.pixel_size, height=self.pixel_size+100)
        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.pack()
        self.captures_labels = {
            "black": tk.Label(master, text="Captures Black: 0"),
            "white": tk.Label(master, text="Captures White: 0")
        }
        self.captures_labels["black"].pack()
        self.captures_labels["white"].pack()
        self.end_game_dialog = None
        self.draw_current_player_indicator()
        self.update_captures_display(self.game_logic.captures)

        # Créer une barre de menu
        menu_bar = tk.Menu(self.master)

        # Créer un menu "Fichier"
        file_menu = tk.Menu(menu_bar, tearoff=0)
        file_menu.add_cascade(label="Nouvelle Partie", command=self.replay_game)
        file_menu.add_separator()
        file_menu.add_command(label="Sauvegarder", command=self.save_game)
        file_menu.add_command(label="Charger", command=self.load_game)
        file_menu.add_separator()
        file_menu.add_command(label="Quitter", command=self.quit_game)

        # Créer un menu "IA"
        self.ia_black_var = tk.IntVar()
        self.ia_white_var = tk.IntVar()
        ia_menu = tk.Menu(menu_bar, tearoff=0)
        # Ajouter des options à cocher pour "Black" et "White"
        ia_menu.add_checkbutton(label="Black", variable=self.ia_black_var, command=self.toggle_ia_color)
        ia_menu.add_checkbutton(label="White", variable=self.ia_white_var, command=self.toggle_ia_color)

        #version
        version_menu = tk.Menu(self.master)
        version_menu.add_command(label="Version", command=self.version)

        # Ajouter les menus
        menu_bar.add_cascade(label="Fichier", menu=file_menu)
        menu_bar.add_cascade(label="IA", menu=ia_menu)
        menu_bar.add_cascade(label="info", menu=version_menu)

        # Configurer la fenêtre principale pour utiliser cette barre de menu
        self.master.config(menu=menu_bar)

        #self.master.bind("<Configure>", self.on_window_resize)



    @property
    def pixel_size(self):
        return self.size * self.cell_size + self.margin * 2

    # def on_window_resize(self, event):
    #     # Imprime la largeur et la hauteur actuelles de la fenêtre
    #     print(f"Taille actuelle de la fenêtre : {event.width}x{event.height}")


    def on_canvas_click(self, event):
        x, y = self.convert_pixel_to_grid(event.x, event.y)
        status = self.game_logic.play(x, y)
        if status == INVALID_MOVE or status == FORBIDDEN_MOVE:
            CustomDialog(parent=self.master, title='Invalide Move',message=status['message'], alert_type=status['alert_type'])
        else:
            self.draw_stones()
            if status == WIN_GAME:
                win = self.game_logic.current_player
                if win == self.game_logic.ia:
                    win += " (IA)"
                self.end_game_dialog = EndGameDialog(self.master, f"{win} a gagné ! Voulez-vous rejouer ?", self.replay_game, self.quit_game)
            else:
                # self.draw_stones()
                self.update_captures_display(self.game_logic.captures)
                self.game_logic.switch_player()
                self.draw_current_player_indicator()
                if self.is_IA_turn():
                    self.ia_play()

    def ia_play(self):
        status = CONTINUE_GAME
        while  status == CONTINUE_GAME and self.is_IA_turn():
            status = self.game_logic.play_IA()
            self.draw_stones()
            self.update_captures_display(self.game_logic.captures)
            self.draw_current_player_indicator()
            self.master.after(100, self.ia_play)
        # TODO sortie

    def is_IA_turn(self):
        return (self.game_logic.IA_Turn())

    def draw_board(self):
        for i in range(self.size):
            x = self.margin + i * self.cell_size
            # Utiliser self.top_margin pour la position verticale
            self.canvas.create_line(x, self.top_margin, x, self.top_margin + self.cell_size * (self.size - 1))
            y = self.top_margin + i * self.cell_size
            # Utiliser self.top_margin pour la position verticale
            self.canvas.create_line(self.margin, y, self.margin + self.cell_size * (self.size - 1), y)

    def draw_stones(self):
        self.canvas.delete("stone")  # Remove existing stones
        for (x, y), color in self.game_logic.board.items():
            self.draw_stone(x, y, color)
        self.canvas.update()

    def draw_stone(self, x, y, color):
        radius = self.cell_size // 2 - 2
        center_x = self.margin + x * self.cell_size
        # Utiliser self.top_margin pour ajuster la position verticale des pierres
        center_y = self.top_margin + y * self.cell_size
        self.canvas.create_oval(center_x - radius, center_y - radius, center_x + radius, center_y + radius, fill=color, tags="stone")

    def draw_current_player_indicator(self):
        # Assurez-vous d'effacer l'indicateur précédent
        self.canvas.delete("current_player_indicator")

        # Position pour le texte et le cercle
        text_x = self.margin // 4
        text_y = self.margin // 2
        indicator_x = (self.margin // 2 + self.cell_size // 4) + 100
        indicator_y = self.margin // 2
        radius = self.cell_size // 4  # Taille de l'indicateur

        # Déterminez la couleur du joueur actuel
        color = self.game_logic.current_player

        # Dessinez le texte "Player: "
        texte = "Player's Turn "
        if self.is_IA_turn():
            texte += "(IA)"
        self.canvas.create_text(text_x, text_y, text=texte, anchor="w", tags="current_player_indicator")

        # Dessinez un cercle pour indiquer le joueur actuel
        self.canvas.create_oval(indicator_x - radius, indicator_y - radius, indicator_x + radius, indicator_y + radius, fill=color, tags="current_player_indicator")
        self.canvas.update()

    def convert_pixel_to_grid(self, pixel_x, pixel_y):
        grid_x = (pixel_x - self.margin + self.cell_size // 2) // self.cell_size
        # Utilisez self.top_margin au lieu de self.margin pour la coordonnée y
        grid_y = (pixel_y - self.top_margin + self.cell_size // 2) // self.cell_size
        grid_x = max(0, min(self.size - 1, grid_x))  # Assurez-vous que x est dans la grille
        grid_y = max(0, min(self.size - 1, grid_y))  # Assurez-vous que y est dans la grille
        return grid_x, grid_y


    def update_captures_display(self, captures):
        # Position pour le texte et le cercle des captures
        text_x = 100  # Ajustez selon vos besoins
        canvas_height = self.canvas.winfo_height()  # Obtenir la hauteur actuelle du canvas

        # Calculer la position y dynamiquement en fonction de la hauteur du canvas
        black_y = canvas_height - 50  # Décalage pour 'black'
        white_y = canvas_height - 20   # Décalage pour 'white'
        radius = 10  # Taille des cercles

        # Dessinez les cercles de capture
        self.canvas.create_oval(text_x, black_y - radius, text_x + 2*radius, black_y + radius, fill="black", tags="capture_indicator")
        self.canvas.create_oval(text_x, white_y - radius, text_x + 2*radius, white_y + radius, fill="white", tags="capture_indicator")

        # Mettez à jour le texte des labels pour inclure le nombre de captures
        self.captures_labels["black"].config(text=f"Captures      {captures['black']}")
        self.captures_labels["white"].config(text=f"Captures      {captures['white']}")

        # Ajuster la position des labels dynamiquement
        self.captures_labels["black"].place(x=text_x + 3*radius, y=black_y - radius)
        self.captures_labels["white"].place(x=text_x + 3*radius, y=white_y - radius)
        self.canvas.update()


    def replay_game(self):
        if self.end_game_dialog:
            self.end_game_dialog.destroy()
            self.end_game_dialog = None
        # Réinitialiser la logique de jeu
        self.game_logic = GomokuLogic(ia=self.game_logic.ia)

        # Effacer le plateau de jeu dans l'interface graphique
        self.canvas.delete("all")

        # Mettre à jour l'affichage des captures, scores, etc. si nécessaire
        self.update_captures_display({ "black": 0, "white": 0 })
        # self.update_captures_display(self.game_logic.captures)

         # Redessiner le plateau de jeu
        self.draw_board()

    def quit_game(self):
        self.master.destroy()

    def save_game(self):
        filepath = filedialog.asksaveasfilename(
            initialdir='parties/',  # Répertoire initial
            defaultextension=".gom",
            filetypes=[("Gomoku files", "*.gom"), ("All files", "*.*")])
        if filepath:
            self.game_logic.save(filepath)

    def load_game(self):
        filepath = filedialog.askopenfilename(
            initialdir='parties/',  # Répertoire initial
            filetypes=[("Gomoku files", "*.gom"), ("All files", "*.*")])
        if filepath:
            if self.game_logic.load(filepath):
                self.change_title()
                self.draw_board()  # Redessiner le plateau de jeu après le chargement
                self.draw_stones()  # Redessiner les pierres après le chargement
                self.update_captures_display(self.game_logic.captures)
                if self.is_IA_turn():
                    self.ia_play()
            else:
                CustomDialog(parent=self.master, title='Bad File',message='bad file', alert_type='error')

    def version(self):
        title = f"Gomoku {VERSION}"
        message = f"{title}\n@ clorin@student.42.fr"
        CustomDialog(parent=self.master, title=title,message=message, alert_type='info')

    def change_title(self):
        title = "Gomoku "
        if self.game_logic.ia['black']:
            title += "IA"
        else:
            title += "Humain"
        title += " vs "
        if self.game_logic.ia['white']:
            title += "IA"
        else:
            title += "Humain"
        self.master.title(title)

    def toggle_ia_color(self):       
        # Mettre à jour la couleur de l'IA dans la logique du jeu
        self.game_logic.ia['black'] = (self.ia_black_var.get() == 1)
        self.game_logic.ia['white'] = (self.ia_white_var.get() == 1)
        self.change_title()
        if self.is_IA_turn():
            self.ia_play()