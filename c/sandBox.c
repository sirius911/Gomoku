/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandBox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 10:48:54 by clorin            #+#    #+#             */
/*   Updated: 2024/03/24 14:01:25 by clorin           ###   ########.fr       */
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
    char winner = '0';
    char current_player = gameState->currentPlayer;
    char opponent = adversaire(current_player);
    GameState *child_gameState = apply_move(gameState, currentMoveX, currentMoveY);
    if (game_over(child_gameState, &winner)){
        free(child_gameState);
        return -1;
    }
    EvalResult result;
    result.playerScore = _evaluate_player(child_gameState, gameState->currentPlayer);
    result.opponentScore = _evaluate_opponent(child_gameState, opponent);
    free_gameState(child_gameState);
    return result.playerScore - result.opponentScore;
}