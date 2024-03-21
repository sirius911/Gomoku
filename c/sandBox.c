/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandBox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 10:48:54 by clorin            #+#    #+#             */
/*   Updated: 2024/03/13 11:09:47 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int nb_coups(GameState *gameState){
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);

    Move *moves = proximate_moves(gameState->board, &move_count, gameState->currentPlayer, topLeftX,topLeftY,bottomRightX,bottomRightY);
    free(moves);
    return move_count;
}