import os
import platform
# import random
from constants import *
import time
import ctypes
import pickle

class GomokuLogic:
    def __init__(self, size=19, ia = {"black":False, "white":False}, debug=False, ia_level=1):
        self.size = size
        self.board = {}
        self.current_player = "black"
        self.opponent = {"black": "white", "white": "black"}
        self.captures = {"black": 0, "white": 0}
        self.ia = ia
        current_dir = os.path.dirname(os.path.abspath(__file__))
        if platform.system() == "Darwin":
            libname = "libgame.dylib"
        else:
            libname = "libgame.so"
        libgame_path = os.path.join(current_dir, libname)
        self.libgame = ctypes.CDLL(libgame_path)
        self.debug = debug
        self.ia_level = ia_level
        self.history = []

    def switch_player(self):
        self.current_player = self.opponent[self.current_player]

    def play(self, x, y):

        if (x, y) in self.board or not (0 <= x < self.size and 0 <= y < self.size):
            return INVALID_MOVE

        self.board[(x, y)] = self.current_player

        if self.check_double_three(x, y):
            del self.board[x,y]
            return FORBIDDEN_MOVE
        captured1, captured2 = self.check_capture(x, y, self.current_player)
        
        self.history.append({'position': (x, y), 'player': self.current_player, 'captures': dict(self.captures), 'captured':(captured1, captured2)})
        
        if self.check_win():
            return WIN_GAME
    
        return CONTINUE_GAME
    
    def IA_Turn(self):
        return self.ia[self.current_player]
    
    def play_IA(self):
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        player = self.current_player.capitalize()[0].encode('utf-8')
        self.libgame.essais(board_c, player,self.debug)
        class Move(ctypes.Structure):
            _fields_ = [("col", ctypes.c_int),
                        ("row", ctypes.c_int)]
        
        depth = self.ia_level # Ajustez la profondeur de recherche selon les besoins
        self.libgame.play_IA.restype = Move
        self.libgame.play_IA.argtypes = [ctypes.c_char_p, ctypes.c_char, ctypes.c_int, ctypes.c_bool]
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        player = self.current_player.capitalize()[0].encode('utf-8')
        best_move = self.libgame.play_IA(board_c, player,depth, self.debug)
        x, y = best_move.col,best_move.row
        if (x, y) == ( -1, -1):
            self.switch_player()
            return WIN_GAME
        
        self.board[(x, y)] = self.current_player
        captured1, captured2 = self.check_capture(x, y, self.current_player)
        self.history.append({'position': (x, y), 'player': self.current_player, 'captures': dict(self.captures), 'captured':(captured1, captured2)})
        if self.check_win():
            return WIN_GAME
        self.switch_player()
        return CONTINUE_GAME
    
    def check_win(self):
         # check captures
        if self.captures[self.current_player] >= 10:
            print(f"{self.current_player} wins")
            return True
        #check alignement of 5 Stones
        self.libgame.count_sequences.restype = ctypes.c_int
        self.libgame.count_sequences.argtypes = [ctypes.c_char_p, ctypes.c_char, ctypes.c_int]
        board = self.board_2_char()
        board_c = ctypes.c_char_p(board.encode('utf-8'))
        winner = self.current_player.capitalize()[0].encode('utf-8')
        return self.libgame.count_sequences(board_c, winner, 5)
    
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
        directions = [(1, 0),(-1, 0),(0, 1), (0, -1),(1, 1),(-1, -1),(-1, 1),(1, -1)]
        stone1, stone2 = None, None
        for dx, dy in directions:
            captured = self.capture(x, y, (dx,dy))
            if captured:
                stone1 = captured[0]
                stone2 = captured[1]
                if self.debug:
                    print(f"Capture détectée à ({stone1[0]}, {stone1[1]}) et ({stone2[0]}, {stone2[1]})")
                del self.board[stone1]
                del self.board[stone2]
                self.captures[player] += 2

        return (stone1, stone2)
    
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
                'ia_black':self.ia['black'],
                'ia_white':self.ia['white'],
                'path':path,
                'ia_level':self.ia_level
            }, save_file)

    def load(self, path):
        try:
            with open(path, 'rb') as load_file:
            
                data = pickle.load(load_file)
                self.size = data['size']
                self.board = data['board']
                self.current_player = data['current_player']
                self.captures = data['captures']
                self.ia = {"black":data['ia_black'], "white":data['ia_white']}
                self.ia_level = data['ia_level']
                return True
        except Exception as e:
            print(f"Erreur (bad file): {e}")
            return False
        
    def undo_move(self):
        if self.history:
            last_move = self.history.pop()  # Récupère le dernier coup joué
            position = last_move['position']
            player = last_move['player']
            self.captures = last_move['captures']
            captured1, captured2 = last_move['captured']
            
            # Supprimez le coup du plateau
            if position in self.board:
                del self.board[position]
            
            # Changez le joueur actuel au joueur précédent
            self.current_player = player
            
            # si captured on restore
            self.opponent[self.current_player]
            if (captured1, captured2) != (None, None):
                self.board[captured1]=self.opponent[self.current_player]
                self.board[captured2]=self.opponent[self.current_player]
            return True
        else:
            return False
