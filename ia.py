# ia.py

from utils_board import count_sequences


def evaluate(board, current_player):
    score = 0
    # Assumer que 'opponent' est défini comme l'adversaire de 'current_player'
    # Ajoutez des points pour chaque séquence favorable à l'IA
    score += count_sequences(board, current_player, 2) * 10  # Exemple de pondération pour une paire
    score += count_sequences(board, current_player, 3) * 50  # Exemple de pondération pour un trio
    score += count_sequences(board, current_player, 4) * 100  # Exemple de pondération pour un quatuor
    score += count_sequences(board, current_player, 5) * 10000
    opponent = "black" if current_player == "white" else "white"
    # Soustrayez des points pour les séquences de l'adversaire pour bloquer les menaces potentielles
    # print(f"count_sequences(board, opponent, 3) = {count_sequences(board, opponent, 3)}", end=' ')
    score -= count_sequences(board, opponent, 2) * 20
    score -= count_sequences(board, opponent, 3) * 100 # Exemple de pondération pour bloquer un trio adverse
    score -= count_sequences(board, opponent, 4) * 200  # Exemple de pondération pour bloquer un quatuor adverse
    score -= count_sequences(board, opponent, 5) * 100000
    return score

def game_over(board):
    for x in range(19):
        for y in range(19):
            if board.get((x, y)) is not None:
                if isAlignement(board, x, y, board.get((x, y))):
                    print("Un alignement gagnant a été trouvé")
                    return True  # Un alignement gagnant a été trouvé
    # Vérifiez si le plateau est entièrement rempli pour le match nul
    if all(board.get((x, y)) is not None for x in range(19) for y in range(19)):
        print("match null")
        return True  # Match nul
    return False  # Le jeu continue


def apply_move(board, move, player):
    # Faites une copie profonde du plateau pour ne pas modifier l'original
    new_board = dict(board)
    # Appliquez le mouvement
    new_board[move] = player
    # Retournez le nouveau plateau après le mouvement
    return new_board


def minmax(board, depth, alpha, beta, maximizingPlayer, current_player):
    # print(f"depth = {depth}")
    if depth == 0 or game_over(board):
        # print("fin recurence")
        return evaluate(board, current_player)
    
    opponent = "black" if current_player == "white" else "white"
    
    if maximizingPlayer:
        maxEval = float('-inf')
        # print(f"Maximizing Player {current_player}",flush=True)
        for move in generate_possible_moves(board, 19, current_player,True if depth ==1 else False):
            child_board = apply_move(board, move, current_player)
            eval = minmax(child_board, depth - 1, alpha, beta, False, opponent)  # Changement ici pour utiliser opponent
            # print(f"\tTry: {move}, Eval: {eval}, MaxEval: {maxEval}, Alpha: {alpha}, Beta: {beta}",flush=True)
            maxEval = max(maxEval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha:
                # print("Pruning",flush=True)
                break
        return maxEval
    else:
        minEval = float('inf')
        # print(f"Minimizing Player {current_player}",flush=True)
        for move in generate_possible_moves(board, 19, opponent,True if depth ==1 else False):  # Changement ici pour générer des coups pour l'opposant
            child_board = apply_move(board, move, opponent)  # Applique le coup comme si l'opposant jouait
            eval = minmax(child_board, depth - 1, alpha, beta, True, current_player)  # Revenir à current_player
            # print(f"\tTry: {move}, Eval: {eval}, MinEval: {minEval}, Alpha: {alpha}, Beta: {beta}",flush=True)
            minEval = min(minEval, eval)
            beta = min(beta, eval)
            if beta <= alpha:
                # print("Pruning",flush=True)
                break
        return minEval
