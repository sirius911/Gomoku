import tkinter as tk
from constants import *
from dialogs import EndGameDialog, CustomDialog
from game_logic import GomokuLogic
from tkinter import filedialog

class GomokuGUI:
    def __init__(self, master, game_logic, size=19, cell_size=30, margin=20, top_margin = 50):
        self.master = master
        self.master.geometry("584x670")
        self.master.resizable(False, False)
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

        # Créer une barre de menu
        menu_bar = tk.Menu(self.master, tearoff=0)

        # Créer un menu "Fichier"
        file_menu = tk.Menu(menu_bar, tearoff=0)
        file_menu.add_command(label="Nouvelle Partie", command=self.new_partie)
        file_menu.add_separator()
        file_menu.add_command(label="Sauvegarder (Ctrl+S)", command=self.save_game)
        file_menu.add_command(label="Charger", command=self.load_game)
        file_menu.add_separator()
        file_menu.add_command(label="Quitter (Ctrl+C)", command=self.quit_game)

        #coups
        self.coups_menu = tk.Menu(menu_bar, tearoff=0)
        self.coups_menu.add_command(label="Undo (Ctrl+Z)", command=self.undo)
        self.coups_menu.entryconfig("Undo (Ctrl+Z)", state="disabled")
        self.edition = tk.BooleanVar()
        self.edition.set(False)
        self.coups_menu.add_checkbutton(label="Edition (Ctrl-E)", variable=self.edition, command=self.manual)
        
        # Créer un menu "IA"
        self.ia_black_var = tk.IntVar()
        self.ia_white_var = tk.IntVar()
        ia_menu = tk.Menu(menu_bar, tearoff=0)
        # Ajouter des options à cocher pour "Black" et "White"
        ia_menu.add_checkbutton(label="Black", variable=self.ia_black_var, command=self.toggle_ia_color)
        ia_menu.add_checkbutton(label="White", variable=self.ia_white_var, command=self.toggle_ia_color)
        ia_menu.add_separator()
        self.ia_level_var = tk.IntVar()
        self.ia_level_var.set(1)
        ia_menu.add_radiobutton(label="Level 1", variable=self.ia_level_var, value=1, command=self.toggle_ia_level)
        ia_menu.add_radiobutton(label="Level 2", variable=self.ia_level_var, value=2, command=self.toggle_ia_level)
        ia_menu.add_radiobutton(label="Level 3", variable=self.ia_level_var, value=3, command=self.toggle_ia_level)
        
        #info
        self.debug = tk.BooleanVar()
        info_menu = tk.Menu(self.master, tearoff=0)
        info_menu.add_command(label="Version", command=self.version)
        info_menu.add_checkbutton(label="Debug", variable=self.debug, command=self.toggle_debug)
        
        # Ajouter les menus
        menu_bar.add_cascade(label="Fichier", menu=file_menu)
        menu_bar.add_cascade(label="Coups", menu=self.coups_menu)
        menu_bar.add_cascade(label="IA", menu=ia_menu)
        menu_bar.add_cascade(label="info", menu=info_menu)

        # Configurer la fenêtre principale pour utiliser cette barre de menu
        self.master.config(menu=menu_bar)

        # pour debug taille fenetre
        #self.master.bind("<Configure>", self.on_window_resize)

        self.path = None
        self.saved = False

        # Bind keyboard shortcuts
        self.master.bind('<Control-z>', lambda event: self.undo())
        self.master.bind('<Control-c>', lambda event: self.quit_game())
        self.master.bind('<Control-s>', lambda event: self.save_game())
        self.master.bind('<Control-e>', lambda event: self.switch_edition())
        self.master.bind('<h>', lambda event: self.help())
        self.master.bind('<b>', lambda event: self.change_color('black'))
        self.master.bind('<w>', lambda event: self.change_color('white')
                         )
        # Créer et positionner le label du chronomètre
        self.timer_label = tk.Label(master, text="0.00s")
        self.timer_label.place(relx=1.0, rely=1.0, x=-10, y=-10, anchor="se")

        self.draw_current_player_indicator()
        self.update_captures_display(self.game_logic.captures)

        # Ajout du label pour afficher les coordonnées de la souris
        self.mouse_coords_label = tk.Label(master, text="ici")
        self.mouse_coords_label.place(relx=0.0, rely=1.0, x=10, y=-10, anchor="sw")

        # Liez l'événement de mouvement de la souris sur le canvas à la fonction update_mouse_coords
        self.canvas.bind("<Motion>", self.update_mouse_coords)

    @property
    def pixel_size(self):
        return self.size * self.cell_size + self.margin * 2

    # def on_window_resize(self, event):
    #     # Imprime la largeur et la hauteur actuelles de la fenêtre
    #     print(f"Taille actuelle de la fenêtre : {event.width}x{event.height}")


    def on_canvas_click(self, event):
        x, y = self.convert_pixel_to_grid(event.x, event.y)
        if not self.edition.get():
            status = self.game_logic.play(x, y)
            if status == INVALID_MOVE or status == FORBIDDEN_MOVE:
                if status == FORBIDDEN_MOVE:
                    CustomDialog(parent=self.master, title='Invalide Move',message=status['message'], alert_type=status['alert_type'])
            else:
                self.saved = False
                self.draw_stones()
                if status == WIN_GAME:
                    win = self.game_logic.current_player
                    if win == self.game_logic.ia:
                        win += " (IA)"
                    self.end_game_dialog = EndGameDialog(self.master, f"{win} a gagné !\nVoulez-vous rejouer ?", self.replay_game, self.quit_game)
                else:
                    # self.draw_stones()
                    self.update_captures_display(self.game_logic.captures)
                    self.game_logic.switch_player()
                    self.draw_current_player_indicator()
                    if self.is_IA_turn():
                        self.ia_play()
        else:
            self.game_logic.manual_play(x, y)
            self.saved = False
            self.draw_stones()

    def ia_play(self):
        status = CONTINUE_GAME
        while  status == CONTINUE_GAME and self.is_IA_turn():
            self.saved = False
            status,play_time = self.game_logic.play_IA()
            self.draw_stones()
            self.update_captures_display(self.game_logic.captures)
            self.draw_current_player_indicator()
            self.timer_label.config(text=f"{play_time:.2f}s")
            if status == WIN_GAME:
                win = self.game_logic.current_player
                if win == self.game_logic.ia:
                    win += " (IA)"
                self.end_game_dialog = EndGameDialog(self.master, f"{win} a gagné ! Voulez-vous rejouer ?", self.replay_game, self.quit_game)

    def is_IA_turn(self):
        return (self.game_logic.IA_Turn() and not self.edition.get())

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
        self.update_title()
        self.update_undo_menu()
        self.canvas.update()

    def draw_stone(self, x, y, color, tag="stone"):
        radius = self.cell_size // 2 - 2
        center_x = self.margin + x * self.cell_size
        # Utiliser self.top_margin pour ajuster la position verticale des pierres
        center_y = self.top_margin + y * self.cell_size
        self.canvas.create_oval(center_x - radius, center_y - radius, center_x + radius, center_y + radius, fill=color, tags=tag)

    def blink_stone(self, x, y, color):
        self.draw_stone(x,y,color,"blink")
        self.canvas.after(500, lambda: [
            self.canvas.delete("blink")
        ])
        self.canvas.delete("stone")
        self.draw_stones()

    def help(self):
        x,y = self.game_logic.help_IA()
        self.blink_stone(x,y, self.game_logic.current_player)

    def draw_current_player_indicator(self):
        # Assurez-vous d'effacer l'indicateur précédent
        self.canvas.delete("current_player_indicator")

        # Position pour le texte et le cercle
        text_x = self.margin // 4
        text_y = self.margin // 2
        indicator_x = (self.margin // 2 + self.cell_size // 4) + 110
        indicator_y = self.margin // 2
        radius = self.cell_size // 4  # Taille de l'indicateur

        # Déterminez la couleur du joueur actuel
        color = self.game_logic.current_player

        # Le texte "Player: "
        if self.edition.get():
            texte = "Edition for "
        else:
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

    def new_partie(self):
        self.game_logic = GomokuLogic(ia=self.game_logic.ia, debug=self.game_logic.debug, ia_level=self.game_logic.ia_level)
        self.path = None
        # Effacer le plateau de jeu dans l'interface graphique
        self.canvas.delete("all")

        # Mettre à jour l'affichage des captures, scores, etc. si nécessaire
        self.update_captures_display({ "black": 0, "white": 0 })
        # self.update_captures_display(self.game_logic.captures)

         # Redessiner le plateau de jeu
        self.draw_board()
        self.draw_current_player_indicator()
        self.update_title()

    def replay_game(self):
        if self.end_game_dialog:
            self.end_game_dialog.destroy()
            self.end_game_dialog = None
        # Réinitialiser la logique de jeu
        self.game_logic = GomokuLogic(ia=self.game_logic.ia, debug=self.game_logic.debug, ia_level=self.game_logic.ia_level)
        # Effacer le plateau de jeu dans l'interface graphique
        self.canvas.delete("all")
        if self.path is not None:
            self.load_game(self.path)
        else:
            # Mettre à jour l'affichage des captures, scores, etc. si nécessaire
            self.update_captures_display({ "black": 0, "white": 0 })
            # self.update_captures_display(self.game_logic.captures)

            # Redessiner le plateau de jeu
            self.draw_board()
            self.draw_current_player_indicator()

    def quit_game(self):
        self.master.destroy()

    def save_game(self):
        filepath = self.path
        if filepath is None:
            filepath = filedialog.asksaveasfilename(
                initialdir='parties/',  # Répertoire initial
                defaultextension=".gom",
                filetypes=[("Gomoku files", "*.gom"), ("All files", "*.*")])
        
        if filepath:
            self.game_logic.save(filepath)
            self.saved = True
            self.update_title()

    def load_game(self, filepath = None):
        if filepath is None:
            filepath = filedialog.askopenfilename(
                initialdir='parties/',  # Répertoire initial
                filetypes=[("Gomoku files", "*.gom"), ("All files", "*.*")])
        if filepath:
            if self.game_logic.load(filepath):
                self.saved = True
                self.path = filepath
                self.ia_level_var.set(self.game_logic.ia_level)
                
            else:
                CustomDialog(parent=self.master, title='Bad File',message='bad file', alert_type='error')
            self.update_title()
            self.update_menu()
            self.draw_board()  # Redessiner le plateau de jeu après le chargement
            self.draw_stones()  # Redessiner les pierres après le chargement
            self.update_captures_display(self.game_logic.captures)
            self.draw_current_player_indicator()

            if self.is_IA_turn():
                self.ia_play()

    def version(self):
        title = f"Gomoku {VERSION}"
        message = f"{title}\n@ clorin@student.42.fr"
        CustomDialog(parent=self.master, title=title,message=message, alert_type='info')

    def update_title(self):
        title = "Gomoku "
        if self.game_logic.ia['black']:
            title += f"IA({self.ia_level_var.get()})"
        else:
            title += "Humain"
        title += " vs "
        if self.game_logic.ia['white']:
            title += f"IA({self.ia_level_var.get()})"
        else:
            title += "Humain"

        if self.game_logic.debug:
            title += " (Debug)"
        if not self.saved:
            title += "*"
        self.master.title(title)

    def toggle_ia_color(self):       
        # Mettre à jour la couleur de l'IA dans la logique du jeu
        self.game_logic.ia['black'] = (self.ia_black_var.get() == 1)
        self.game_logic.ia['white'] = (self.ia_white_var.get() == 1)
        self.update_title()
        if self.is_IA_turn():
            self.ia_play()

    def toggle_debug(self):
        self.game_logic.debug = (self.debug.get() == 1)
        self.update_title()

    def update_menu(self):
        self.ia_black_var.set(1 if self.game_logic.ia['black'] else 0)
        self.ia_white_var.set(1 if self.game_logic.ia['white'] else 0)
        self.debug.set(1 if self.game_logic.debug else 0)

    def toggle_ia_level(self):
        self.game_logic.ia_level = self.ia_level_var.get()
        self.update_title()

    def undo(self):
        self.game_logic.undo_move()
        self.update_undo_menu()
        self.draw_stones()
        self.update_captures_display(self.game_logic.captures)
        self.draw_current_player_indicator()

    def update_undo_menu(self):
        if len(self.game_logic.history) > 0:
            self.coups_menu.entryconfig("Undo (Ctrl+Z)", state="normal")
        else:
            self.coups_menu.entryconfig("Undo (Ctrl+Z)", state="disabled")

    def change_color(self, color):
        if self.edition.get():
            self.game_logic.current_player = color
            self.draw_current_player_indicator()

    def switch_edition(self):
        self.edition.set(not self.edition.get())
        self.manual()

    def manual(self):
        # print(f"self.edition = {self.edition.get()}")
        # print(f"self.is_IA_turn = {self.is_IA_turn()}")
        self.draw_current_player_indicator()
        if not self.edition.get():
            if self.is_IA_turn():
                self.ia_play()

    def update_mouse_coords(self, event):
        # Convertissez les pixels en coordonnées de grille
        grid_x, grid_y = self.convert_pixel_to_grid(event.x, event.y)
        # Mettez à jour le label avec les coordonnées de la grille
        self.mouse_coords_label.config(text=f"X: {grid_x}, Y: {grid_y}")