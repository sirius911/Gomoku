/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minmax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 23:31:34 by thoberth          #+#    #+#             */
/*   Updated: 2024/05/13 11:29:27 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

bool DEBUG = false;
bool STAT = false;

void    free_gameState(GameState *game) {
    if (game) {
        if (game->board)
            free(game->board);
        free(game);
        game = NULL;
    }
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
    new_gameState->currentPlayer = adversaire(original_gameState->currentPlayer);

    Move *captured;
    do {
        captured = check_capture(new_gameState->board, x, y);
        if (captured) {
            new_gameState->board = del_captured(new_gameState->board, captured);
            if (original_gameState->currentPlayer == 'B')
                new_gameState->captures[0] += 2;
            else
                new_gameState->captures[1] += 2;
            free_moves(captured);
        }
    } while (captured != NULL);
    print_stat(".");
    return new_gameState; // Retourne la nouvelle copie avec le mouvement appliqué
}

EvalResult minmax(GameState *gameState, int depth, int alpha, int beta, bool maximizingPlayer, int col, int row, int ia_level) {
    char winner = '0';
    if (game_over(gameState, &winner) || depth == 0) {
        bool coup_gagnant = false;
        bool coup_perdant = false;
        int score,score_player,score_opponent;
        if (winner != '0') {
            if (maximizingPlayer){
                if (gameState->currentPlayer == winner)
                    coup_gagnant = true;
                else
                    coup_perdant = true;
            } else {
                if (gameState->currentPlayer == winner)
                    coup_perdant = true;
                else
                    coup_gagnant = true;
            }
        } 
            score_player = evaluation_player(gameState, gameState->currentPlayer);
            score_opponent = evaluation_opponent(gameState, adversaire(gameState->currentPlayer));
            score = score_player - score_opponent;
        
        // printf("\t\tscore %c = %d   %c = %d --> %d\n",gameState->currentPlayer, score_player, adversaire(gameState->currentPlayer), score_opponent, score);
        return (EvalResult){.scoreDiff = score, .coup = (Move){col, row, score},
         .coup_gagnant = coup_gagnant, .coup_perdant = coup_perdant, .playerScore = score_player, .opponentScore = score_opponent};
    }

    int move_count;
    EvalResult bestEval = maximizingPlayer ? (EvalResult){.scoreDiff = INT_MIN} : (EvalResult){.scoreDiff = INT_MAX};

    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
	Move *moves = proximate_moves(gameState, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY, ia_level);

    for (int i = 0; i < move_count; i++) {
        // print("\t%c(%d,%d)\n", gameState->currentPlayer,moves[i].col, moves[i].row );
        GameState *newGameState = apply_move(gameState, moves[i].col, moves[i].row);
        EvalResult eval = minmax(newGameState, depth - 1, alpha, beta, !maximizingPlayer, moves[i].col, moves[i].row, ia_level);
        free_gameState(newGameState); // Libération de l'instance de GameState
        if (maximizingPlayer && eval.coup_gagnant) {
            // Si on trouve un coup gagnant pour le joueur maximisant, on choisit immédiatement ce coup.
            bestEval = eval;
            break;
        }
        if (maximizingPlayer) {
            if (eval.scoreDiff > bestEval.scoreDiff) {
                bestEval = eval;
                bestEval.coup = moves[i];
            }
            alpha = (alpha > eval.scoreDiff) ? alpha : eval.scoreDiff;
        } else {
            if (eval.scoreDiff < bestEval.scoreDiff ) {
                bestEval = eval;
                bestEval.coup = moves[i];
            }
            beta = (beta < eval.scoreDiff) ? beta : eval.scoreDiff;
        }

        if (beta <= alpha) {
            break; // Coupe Alpha-Beta
        }
    }
    free_moves(moves);
    return bestEval;
}

Move play_IA(GameState *gameState, int depth, bool debug, bool stat) {
    DEBUG = debug;
    STAT = stat;
    int ia_level = depth;
    int best_score = MIN_EVAL; // Utilisez MIN_EVAL qui est INT_MIN
    Move best_move = {-1, -1}; // Initialisez best_move à une valeur non valide
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = proximate_moves(gameState, &move_count, gameState->currentPlayer, topLeftX,topLeftY,bottomRightX,bottomRightY, ia_level);
    print_stat("\n#\n");
    for (int i = 0; i < move_count; i++) {
        EvalResult result;
        print("\n ***** Coup IA : %c(%d, %d) *****\n", gameState->currentPlayer, moves[i].col, moves[i].row);
		GameState *newGameState = apply_move(gameState, moves[i].col, moves[i].row);
        // analyse(newGameState,true);
        result = minmax(newGameState, (depth * 2) - 1, MIN_EVAL, MAX_EVAL, false, moves[i].col, moves[i].row, ia_level);
		free_gameState(newGameState); // Libération de l'instance de GameState
		print("\n---> Coup: (%d, %d), Score : %d - Score IA: %d, Score Adversaire: %d %s%s",
            moves[i].col, moves[i].row, result.scoreDiff, result.playerScore,result.opponentScore,
            result.coup_gagnant? "Coup gagnant":"", result.coup_perdant? "Coup perdant":"");
        print("\n---------------\n");
        if (result.coup_gagnant){
            best_score = result.scoreDiff;
            best_move = moves[i];
            print("Break coup gagnant\n");
            break;
        }
        if (result.scoreDiff > best_score) {
            best_score = result.scoreDiff;
            best_move = moves[i];
            print("*=* Meilleur coup pour IA *=*\n");
        }
        print("\n");
    }
    if (best_move.col != -1 && best_move.row != -1) {
        print("\n*** best = (%d, %d) with score = %d\n", best_move.col, best_move.row, best_score);
    } else {
        print("Aucun mouvement possible trouvé\n");
    }

    free_moves(moves); // Libérez la liste des mouvements possibles après utilisation
    return best_move;
}

void analyse(GameState *gameState, bool debug) {
    DEBUG = debug;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;

    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    if (topLeftX < SIZE && topLeftY < SIZE) {
        int move_count;
        Move *moves = proximate_moves(gameState, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY, 1);
        
        // print("Analyse sur (%d,%d)x(%d, %d) : %d coups possibles\n", topLeftX, topLeftY, bottomRightX, bottomRightY, move_count);
        for (int i = 0; i < move_count; i++) {
            print("(%d, %d) ", moves[i].col, moves[i].row);
        }
        // print("\n");
        free_moves(moves);
    } else {
        print("Aucun élément non vide trouvé sur le plateau.\n");
    }
    print("Capture pour Black:%d, pour White:%d\n", gameState->captures[0], gameState->captures[1]);
}

bool score_move(GameState *gameState, char *board, int index, Move *move, const char current_player){
    /*
    Add a score to sort the moves depending on:
        - Sequences with other stone of the same color
        - stopping an opponent sequence
        - Make capture
    */
	char opponent_player = (current_player == 'B')?'W':'B';
	int num_player = (current_player == 'B') ? 0 : 1;
    // print("coordonees = %d %d, ", move->col, move->row);
    int score = heuristic(move, current_player, board, index);
	if (score == WIN_MOVE){
		move->score = 10000;
		return true;
    }
    else
        move->score = score + 50;
    // print("coup joueur = %d, ", move->score);
    board[index] = opponent_player;
	score = heuristic(move, opponent_player, board, index);
	if (score == WIN_MOVE) {
		move->score = 5000;
	}
	else
		move->score += score;
	board[index] = current_player;
    // print("coup adversaire = %d\n", score);
	if (check_capture_score(board, move, current_player, opponent_player)) {
		if (gameState->captures[num_player] == 8)
			move->score = 10000;
		else if (gameState->captures[num_player] == 6)
			move->score += 100;
		else
			move->score += 70;
	}
	return false;
}

