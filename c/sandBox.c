/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandBox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 10:48:54 by clorin            #+#    #+#             */
/*   Updated: 2024/05/13 11:13:42 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int value_coup(GameState *gameState, int currentMoveX, int currentMoveY){
    /*
        return score player
    */
    char winner = '0';
    char current_player = gameState->currentPlayer;

    GameState *child_gameState = apply_move(gameState, currentMoveX, currentMoveY);
    if (game_over(child_gameState, &winner)){
        free_gameState(child_gameState);
        return -1;
    }
    EvalResult result;
    result.playerScore = evaluation_player(child_gameState, current_player);
    free_gameState(child_gameState);
    return result.playerScore;
}

int value_coup2(GameState *gameState, int currentMoveX, int currentMoveY){
    /*
    return score opponent
    */
    char winner = '0';
    char current_player = gameState->currentPlayer;
    char opponent = adversaire(current_player);
    GameState *child_gameState = apply_move(gameState, currentMoveX, currentMoveY);
    if (game_over(child_gameState, &winner)){
        free_gameState(child_gameState);
        return -1;
    }
    EvalResult result;
    result.opponentScore = evaluation_opponent(child_gameState, opponent);
    free_gameState(child_gameState);
    return (result.opponentScore);
}