import tkinter as tk
from tkinter import scrolledtext

HELP_WINDOW_OPEN = False

# def on_window_resize( event):
#     # Imprime la largeur et la hauteur actuelles de la fenêtre
#     print(f"Taille actuelle de la fenêtre : {event.width}x{event.height}

def on_help_window_close(window):
    global HELP_WINDOW_OPEN
    HELP_WINDOW_OPEN = False
    window.destroy()

def display_help():
    global HELP_WINDOW_OPEN

    if HELP_WINDOW_OPEN:
        return
    
    help_window = tk.Toplevel()
    help_window.title("Guide du Jeu Gomoku")
    help_window.geometry("1072x797")
    help_window.resizable(False, False)

    # changer l'état lorsque la fenêtre d'aide est ouverte
    HELP_WINDOW_OPEN = True

    # Lorsque la fenêtre est fermée, mettre à jour la variable
    help_window.protocol("WM_DELETE_WINDOW", lambda: on_help_window_close(help_window))

    text_area = scrolledtext.ScrolledText(help_window, wrap=tk.WORD, font=("Arial", 12))
    text_area.pack(fill="both", expand=True)

    # pour debug taille fenetre
    # root.bind("<Configure>", on_window_resize)

    content = [
        ("Guide du Jeu Gomoku ", "title"),
        ("Introduction", "subtitle"),
        ("Gomoku est un jeu de plateau traditionnel où deux joueurs placent alternativement des pierres noires et blanches sur une grille de 19x19 dans le but d'aligner cinq pierres de suite horizontalement, verticalement, ou en diagonale.", "normal"),
        ("",""),
        ("Règles du Jeu", "subtitle"),
        ("",""),
        ("    **1. Configuration du Plateau :** La grille du jeu est composée de 19x19 intersections.", "list"),
        ("    **2. Jouer au Jeu :** Les joueurs placent à tour de rôle une pierre de leur couleur sur une intersection vide. Le joueur avec les pierres noires commence généralement.", "list"),
        ("    **3. Gagner la Partie :** Un joueur gagne en :", "list"),
        ("        - Alignant cinq pierres de suite.", "sublist"),
        ("        - Capturant un total de 10 pierres adverses.", "sublist"),
        ("    **4. Coups Interdits :** Le jeu inclut une règle contre les \"Doubles-Trois\", où un coup qui forme simultanément deux lignes de trois pierres est interdit sauf s'il contribue directement à faire un cinq de suite.", "list"),
        ("    **5. Captures :** Les joueurs peuvent retirer une paire de pierres adverses du plateau en les encadrant des deux côtés avec leurs propres pierres.", "list"),
        ("",""),
        ("Interface Utilisateur", "subtitle"),
        ("",""),
        ("Menus et Options", "subsubtitle"),
        ("",""),
        ("    - **Menu Fichier**", "list"),
        ("       - **Nouvelle Partie :** Commence une nouvelle partie.", "sublist"),
        ("       - **Sauvegarder :** Sauvegarde l'état actuel du jeu dans un fichier.", "sublist"),
        ("       - **Charger :** Charge un état de jeu sauvegardé depuis un fichier.", "sublist"),
        ("       - **Quitter (Ctrl-C) :** Quitte l'application de jeu.", "sublist"),
        ("    - **Menu Coups**", "list"),
        ("       - **Annuler (Ctrl+Z) :** Annule le dernier coup joué.", "sublist"),
        ("       - **Refaire (Ctrl-Shift-Z) :** Restaure les coups qui ont été annulés.", "sublist"),
        ("       - **Mode Édition (Ctrl-E) :** Permet le placement manuel des pierres sur le plateau. Utilisez 'b' pour les pierres noires et 'w' pour les blanches. Cliquer sur une pierre placée la retire.", "sublist"),
        ("       - **Afficher Valeur Coup (Ctrl-V) :** Affiche la valeur stratégique de chaque coup potentiel lorsqu'activé.", "sublist"),
        ("    - **Menu IA**", "list"),
        ("       - **IA pour Noir/Blanc :** Active le contrôle de l'IA pour le joueur Noir ou Blanc.", "sublist"),
        ("       - **Niveau de Difficulté de l'IA :** Réglages de difficulté ajustables pour les adversaires IA (Niveau 1 à 3).", "sublist"),
        ("       - **Threads :** Active ou désactive le multithreading pour les calculs de l'IA.", "sublist"),
        ("    - **Menu Timer**", "list"),
        ("       - **Aucun Timer :** Il n'y a pas de limite de temps pour les coups.", "sublist"),
        ("       - **5/10/15 Minutes :** Définit un timer décompté pour le coup de chaque joueur. Si le timer d'un joueur expire, il perd la partie.", "sublist"),
        ("    - **Menu Info**", "list"),
        ("       - **Version :** Affiche la version actuelle du jeu et les informations des développeurs.", "sublist"),
        ("       - **Mode Débogage :** Active des sorties supplémentaires pour le débogage.", "sublist"),
        ("",""),
        ("Assistance IA", "subtitle"),
        ("    - Appuyez sur **'h'** pour demander à l'IA de suggérer le meilleur coup.", "list"),
        ("    - Appuyez sur **Ctrl+H** pour mettre en évidence les meilleurs coups possibles sur le plateau.", "list")
    ]

    for text, style in content:
        if '**' in text:
            parts = text.split('**')
            for i, part in enumerate(parts):
                tag = style + '_bold' if i % 2 != 0 else style
                text_area.insert(tk.END, part, tag)
            text_area.insert(tk.END, '\n')
        else:
            text_area.insert(tk.END, text + '\n', style)

    text_area.tag_configure('title', font=('Arial', 30, 'bold'), justify='center')
    text_area.tag_configure('subtitle', font=('Arial', 20, 'bold'))
    text_area.tag_configure('subsubtitle', font=('Arial', 18, 'bold'))
    text_area.tag_configure('list', font=('Arial', 14), lmargin1=20, lmargin2=20)
    text_area.tag_configure('sublist', font=('Arial', 14), lmargin1=40, lmargin2=40)
    text_area.tag_configure(style + '_bold', font=('Arial', 14, 'bold'))

    text_area.config(state=tk.DISABLED)