Move* proximate_moves(GameState *gameState, int *move_count, const char current_player, int x1, int y1, int x2, int y2, int ia_level){
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
    strcpy(copie_board, gameState->board); // Copie l'original dans la nouvelle copie
    int directions[8][2] = {
        {-1, -1}, {0, -1}, {+1, -1},
        {-1, 0},           {+1, 0},
        {-1, +1}, {0, +1}, {+1, +1}
    };
    // on coche toutes les cases autour d'un joueur si vide
    for (int row = y1; row <= y2; ++row) {
        for (int col = x1; col <= x2; ++col) {
            int index = idx(col, row);
            if (copie_board[index] != '0' && copie_board[index] != 'X'){
                for (int i = 0; i < 8; i++){
                    int dx = directions[i][0];
                    int dy = directions[i][1];
                    if (is_valid_position(col + dx, row + dy)) {
                        int index2 = idx(col + dx, row + dy);
                        if (copie_board[index2] == '0' && !check_double_three(gameState->board, col+dx, row+dy, current_player)){
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
        free(copie_board);
        return move;
    }
    // on recupere toutes les cases X
    Move *moves = (Move*) malloc(count * sizeof(Move));
    if (moves == NULL) {
        free(copie_board);
        fprintf(stderr, "Allocation de mémoire échouée\n");
        return NULL;
    }
    for (int i = 0; i < count; i++) { // initialisation de score
        moves[i].score = 0;
    }
	int count2 = 0;
    for (int row = y1; row <= y2; ++row) {
        for (int col = x1; col <= x2; ++col) {
            int index = idx(col, row);
            if (copie_board[index] == 'X'){
                moves[count2].col = col;
                moves[count2].row = row;
				copie_board[index] = current_player;
                if (score_move(gameState, copie_board, index, &moves[count2], current_player)) {
					count2++;
					goto fin_boucles;
				}
				copie_board[index] = 'X';
				count2++;
			}
        }
    }
	fin_boucles:
	qsort(moves, (size_t)count, sizeof(Move), compare_age);

    // ajustement du nombre de coups en fonction de l'IA
    if (ia_level == 1){
        if (count2 < 20)
            *move_count = count2;
        else
            *move_count = 20;
    }
    else if(ia_level == 2){
        if (count2 < 10)
            *move_count = count2;
        else
            *move_count = 10;
    } else {
        if (count2 < 5)
            *move_count = count2;
        else
            *move_count = 5;
    }
    if (copie_board)
        free(copie_board);
    return moves;
}