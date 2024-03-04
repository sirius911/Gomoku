import sys
try:
    import argparse
    import tkinter as tk
    from gui import GomokuGUI
    from game_logic import GomokuLogic
    from colorama import Fore,Style
    from constants import VERSION
except ModuleNotFoundError as e:
    print(f"{e}")
    print("Please type : pip install -r requirements.txt")
    sys.exit(1)

def main():
    root = tk.Tk()
    game_logic = GomokuLogic()
    gui = GomokuGUI(root, game_logic)
    gui.change_title()
    gui.draw_board()
    gui.draw_stones()
    if gui.is_IA_turn():
        gui.ia_play()
    root.mainloop()

if __name__ == "__main__":
    main()
