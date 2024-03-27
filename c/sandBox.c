/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandBox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 10:48:54 by clorin            #+#    #+#             */
/*   Updated: 2024/03/26 16:07:14 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int nb_coups(GameState *gameState){
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);

    Move *moves = proximate_moves(gameState, &move_count, gameState->currentPlayer, topLeftX,topLeftY,bottomRightX,bottomRightY);
    free(moves);
    return move_count;
}

int value_coup(GameState *gameState, int currentMoveX, int currentMoveY){
    // test des count_seq
    gameState->board[idx(currentMoveX, currentMoveY)] = gameState->currentPlayer;
    printf("%c-2 libres : %d\n", gameState->currentPlayer, counter(gameState->board, gameState->currentPlayer, SEQ_2_LIBRE));
    // printf("%c-2 semi-libre :  %d\n", gameState->currentPlayer, count_seq_2_semi_libre(gameState->board,gameState->currentPlayer));
    // printf("%c-3 libres : %d\n", gameState->currentPlayer, count_seq_3_libre(gameState->board, gameState->currentPlayer));
    // printf("%c-3 semi-libre : %d\n", gameState->currentPlayer, count_seq_3_semi_libre(gameState->board, gameState->currentPlayer));
    gameState->board[idx(currentMoveX, currentMoveY)] = '0';
    return 0;
    char winner = '0';
    char current_player = gameState->currentPlayer;
    char opponent = adversaire(current_player);
    GameState *child_gameState = apply_move(gameState, currentMoveX, currentMoveY);
    if (game_over(child_gameState, &winner)){
        free_gameState(child_gameState);
        return -1;
    }
    EvalResult result;
    result.playerScore = _evaluate_player(child_gameState, gameState->currentPlayer);
    result.opponentScore = _evaluate_opponent(child_gameState, opponent);
    free_gameState(child_gameState);
    return result.playerScore - result.opponentScore;
}

int value_coup2(GameState *gameState, int currentMoveX, int currentMoveY){
    Move *move = (Move*) malloc(sizeof(Move));
    int score = 0;
    move->col=currentMoveX;
    move->row=currentMoveY;
    int index = idx(currentMoveX, currentMoveY);
    char *copie_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour une copie
    
    if (copie_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(copie_board, gameState->board); // Copie l'original dans la nouvelle copie
    copie_board[index] = gameState->currentPlayer;
    if (score_move(gameState, copie_board, index, move, gameState->currentPlayer)){
        free(copie_board);
        free(move);
        return -1;
    }
    score = move->score;
    free(copie_board);
    free(move);
    return score;
}