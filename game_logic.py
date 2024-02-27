from constants import *

class GomokuLogic:
    def __init__(self, size=19):
        self.size = size
        self.board = {}
        self.current_player = "black"
        self.opponent = {"black": "white", "white": "black"}
        self.captures = {"black": 0, "white": 0}

    def switch_player(self):
        self.current_player = self.opponent[self.current_player]

    def play(self, x, y):
        if (x, y) in self.board or not (0 <= x < self.size and 0 <= y < self.size):
            return INVALID_MOVE
        self.board[(x, y)] = self.current_player
        # print(f"self.board[({x}, {y})] = {self.current_player}")
        if self.check_double_three(x,y):
            return FORBIDDEN_MOVE
        self.check_capture(x, y, self.current_player)
        if self.check_win(x, y):
            return WIN_GAME
        # self.current_player = "white" if self.current_player == "black" else "black"
        return CONTINUE_GAME

    def check_win(self, x, y):
        # check captures
        if self.captures[self.current_player] >= 10:
            print(f"{self.current_player} wins")
            return True
        #check alignement of 5 Stones
        return self.isAlignement(x,y)
    
    def is_three(self, x, y, direction, length=6):
        dx, dy = direction
        sequence = []
        #sequence d'un three
        three_free=[[0,1,1,1,0,0],[0,1,1,1,0,1],[0,1,1,1,0,2],[0,1,0,1,1,0],[0,1,1,0,1,0]]
        for i in range(-1, length-1):
            nx, ny = x + dx * i, y + dy * i
            if 0 <= nx < 19 and 0 <= ny < 19:  # Assure que la position est dans les limites du plateau
                stone = self.board.get((nx, ny))
                if stone and stone == self.current_player:
                    sequence.append(1)  # Pierres de la même couleur
                elif stone:
                    sequence.append(2)  # Autre couleur
                else:
                    sequence.append(0)  # Aucune pierre ou pierre de couleur différente
            else:
                sequence.append(0)  # Hors du plateau pour les positions hors limites
        return sequence in three_free
    
    def check_double_three(self, x, y):
        directions = [(1, 0), (0, 1), (-1, 0), (0, -1),(1, 1),(-1, -1)]
        three_count = 0
        for d in directions:
            if self.is_three(x, y, d):
                three_count += 1
                if three_count > 1:
                    del self.board[x,y]
                    return True
        return False
    
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
                # canvas.delete(board[stone1]['id'])
                # canvas.delete(board[stone2]['id'])
                del self.board[stone1]
                del self.board[stone2]
                self.captures[player] += 2
                captures_made += 2

        return captures_made
    
    def isAlignement(self, x, y):
        directions = [(1, 0), (0, 1), (1, 1), (1, -1)]
        for dx, dy in directions:
            count = 1
            i, j = x + dx, y + dy
            while 0 <= i < 19 and 0 <= j < 19 and self.board.get((i, j), {}) == self.current_player:
                count += 1
                i, j = i + dx, j + dy
            i, j = x - dx, y - dy
            while 0 <= i < 19 and 0 <= j < 19 and self.board.get((i, j), {}) == self.current_player:
                count += 1
                i, j = i - dx, j - dy
            if count >= 5:
                print(f"{self.current_player} wins")
                return True
        return False