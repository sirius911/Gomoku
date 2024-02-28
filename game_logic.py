import random
from constants import *
import time
from ia import minmax
from utils_board import check_double_three, generate_possible_moves, isAlignement

class GomokuLogic:
    def __init__(self, size=19, ia = None):
        self.size = size
        self.board = {}
        self.current_player = "black"
        self.opponent = {"black": "white", "white": "black"}
        self.captures = {"black": 0, "white": 0}
        self.ia = ia

    def switch_player(self):
        self.current_player = self.opponent[self.current_player]

    def play(self, x, y):
        if (x, y) in self.board or not (0 <= x < self.size and 0 <= y < self.size):
            return INVALID_MOVE
        self.board[(x, y)] = self.current_player
        # print(f"self.board[({x}, {y})] = {self.current_player}")
        if check_double_three(self.board, x, y, self.current_player):
            del self.board[x,y]
            return FORBIDDEN_MOVE
        self.check_capture(x, y, self.current_player)
        if self.check_win(x, y):
            return WIN_GAME
        # self.current_player = "white" if self.current_player == "black" else "black"
        return CONTINUE_GAME
    
    def play_IA(self):
        depth = 3  # Profondeur de recherche, ajustez selon le besoin
        best_score = float('-inf')
        best_move = None
        for move in generate_possible_moves(self.board, self.size, self.current_player):
            print(f"move = {move}", end=' ', flush=True)
            # Assumez une structure de données pour 'move' qui est compatible avec votre logique de jeu
            score = minmax(self.board, depth, float('-inf'), float('inf'), True, self.current_player)
            print(f"score = {score}", end=' ', flush=True)
            if score > best_score:
                best_score = score
                best_move = move
                print("+")
            else:
                print("")
        print(f"*** best = {best_move} with score = {score}")
        x, y = best_move
        self.board[(x, y)] = self.current_player
        self.switch_player()
        return CONTINUE_GAME

    def check_win(self, x, y):
        # check captures
        if self.captures[self.current_player] >= 10:
            print(f"{self.current_player} wins")
            return True
        #check alignement of 5 Stones
        return isAlignement(self.board,x,y, self.current_player)
    
    def capture(self, x, y, direction):
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
        if (x, y) in self.board:
            if ((x + dx1, y + dy1)) in self.board:
                if ((x + dx2, y + dy2)) in self.board:
                    if ((x + dx3, y + dy3)) in self.board:
                        ret = [self.board[x,y], \
                               self.board[x+dx1,y+dy1],\
                               self.board[x+dx2,y+dy2], \
                               self.board[x+dx3,y+dy3]]
        if (len(ret) == 4):
            if (ret[0] == ret[3] and ret[1] == ret[2] and ret[0] != ret[1]):
                to_del1 = (x+dx1, y+dy1)
                to_del2 = (x+dx2, y+dy2)
                return (to_del1, to_del2) 
        return None

    def check_capture(self, x, y, player):
        captures_made = 0
        directions = [(1, 0),(-1, 0),(0, 1), (0, -1),(1, 1),(-1, -1),(-1, 1),(1, -1)]
        for dx, dy in directions:
            captured = self.capture(x, y, (dx,dy))
            if captured:
                stone1 = captured[0]
                stone2 = captured[1]
                print(f"Capture détectée à ({stone1[0]}, {stone1[1]}) et ({stone2[0]}, {stone2[1]})")
                del self.board[stone1]
                del self.board[stone2]
                self.captures[player] += 2
                captures_made += 2

        return captures_made
    