# def is_three(board, x, y, direction, current_player):
#         dx, dy = direction
#         length=6
#         sequence = []
#         #sequence d'un three
#         three_free=[[0,1,1,1,0,0],[0,1,1,1,0,1],[0,1,1,1,0,2],[0,1,0,1,1,0],[0,1,1,0,1,0]]
#         for i in range(-1, length-1):
#             nx, ny = x + dx * i, y + dy * i
#             if 0 <= nx < 19 and 0 <= ny < 19:  # Assure que la position est dans les limites du plateau
#                 stone = board.get((nx, ny))
#                 if stone and stone == current_player:
#                     sequence.append(1)  # Pierres de la même couleur
#                 elif stone:
#                     sequence.append(2)  # Autre couleur
#                 else:
#                     sequence.append(0)  # Aucune pierre ou pierre de couleur différente
#             else:
#                 sequence.append(0)  # Hors du plateau pour les positions hors limites
#         return sequence in three_free

# def check_double_three(board, x, y, player):
#         directions = [(1, 0), (0, 1), (-1, 0), (0, -1),(1, 1),(-1, -1)]
#         three_count = 0
#         for d in directions:
#             if is_three(board, x, y, d, player):
#                 three_count += 1
#                 if three_count > 1:
#                     # del self.board[x,y]
#                     return True
#         return False

# def generate_possible_moves(board, size, current_player, initial_move=False, debug=False):
#     if len(board)  <= 10 :
#         initial_move = True
#     possible_moves = []
#     center = size // 2
#     radius = 1  # Définissez un rayon initial pour limiter la recherche autour du centre

#     if initial_move:
#         # Générer des mouvements seulement autour du centre pour le premier coup
#         for row in range(center - radius, center + radius + 1):
#             for col in range(center - radius, center + radius + 1):
#                 if (col, row) not in board and \
#                     not check_double_three(board, col, row, current_player): #case vide et pas de Double Three
#                     possible_moves.append((col, row))
#     else:
#         # Votre logique habituelle pour générer des mouvements
#         for row in range(size):  # Assumant que board_size est défini ailleurs
#             for col in range(size):
#                 if (col, row) not in board and \
#                     not check_double_three(board, col, row, current_player): #case vide et pas de Double Three
#                     possible_moves.append((col, row))
#     return possible_moves

# def isAlignement(board, x, y, current_player):
#         directions = [(1, 0), (0, 1), (1, 1), (1, -1)]
#         for dx, dy in directions:
#             count = 1
#             i, j = x + dx, y + dy
#             while 0 <= i < 19 and 0 <= j < 19 and board.get((i, j), {}) == current_player:
#                 count += 1
#                 i, j = i + dx, j + dy
#             i, j = x - dx, y - dy
#             while 0 <= i < 19 and 0 <= j < 19 and board.get((i, j), {}) == current_player:
#                 count += 1
#                 i, j = i - dx, j - dy
#             if count >= 5:
#                 print(f"{current_player} wins")
#                 return True
#         return False

def count_sequences(board, player, sequence_length):
    directions = [(1, 0), (0, 1), (1, 1), (-1, 1)]  # horizontal, vertical, diagonal down, diagonal up
    count = 0
    
    for (x, y), piece in board.items():
        if piece != player:
            continue
        
        for dx, dy in directions:
            seq_count = 1  # Current piece counts as 1
            # Check forward direction
            for i in range(1, sequence_length):
                if board.get((x + dx*i, y + dy*i)) == player:
                    seq_count += 1
                else:
                    break
            # Check if sequence is exactly of 'sequence_length'
            if seq_count == sequence_length:
                # Check backward for overlap only if sequence is exactly matching the length
                if board.get((x - dx, y - dy)) != player:
                    count += 1
                    # Prevent double counting for this direction
                    break
    
    return count
