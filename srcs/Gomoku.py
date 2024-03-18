#!/usr/bin/env python3

import argparse
import tkinter as tk
from gui import GomokuGUI
from game_logic import GomokuLogic
from constants import VERSION

def is_gom_file(fileName):
    if not fileName.endswith(".gom"):
        raise argparse.ArgumentTypeError("Le fichier doit avoir l'extension '.gom'")
    return fileName

def main(debug, load, threads):
    root = tk.Tk()
    game_logic = GomokuLogic(debug=debug)
    game_logic.threads = threads
    gui = GomokuGUI(root, game_logic)
    if load is not None:
       gui.load_game(load)
    else:
        gui.update_title()
        gui.update_menu()
        gui.draw_board()
        gui.draw_stones()

    root.mainloop()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=f"Gomoku : Gomoku 42 Game. ({VERSION})")
    parser.add_argument("-d", "--debug", action="store_true", help="Debug option.")
    parser.add_argument("load", nargs='?', type=is_gom_file, help="load file *.go", default=None)
    parser.add_argument("-t","--threads", action="store_true", help="Ia with threads")
    
    args = parser.parse_args()
    main(args.debug, args.load, args.threads)
