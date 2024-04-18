# Gomoku Game Guide (English)

## Introduction

Gomoku is a traditional board game where two players alternate placing black and white stones on a 19x19 grid with the objective to align five stones in a row horizontally, vertically, or diagonally.

## Game Rules

1. **Board Setup:** The game board consists of a 19x19 grid.
2. **Playing the Game:** Players take turns placing a stone of their color on an empty intersection. Black typically plays first.
3. **Winning the Game:** A player wins by either:
   - Aligning five stones in a row.
   - Capturing a total of 10 opponent stones.
4. **Forbidden Moves:** The game includes a rule against "Double-Threes," where a move that simultaneously forms two lines of three stones is not allowed unless directly contributing to a five-in-a-row.
5. **Captures:** Players can remove a pair of opponent stones from the board by flanking them on both sides with their stones.

## User Interface

### Menus and Options

- **File Menu**
  - **New Game:** Begins a new game.
  - **Save:** Saves the current game state to a file.
  - **Load:** Loads a saved game state from a file.
  - **Quit (Ctr-C):** Exits the game application.
  
- **Moves Menu**
  - **Undo (Ctrl+Z):** Reverses the last move made.
  - **Redo (Ctrl-Shift-Z):** Restores any moves that were undone.
  - **Edit Mode (Ctrl-E):** Allows for the manual placement of stones on the board. Use 'b' for black stones and 'w' for white stones. Clicking on a placed stone will remove it.
  - **Show Move Value (Ctrl-V):** When enabled, displays the strategic value of each potential move.

- **AI Menu**
  - **AI for Black/White:** Enables AI control for either Black or White.
  - **AI Difficulty Level:** Adjustable difficulty settings for AI opponents (Level 1 to 3).
  - **Threads:** Enables or disables multithreading for AI computations.
  
- **Timer Menu**
  - **No Timer:** There is no time limit for moves.
  - **5/10/15 Minutes:** Sets a countdown timer for each player’s move. If a player's timer runs out, they lose the game.

- **Info Menu**
  - **Version:** Displays the game's current version and developer information.
  - **Debug Mode:** Toggles additional output for debugging purposes.

### Additional Features

- **AI Assistance:**
  - Press 'h' to request the AI to suggest the best move.
  - Press Ctrl+H to highlight the best possible moves on the board.

---

# Guide du Jeu Gomoku (Français)

## Introduction

Gomoku est un jeu de plateau traditionnel où deux joueurs placent alternativement des pierres noires et blanches sur une grille de 19x19 dans le but d'aligner cinq pierres de suite horizontalement, verticalement, ou en diagonale.

## Règles du Jeu

1. **Configuration du Plateau :** La grille du jeu est composée de 19x19 intersections.
2. **Jouer au Jeu :** Les joueurs placent à tour de rôle une pierre de leur couleur sur une intersection vide. Le joueur avec les pierres noires commence généralement.
3. **Gagner la Partie :** Un joueur gagne en :
   - Alignant cinq pierres de suite.
   - Capturant un total de 10 pierres adverses.
4. **Coups Interdits :** Le jeu inclut une règle contre les "Doubles-Trois", où un coup qui forme simultanément deux lignes de trois pierres est interdit sauf s'il contribue directement à faire un cinq de suite.
5. **Captures :** Les joueurs peuvent retirer une paire de pierres adverses du plateau en les encadrant des deux côtés avec leurs propres pierres.

## Interface Utilisateur

### Menus et Options

- **Menu Fichier**
  - **Nouvelle Partie :** Commence une nouvelle partie.
  - **Sauvegarder :** Sauvegarde l'état actuel du jeu dans un fichier.
  -

 **Charger :** Charge un état de jeu sauvegardé depuis un fichier.
  - **Quitter (Ctr-C) :** Quitte l'application de jeu.
  
- **Menu Coups**
  - **Annuler (Ctrl+Z) :** Annule le dernier coup joué.
  - **Refaire (Ctrl-Shift-Z) :** Restaure les coups qui ont été annulés.
  - **Mode Édition (Ctrl-E) :** Permet le placement manuel des pierres sur le plateau. Utilisez 'b' pour les pierres noires et 'w' pour les blanches. Cliquer sur une pierre placée la retire.
  - **Afficher Valeur Coup (Ctrl-V) :** Affiche la valeur stratégique de chaque coup potentiel lorsqu'activé.

- **Menu IA**
  - **IA pour Noir/Blanc :** Active le contrôle de l'IA pour le joueur Noir ou Blanc.
  - **Niveau de Difficulté de l'IA :** Réglages de difficulté ajustables pour les adversaires IA (Niveau 1 à 3).
  - **Threads :** Active ou désactive le multithreading pour les calculs de l'IA.
  
- **Menu Timer**
  - **Aucun Timer :** Il n'y a pas de limite de temps pour les coups.
  - **5/10/15 Minutes :** Définit un timer décompté pour le coup de chaque joueur. Si le timer d'un joueur expire, il perd la partie.

- **Menu Info**
  - **Version :** Affiche la version actuelle du jeu et les informations des développeurs.
  - **Mode Débogage :** Active des sorties supplémentaires pour le débogage.

### Fonctionnalités Supplémentaires

- **Assistance IA :**
  - Appuyez sur 'h' pour demander à l'IA de suggérer le meilleur coup.
  - Appuyez sur Ctrl+H pour mettre en évidence les meilleurs coups possibles sur le plateau.