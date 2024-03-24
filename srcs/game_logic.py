import platform
import sys
from constants import *
import time
import ctypes
import pickle

class Move(ctypes.Structure):
            _fields_ = [("col", ctypes.c_int),
                        ("row", ctypes.c_int),
                        ("score", ctypes.c_int)]
            
class GameState(ctypes.Structure):
            _fields_ = [("board", ctypes.POINTER(ctypes.c_char)), # Un pointeur vers un tableau de char
                        ("captures", ctypes.c_int * 2),    # Un tableau de 2 entiers pour les captures
                        ("currentPlayer", ctypes.c_char)]  # Le joueur actuel

class GomokuLogic:
    def __init__(self, size=19, ia = {"black":False, "white":False}, debug=False, ia_level=1):
        self.size = size
        self.board = {}
        self.current_player = "black"
        self.opponent = {"black": "white", "white": "black"}
        self.captures = {"black": 0, "white": 0}
        self.ia = ia
        # current_dir = os.path.dirname(os.path.abspath(__file__))
        if platform.system() == "Darwin":
            libname = f"{PATH_LIB}/libgame.dylib"
        else:
            libname = f"{PATH_LIB}/libgame.so"
        # libgame_path = os.path.join(current_dir, libname)
        try:
            self.libgame = ctypes.CDLL(libname)
        except OSError:
            print(f"La librairie {libname} n'est pas disponible !")
            sys.exit(0)
        except Exception as e:
            print(e)
            sys.exit(0)
        self.debug = debug
        self.ia_level = ia_level
        self.history = []
        self.history_index = -1  # Indice du coup actuel dans l'historique
        self.threads = False
        self.stat = False

    def switch_player(self):
        self.current_player = self.opponent[self.current_player]

    def manual_play(self, x, y):
        if (0 <= x < self.size and 0 <= y < self.size):
            if self.board.get((x, y), '') == self.current_player:
                del self.board[x,y]
            else:
                self.board[(x, y)] = self.current_player

    def play(self, x, y):

        if (x, y) in self.board or not (0 <= x < self.size and 0 <= y < self.size):
            return INVALID_MOVE

        self.board[(x, y)] = self.current_player
        
        if self.check_double_three(x, y):
            del self.board[x,y]
            return FORBIDDEN_MOVE
        captured1, captured2 = self.check_capture(x, y, self.current_player)
        self.record_move(x, y, captured1, captured2)
        # self.history.append({'position': (x, y), 'player': self.current_player, 'captures': dict(self.captures), 'captured':(captured1, captured2)})
        # self.history_index += 1
        # self.sandBox()

        if self.check_win():
            return WIN_GAME
    
        return CONTINUE_GAME
    
    def IA_Turn(self):
        return self.ia[self.current_player]
    
    def play_IA(self):
        
        gameState = self.getGameState()
        self.libgame.analyse(gameState, self.debug)
        start_time = time.time()
        if self.threads:
            self.libgame.play_IA_threads.restype = Move
            best_move = self.libgame.play_IA_threads(gameState, self.ia_level, self.debug)
        else:
            self.libgame.play_IA.restype = Move
            best_move = self.libgame.play_IA(gameState, self.ia_level, self.debug, self.stat)
        end_time = time.time()
        play_time = end_time - start_time
        x, y = best_move.col,best_move.row
        if (x, y) == ( -1, -1):
            print("Abandon prochain coup gagnant !!")       #TODO
            self.switch_player()
            return WIN_GAME, play_time
        
        self.board[(x, y)] = self.current_player
        captured1, captured2 = self.check_capture(x, y, self.current_player)
        self.record_move(x, y, captured1, captured2)
        # self.history.append({'position': (x, y), 'player': self.current_player, 'captures': dict(self.captures), 'captured':(captured1, captured2)})
        # self.history_index += 1
        if self.check_win():
            return WIN_GAME, play_time
        self.switch_player()
        return CONTINUE_GAME, play_time
    
    def help_IA(self):
        gameState = self.getGameState()
        if self.threads:
            self.libgame.play_IA_threads.restype = Move
            best_move = self.libgame.play_IA_threads(gameState, self.ia_level, self.debug)
        else:
            self.libgame.play_IA.restype = Move
            best_move = self.libgame.play_IA(gameState, self.ia_level, self.debug, self.stat)
            
        return best_move.col,best_move.row
    
    def check_win(self):
        self.libgame.game_over.argtypes = [ctypes.POINTER(GameState), ctypes.POINTER(ctypes.c_char)]
        self.libgame.game_over.restype = ctypes.c_bool
        gameState = self.getGameState()
        winner_p = ctypes.c_char()
        return self.libgame.game_over(gameState, ctypes.byref(winner_p))

    def check_capture(self, x, y, player):
        board = self.board_2_char()
        # Configurez les types de retour et les types d'argument pour check_capture et free_captured_moves
        self.libgame.check_capture.restype = ctypes.POINTER(Move)
        self.libgame.check_capture.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
        self.libgame.free_moves.argtypes = [ctypes.POINTER(Move)]

        # Appel à check_capture
        captured_moves = self.libgame.check_capture(board, x, y)

        move1,move2 = None, None
        if captured_moves:
            move1 = Move(captured_moves[0].col, captured_moves[0].row)
            move2 = Move(captured_moves[1].col, captured_moves[1].row)
            del self.board[(move1.col,move1.row)]
            del self.board[(move2.col,move2.row)]
            self.captures[player] += 2
            self.libgame.free_moves(captured_moves)# Libérer la mémoire allouée une fois que vous avez fini avec captured_moves

        return (move1, move2)
    
    def getGameState(self):
        game_state = GameState()
        board_bytes = self.board_2_char()
        game_state.board = ctypes.cast(board_bytes, ctypes.POINTER(ctypes.c_char))  # Conversion en POINTER(c_char)
        game_state.captures = (ctypes.c_int * 2)(self.captures['black'], self.captures['white'])  # Initialiser les captures
        game_state.currentPlayer = ctypes.c_char(self.current_player.capitalize()[0].encode('utf-8'))
        return ctypes.byref(game_state)

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
        return ctypes.c_char_p(ret.encode('utf-8'))
    
    def check_double_three(self, x, y):
        self.libgame.check_double_three.restype = ctypes.c_bool
        self.libgame.check_double_three.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_char]
        board = self.board_2_char()
        player = self.current_player.capitalize()[0].encode('utf-8')
        return self.libgame.check_double_three(board, x, y, player)

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
                'ia_level':self.ia_level,
                'threads':self.threads,
                'history':self.history,
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
                self.threads = data['threads']
                self.history = data['history']
                self.history_index = len(self.history) - 1  
                return True
        except Exception as e:
            print(f"Erreur (bad file): {e}")
            return False
        
    def record_move(self, x, y, captured1, captured2):
        new_move = {'position': (x, y), 'player': self.current_player, 'captures': dict(self.captures), 'captured':(captured1, captured2)}
        # Vérifie si on est à la fin de l'historique
        if self.history_index < len(self.history) - 1:
            # Si non, efface les mouvements qui sont "en avant" dans l'historique
            self.history = self.history[:self.history_index + 1]
        # Ajoute le nouveau mouvement à l'historique
        self.history.append(new_move)
        # Met à jour l'indice pour qu'il pointe toujours sur le dernier élément
        self.history_index = len(self.history) - 1

        
    def undo_move(self):
        if self.history_index >= 0:
            last_move = self.history[self.history_index]
            self.history_index -= 1  # Décrémentez l'indice au lieu de supprimer l'élément
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
                self.board[(captured1.col, captured1.row)]=self.opponent[self.current_player]
                self.board[(captured2.col, captured2.row)]=self.opponent[self.current_player]
                self.captures[player] -= 2
            return True
        else:
            return False
        
    def redo_move(self):
        if self.history_index < len(self.history) - 1:
            self.history_index += 1
            next_move = self.history[self.history_index]
            position = next_move['position']
            player = next_move['player']
            # Appliquez le coup suivant. Assurez-vous que la logique ici correspond à "refaire" le coup correctement
            self.board[position] = player
            self.current_player = self.opponent[player]  # Changez le joueur
            if next_move['captured'] != (None, None):
                captured1, captured2 = next_move['captured']
                del self.board[(captured1.col, captured1.row)]
                del self.board[(captured2.col, captured2.row)]
                self.captures[player] += 2  # Restaurez les captures si nécessaire
            return True
        else:
            return False

        
    def sandBox(self):
        gameState = self.getGameState()
        self.libgame.nb_coups.restype = ctypes.c_int
        nb_coup = self.libgame.nb_coups(gameState)
        print(f"Nombre de coups évalués = {nb_coup}")

    def value_coup(self,x, y):
        self.libgame.value_coup.restype = ctypes.c_int
        gameState = self.getGameState()
        value1 = self.libgame.value_coup(gameState, x, y) # issue des eval minmax
        value2 = self.libgame.value_coup2(gameState, x, y) # issue du score des proximates moves
        return value1, value2