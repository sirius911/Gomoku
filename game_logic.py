import random
from constants import *
import time
from ia import minmax
import ctypes
import pickle

class GomokuLogic:
    def __init__(self, size=19, ia = None):
        self.size = size
        self.board = {}
        self.current_player = "black"
        self.opponent = {"black": "white", "white": "black"}
        self.captures = {"black": 0, "white": 0}
        self.ia = ia
        self.libgame = ctypes.CDLL('c/libgame.dylib')

    def switch_player(self):
        self.current_player = self.opponent[self.current_player]

    def play(self, x, y):

        if (x, y) in self.board or not (0 <= x < self.size and 0 <= y < self.size):
            return INVALID_MOVE
        self.board[(x, y)] = self.current_player

        if self.check_double_three(x, y):
            del self.board[x,y]
            return FORBIDDEN_MOVE
        self.check_capture(x, y, self.current_player)
        if self.check_win(x, y):
            return WIN_GAME
        
        # *****   Travail sur implementation C
 
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        self.libgame.count_sequences.restype = ctypes.c_int
        self.libgame.count_sequences.argtypes = [ctypes.c_char_p, ctypes.c_char, ctypes.c_int]
        player = self.current_player.capitalize()[0].encode('utf-8')
        result2 = self.libgame.count_sequences(board_c, player, 2)
        result3 = self.libgame.count_sequences(board_c, player, 3)
        result4 = self.libgame.count_sequences(board_c, player, 4)
        # if result2 > 0 or result3 >0 or result4 >0:
        # print(f"pour {player.decode()}[2]: {result2} - [3]: {result3} - [4]: {result4}")
        # print("--------------------")
        self.libgame.essais(board_c, player)
        #*********************************************

        return CONTINUE_GAME
    
    def play_IA(self):
        class Move(ctypes.Structure):
            _fields_ = [("col", ctypes.c_int),
                        ("row", ctypes.c_int)]
        
        depth = 2 # Ajustez la profondeur de recherche selon les besoins
        self.libgame.play_IA.restype = Move
        self.libgame.play_IA.argtypes = [ctypes.c_char_p, ctypes.c_char, ctypes.c_int]
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        player = self.current_player.capitalize()[0].encode('utf-8')
        best_move = self.libgame.play_IA(board_c, player,depth )
        x, y = best_move.col,best_move.row
        # print(f"best move = ({x},{y})")
        self.board[(x, y)] = self.current_player
        self.switch_player()
        return CONTINUE_GAME

    def check_win(self, x, y):
        # check captures
        if self.captures[self.current_player] >= 10:
            print(f"{self.current_player} wins")
            return True
        #check alignement of 5 Stones
        self.libgame.isAlignment.restype = ctypes.c_bool
        self.libgame.isAlignment.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_char]
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        player = self.current_player.capitalize()[0].encode('utf-8')
        return self.libgame.isAlignment(board_c, x, y, player)
    
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
    
    def board_2_char(self, board = None):
        ret = ""
        if not board:
            board = self.board
        for row in range(self.size):
            for col in range(self.size):
                if (col,row) not in board:
                    ret += "0"
                else:
                    stone = board[(col,row)].capitalize()[0]
                    ret += stone
        ret += "\0"
        return ret
    
    def check_double_three(self, x, y):
        self.libgame.check_double_three.restype = ctypes.c_bool
        self.libgame.check_double_three.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_char]
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        player = self.current_player.capitalize()[0].encode('utf-8')
        return self.libgame.check_double_three(board_c, x, y, player)

    def save(self, path):
        with open(path, 'wb') as save_file:
            pickle.dump({
                'size': self.size,
                'board': self.board,
                'current_player': self.current_player,
                'captures': self.captures,
            }, save_file)

    def load(self, path):
        with open(path, 'rb') as load_file:
            data = pickle.load(load_file)
            self.size = data['size']
            self.board = data['board']
            self.current_player = data['current_player']
            self.captures = data['captures']