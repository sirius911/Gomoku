/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ai_logic.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 18:55:06 by clorin            #+#    #+#             */
/*   Updated: 2024/03/11 19:44:11 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

bool DEBUG = false; 

void    free_gameState(GameState *game) {
    if (game) {
        if (game->board)
            free(game->board);
        free(game);
    }
}

int _evaluate_player(const GameState *gameState, char player) {
    int score = 0;
    int num_player =( player == 'B')? 0:1;
    if (count_sequences(gameState->board, player, 4) >= 1 || 
        count_sequences(gameState->board, player, 5) > 0 ||
        gameState->captures[num_player] >= 10)
        return MAX_EVAL;

    score += count_sequences(gameState->board, player, 2) * 10;
    score += count_sequences(gameState->board, player, 3) * 50;
    score += count_sequences(gameState->board, player, 4) *10000;

    score += gameState->captures[num_player] * 100 ;
    return score;
}
int _evaluate_opponent(const GameState *gameState, char opponent) {
    int score = 0;
    int num_player =( opponent == 'B')? 0:1;
    if (count_sequences(gameState->board, opponent, 5) > 0 ||
        gameState->captures[num_player] >= 10)
        return MAX_EVAL;

    score += count_sequences(gameState->board, opponent, 2) * 20;
    score += count_sequences(gameState->board, opponent, 3) * 100;
    score += count_sequences(gameState->board, opponent, 4) * 100000;

    score += gameState->captures[num_player] * 50 ;
    return score;
}

GameState *apply_move(const GameState *original_gameState, int x, int y) {
    int capture0 = original_gameState->captures[0];
    int capture1 = original_gameState->captures[1];

    GameState *new_gameState = malloc(sizeof (GameState)); // Alloue de la mémoire pour la nouvelle copie
    if (new_gameState == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    char *new_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour la nouvelle copie
    if (new_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_board, original_gameState->board); // Copie l'original dans la nouvelle copie
    new_gameState->board = new_board;
    new_gameState->captures[0] = capture0;
    new_gameState->captures[1] = capture1;
    int index = idx(x, y);
    new_gameState->board[index] = original_gameState->currentPlayer;
    new_gameState->currentPlayer = (original_gameState->currentPlayer == 'W') ? 'B' : 'W';
    Move *captured = check_capture(new_gameState->board, x, y);
    if (captured){
        new_gameState->board = del_captured(new_gameState->board, captured);
        if (original_gameState->currentPlayer == 'B')
            new_gameState->captures[0] += 2;
        else
            new_gameState->captures[1] += 2;
    }
    return new_gameState; // Retourne la nouvelle copie avec le mouvement appliqué
}


EvalResult minmax(GameState *gameState, int depth, int alpha, int beta, bool maximizingPlayer, int currentMoveX, int currentMoveY) {
    char winner = '0';
    GameState *child_gameState = apply_move(gameState, currentMoveX, currentMoveY);
    if (game_over(child_gameState, &winner) || depth == 0) {

        char opponent = (gameState->currentPlayer == 'W') ? 'B' : 'W';

        EvalResult result;
        result.coup_gagnant = false;
        if (winner != '0'){
            // un gagnant ou match null
            // TODO gestion de winner == 'N' -> match Null
            if (maximizingPlayer){
                result.scoreDiff = INT_MAX - depth; // Favoriser les victoires plus rapides
                result.playerScore = INT_MAX - depth;
                result.opponentScore = INT_MIN + depth;
                result.coup_gagnant = true;
            } else {
                result.scoreDiff = INT_MIN + depth; // La pénalité est moindre pour les défaites tardives
                result.playerScore = INT_MIN + depth;
                result.opponentScore = INT_MAX - depth;
            }
        } else {
            // Pas de vainqueur ou profondeur atteinte, évaluer la position
            if (maximizingPlayer) {
                result.playerScore = _evaluate_player(child_gameState, gameState->currentPlayer);
                result.opponentScore = _evaluate_opponent(child_gameState, opponent);
            } else {
                result.playerScore = _evaluate_player(child_gameState, opponent);
                result.opponentScore = _evaluate_opponent(child_gameState, gameState->currentPlayer);
            }
            result.scoreDiff = result.playerScore - result.opponentScore;
        }
        print("\tScore : %c= %d,  %c= %d -> diff= %d\n\t capture B:%d, W:%d\n",
            gameState->currentPlayer , result.playerScore, opponent, result.opponentScore, result.scoreDiff, gameState->captures[0], gameState->captures[1]);

        free_gameState(child_gameState);
        return result;
    }

    char opponent = adversaire(gameState->currentPlayer);
    EvalResult bestResult;

    if (maximizingPlayer) {
        int maxEval = MIN_EVAL;
        int move_count;
        bestResult.playerScore = MIN_EVAL; // Le pire score pour le joueur

        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(child_gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = proximate_moves(child_gameState->board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            print("\t+ Si %c(%d,%d)\n",opponent,moves[i].col, moves[i].row);

            EvalResult result = minmax(child_gameState, depth - 1, alpha, beta, !maximizingPlayer, moves[i].col, moves[i].row);

            if(result.scoreDiff > maxEval) {
                maxEval = result.scoreDiff;
                result.coup = moves[i];
                bestResult = result;
                print(" best move!\n");
            }
            else
                print("\n");
            
            alpha = (result.scoreDiff > alpha) ? result.scoreDiff : alpha;
            if (beta <= alpha) {
                print("- break \n");
                break;
            }
        }
        free(moves);
        print("\t\tMeilleur coup pour %c(%d,%d), score=%d  %c=%d, %c=%d\n",
        child_gameState->currentPlayer, bestResult.coup.col, bestResult.coup.row,
        bestResult.scoreDiff,
        child_gameState->currentPlayer, bestResult.playerScore,
        adversaire(child_gameState->currentPlayer), bestResult.opponentScore);
    } else {
        int minEval = MAX_EVAL;
        int move_count;
        bestResult.playerScore = MAX_EVAL; // Le meilleur score pour le joueur, à minimiser
        bestResult.opponentScore = INT_MIN; // Optionnellement, le meilleur score pour l'adversaire
        
        
        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(child_gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = proximate_moves(child_gameState->board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            print("\t\t-%c(%d,%d)\n",opponent,moves[i].col, moves[i].row);
           EvalResult result = minmax(child_gameState, depth - 1, alpha, beta, !maximizingPlayer, moves[i].col, moves[i].row);
 
            if (result.scoreDiff < minEval) {
                minEval = result.scoreDiff;
                result.coup = moves[i];
                bestResult = result;
                print(" *worst move !\n");
            }
            else
                print("\n");
           beta = (result.scoreDiff < beta) ? result.scoreDiff : beta;
            
            if (beta <= alpha) {
                print("- break -\n");
                break;
            }
        }
        free(moves);
        print("\tMeilleur coup pour %c(%d,%d), Score: %d, %c=%d, %c=%d\n",
        opponent, bestResult.coup.col, bestResult.coup.row, 
        bestResult.scoreDiff,
        opponent, bestResult.playerScore,
        adversaire(opponent), bestResult.opponentScore);
    }
    return bestResult;
}

Move play_IA(GameState *gameState, int depth, bool debug) {
    DEBUG = debug;
    int best_score = MIN_EVAL; // Utilisez MIN_EVAL qui est INT_MIN
    Move best_move = {-1, -1}; // Initialisez best_move à une valeur non valide
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = proximate_moves(gameState->board, &move_count, gameState->currentPlayer, topLeftX,topLeftY,bottomRightX,bottomRightY);
    for (int i = 0; i < move_count; i++) {
        EvalResult result;
        print("\n ***** Coup IA : %c(%d, %d) *****\n", gameState->currentPlayer, moves[i].col, moves[i].row);
        result = minmax(gameState, depth, MIN_EVAL, MAX_EVAL, true, moves[i].col, moves[i].row);
        print("\n---> Coup: (%d, %d), Score : %d - Score IA: %d, Score Adversaire: %d %s",
            moves[i].col, moves[i].row, result.scoreDiff, result.playerScore,result.opponentScore, result.coup_gagnant? "Coup gagnant":"");
        print("\n---------------\n");
        if (result.coup_gagnant){
            best_score = result.scoreDiff;
            best_move = moves[i];
            break;
        }
        if (result.scoreDiff > best_score) {
            best_score = result.scoreDiff;
            best_move = moves[i];
        }
        print("\n");
    }

    if (best_move.col != -1 && best_move.row != -1) {
        print("\n*** best = (%d, %d) with score = %d\n", best_move.col, best_move.row, best_score);
    } else {
        print("Aucun mouvement possible trouvé\n");
    }

    free(moves); // Libérez la liste des mouvements possibles après utilisation
    return best_move;
}

void analyse(GameState *gameState, bool debug) {
    DEBUG = debug;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;

    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    if (topLeftX < SIZE && topLeftY < SIZE) {
        int move_count;
        Move *moves = proximate_moves(gameState->board, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY);
        
        print("Analyse sur (%d,%d)x(%d, %d) : %d coups possibles\n", topLeftX, topLeftY, bottomRightX, bottomRightY, move_count);
        print_sequences_board(gameState->board, "");
        for (int i = 0; i < move_count; i++) {
            print("(%d, %d) ", moves[i].col, moves[i].row);
        }
        print("\n");
        free(moves);
    } else {
        print("Aucun élément non vide trouvé sur le plateau.\n");
    }
    print("Capture pour Black:%d, pour White:%d\n", gameState->captures[0], gameState->captures[1]);
}

Move* proximate_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2){
    /*  XXX
        XBX
        XXX
    */
    int count = 0;
    char *copie_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour une copie
    if (copie_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(copie_board, board); // Copie l'original dans la nouvelle copie
    int directions[8][2] = {
        {-1, -1}, {0, -1}, {+1, -1},
        {-1, 0},           {+1, 0},
        {-1, +1}, {0, +1}, {+1, +1}
    };
    // on coche toutes les cases autour d'un joueur si vide
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            int index = idx(col, row);
            if (copie_board[index] != '0' && copie_board[index] != 'X'){
                for (int i = 0; i < 8; i++){
                    int dx = directions[i][0];
                    int dy = directions[i][1];
                    if (is_valid_position(col + dx, row + dy)) {
                        int index2 = idx(col + dx, row + dy);
                        if (copie_board[index2] == '0' && !check_double_three(board, col+dx, row+dy, current_player)){
                            copie_board[index2] = 'X' ;
                            count++;
                        }
                    }
                }
            }
        }
    }
    if (count == 0) {
        print("board vide");
        Move *move = (Move*) malloc(sizeof(Move));
        move->col=9;
        move->row=9;
        *move_count = 1;
        return move;
    }
    // on recupere toutes les cases X
    Move *moves = (Move*) malloc(MAX_MOVES * sizeof(Move));
    if (moves == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        return NULL;
    }
    count = 0;
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            int index = idx(col, row);
            if (copie_board[index] == 'X'){
                moves[count].col = col;
                moves[count].row = row;
                count++;
            }
        }
    }
    *move_count = count;
    free(copie_board);
    return moves;
}

Move* generate_possible_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2) {
    /*
        return a array of struc Move with possible moves in (x1,y1)(x2,y2)
    */
    int maxMoves = abs(x2 - x1 + 1) * abs(y2 - y1 + 1);
    Move *moves = (Move*) malloc(maxMoves * sizeof(Move));
    int count = 0;

    for (int row = y1; row <= y2; ++row) {
        for (int col = x1; col <= x2; ++col) {
            int index = idx(col, row);
            if (board[index] == '0' && ! check_double_three(board, col, row, current_player)) { // '0' représente une case vide
                if (count < MAX_MOVES) {
                    moves[count].col = col;
                    moves[count].row = row;
                    count++;
                }
            }
        }
    }

    *move_count = count;
    return moves;
}