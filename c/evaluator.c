/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   evaluator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:50:04 by clorin            #+#    #+#             */
/*   Updated: 2024/03/29 19:32:44 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

// Compare deux chaînes en tenant compte des jokers '*' '+'
// Retourne true si les chaînes correspondent, false sinon
bool matchWithJoker(const char *sequence, const char *pattern) {
    // printf(" Compare %s et %s ", sequence, pattern);
    for (int i = 0; pattern[i] != '\0' && sequence[i] != '\0'; i++) {
        if ((pattern[i] != '*' && pattern[i] != '+' && sequence[i] != pattern[i] )) {
            // printf("False 1\n");
            return false;
        }
        if (pattern[i] == '+' && sequence[i] == '0'){ // Forcer un non vide
            // printf("False 2\n");
            return false;
        }
    }
    // printf("True\n");
    return true;
}

char *seq2(const char *board, int x, int y, int dx, int dy, char player, int nb) {
    /*
        return a seq of nb ligne with '0' if empty, '1' if player, '2' if opponent
                X                          X
            1 0 0 0 0 0 0 0   (2,2)      [.1 0 0 0]0 0 0 0  -> [-1, 1, 0, 0]
            0 0 0 0 0 0 0 0   nb=5 ->      0 0 0 0 0 0 0 0  -> [0, 1, 0, 0]
        Y   0 0 1 0 1 0 0 0                0[0 1 0 1 0]0 0
    */

    // Allocation dynamique du tableau pour stocker la séquence
    // printf("Seq de %d en (%d,%d) [%d,%d] %c -> ", nb, x, y, dx, dy, player);
    char *sequence = (char *)malloc((nb + 1) * sizeof(char));
    if (sequence == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL; // Retourne NULL en cas d'échec de l'allocation
    }
    // memset(sequence, 0, nb * sizeof(char));
    for (int i = -1; i < nb-1; i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        if (is_valid_position(nx, ny)) {
            char stone = get(board, nx, ny);
            if (stone == player) 
                sequence[i + 1] = '1';
            else if (stone != '\0' && stone != '0') // Suppose '0' représente une case vide
                sequence[i + 1] = '2';
            else
                sequence[i + 1] = '0';
        }
        else
            sequence[i + 1] = '|'; //bord
    }
    sequence[nb] = '\0';
    // printf(" - [%s]",sequence);
    return sequence;
}

int counter(const char *board, const char player, const char good[6]){
    // int topLeftX, topLeftY, bottomRightX, bottomRightY;
    const int nb_dir = 8;
    const int taille_seq = 6;
    int count = 0;
    int directions[8][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };
    // findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    for (int y = 0 ; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[idx(x, y)] != player) continue;
            for (int d = 0; d < nb_dir; d++) {
                char *sequence = seq2(board, x, y, directions[d][1], directions[d][0], player, taille_seq);
                // Vérifie si la séquence correspond à une des séquences de "good" autorisées
                    if (matchWithJoker(sequence, good))
                        count++;
                if(sequence)
                    free(sequence);
            }
        }
    }
    return count;
}

int count_seq_4_trous(const char *board, char player){
    return counter(board, player, SEQ_4_TROUS);
}

#define SCORE_A_PLAYER 2000
#define SCORE_B_PLAYER 50
#define SCORE_C_PLAYER 25
#define SCORE_D_PLAYER 10
#define SCORE_E_PLAYER 5
#define SCORE_PRISE_PLAYER 100
int evaluation_player(const GameState *gameState, const char player){
    int _4_, _4, _4t, _3_, _3, _2_, _2;
    int score = 0;
    int num_player =( player == 'B')? 0:1;

    int nb_prise = gameState->captures[num_player];

    if (counter(gameState->board, player, SEQ_5) > 0 || nb_prise >= 10)
        return MAX_EVAL;
    
    _4_ = counter(gameState->board, player, SEQ_4_LIBRE);
    score += _4_ * SCORE_A_PLAYER;
    // if ( _4_ > 0)
    //     return MAX_EVAL -1;
    _4  = counter(gameState->board, player, SEQ_4_SEMI_LIBRE);
    score += SCORE_A_PLAYER * _4;
    _4t = counter(gameState->board, player, SEQ_4_TROUS);
    score += SCORE_A_PLAYER * _4t;
    _3_ = counter(gameState->board, player, SEQ_3_LIBRE);
    score += SCORE_B_PLAYER * (_3_ / 2);
    _3 = counter(gameState->board, player, SEQ_3_SEMI_LIBRE) - _4_ - _4;
    score += (_3 * SCORE_C_PLAYER);
    _2_ = counter(gameState->board, player, SEQ_2_LIBRE) / 2;
    score += (_2_ * SCORE_D_PLAYER);
    _2 = counter(gameState->board, player, SEQ_2_SEMI_LIBRE) - _4_ - _4 - _3_ - _3;
    score += (_2 * SCORE_E_PLAYER);
    score += (nb_prise * SCORE_PRISE_PLAYER);
    return score;
}
#define SCORE_A_OPPONENT 4000
#define SCORE_B_OPPONENT 500
#define SCORE_C_OPPONENT 40
#define SCORE_D_OPPONENT 20
#define SCORE_E_OPPONENT 10
#define SCORE_PRISE_OPPONENT 50
int evaluation_opponent(const GameState *gameState, const char player){
    int _4_, _4, _4t, _3_, _3, _2_, _2;
    int score = 0;
    int num_player =( player == 'B')? 0:1;

    int nb_prise = gameState->captures[num_player];

    if (counter(gameState->board, player, SEQ_5) > 0 || nb_prise >= 10)
        return MAX_EVAL;
    
    _4_ = counter(gameState->board, player, SEQ_4_LIBRE);
    if ( _4_ > 0)
        return MAX_EVAL;
    _4  = counter(gameState->board, player, SEQ_4_SEMI_LIBRE);
    _4t = counter(gameState->board, player, SEQ_4_TROUS);
    score += SCORE_A_OPPONENT * (2 * _4t);
    score += SCORE_A_OPPONENT * _4;
    _3_ = counter(gameState->board, player, SEQ_3_LIBRE);
    score += SCORE_B_OPPONENT * (_3_ / 2);
    _3 = counter(gameState->board, player, SEQ_3_SEMI_LIBRE) - _4_ - _4;
    score += (_3 * SCORE_C_OPPONENT);
    _2_ = counter(gameState->board, player, SEQ_2_LIBRE) / 2;
    score += (_2_ * SCORE_D_OPPONENT);
    _2 = counter(gameState->board, player, SEQ_2_SEMI_LIBRE) - _4_ - _4 - _3_ - _3;
    score += (_2 * SCORE_E_OPPONENT);
    score += (nb_prise * SCORE_PRISE_OPPONENT);
    return score;
}

int evaluate_game(const GameState *gameState){
    const char player = gameState->currentPlayer;
    const char opponent = adversaire(player);

    int score_player = evaluation_player(gameState, player);
    int score_opponent = evaluation_opponent(gameState, opponent);
    return score_player - score_opponent;
}