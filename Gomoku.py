import sys
try:
    import argparse
    import tkinter as tk
    from gui import GomokuGUI
    from game_logic import GomokuLogic
    from colorama import Fore,Style
except ModuleNotFoundError as e:
    print(f"{e}")
    print("Please type : pip install -r requirements.txt")
    sys.exit(1)

VERSION = "v1.5c"

def main(ia):
    root = tk.Tk()
    title = "Gomoku "
    if ia == "black":
        title += "IA vs Humain"
    elif ia == "white":
        title += "Humain vs IA"
    else:
        title += "Humain vs Humain"
    root.title(title)
    game_logic = GomokuLogic(ia=ia)
    gui = GomokuGUI(root, game_logic)
    gui.draw_board()
    gui.draw_stones()
    if gui.is_IA_turn():
        gui.ia_play()
    root.mainloop()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Gomoku Game")
    parser.add_argument("-v", "--version", action="store_true", help="Show version of the program.")
    parser.add_argument("-i","--ia", type=str, default="",choices=["white", "black"], help="color of IA")
    args = parser.parse_args()
    if args.version:
        print(f"\n----------------------------------\n{Fore.CYAN}Gomoku 42 Project{Style.RESET_ALL}: Version = {Fore.GREEN}{VERSION}{Style.RESET_ALL}\n----------------------------------\n")
        sys.exit(0)
    main(args.ia)
