import tkinter as tk
from gui import GomokuGUI
from game_logic import GomokuLogic

def main():
    root = tk.Tk()
    root.title("Gomoku")
    game_logic = GomokuLogic()
    gui = GomokuGUI(root, game_logic)
    gui.draw_board()
    root.mainloop()

if __name__ == "__main__":
    main()
