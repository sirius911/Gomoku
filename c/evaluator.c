/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   evaluator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:50:04 by clorin            #+#    #+#             */
/*   Updated: 2024/05/13 11:07:26 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

// Compare deux chaînes en tenant compte des jokers '*' '+'
// Retourne true si les chaînes correspondent, false sinon
bool matchWithJoker(const char *sequence, const char *pattern) {
    for (int i = 0; pattern[i] != '\0' && sequence[i] != '\0'; i++) {
        if ((pattern[i] != '*' && pattern[i] != '+' && sequence[i] != pattern[i] ))
            return false;
        if (pattern[i] == '+' && sequence[i] == '0') // Forcer un non vide
            return false;
    }
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
    char *sequence = (char *)malloc((nb + 1) * sizeof(char));
    if (sequence == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL; // Retourne NULL en cas d'échec de l'allocation
    }
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
    return sequence;
}

int counter(const char *board, const char player, const char good[6]){
    const int nb_dir = 8;
    const int taille_seq = 6;
    int count = 0;
    int directions[8][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };
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


#define SCORE_4 2500
#define SCORE_3 1200
#define SCORE_2 500
#define SCORE_PEN 100
#define SCORE_CAPT 1200


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
    score += _4_ * SCORE_4;
    _4  = counter(gameState->board, player, SEQ_4_SEMI_LIBRE);
    score += (SCORE_4 - SCORE_PEN) * _4;
    _4t = counter(gameState->board, player, SEQ_4_TROUS);
    score += SCORE_3 * _4t;
    _3_ = counter(gameState->board, player, SEQ_3_LIBRE);
    score += SCORE_3 * _3_;
    _3 = counter(gameState->board, player, SEQ_3_SEMI_LIBRE);
    score += (_3 * (SCORE_3 - SCORE_PEN));
    _2_ = counter(gameState->board, player, SEQ_2_LIBRE);
    score += (_2_ * SCORE_2);
    _2 = counter(gameState->board, player, SEQ_2_SEMI_LIBRE);
    score += (_2 * (SCORE_2 - SCORE_PEN));
    score += (nb_prise * SCORE_CAPT);
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
    score += (SCORE_4 - SCORE_PEN) * _4 + SCORE_PEN;
    _4t = counter(gameState->board, player, SEQ_4_TROUS);
    score += SCORE_3 * _4t + SCORE_PEN;
    _3_ = counter(gameState->board, player, SEQ_3_LIBRE);
    score += SCORE_3 * _3_ + SCORE_PEN;
    _3 = counter(gameState->board, player, SEQ_3_SEMI_LIBRE);
    score += (_3 * (SCORE_3 - SCORE_PEN)) + SCORE_PEN;
    _2_ = counter(gameState->board, player, SEQ_2_LIBRE);
    score += (_2_ * SCORE_2) + SCORE_PEN;
    _2 = counter(gameState->board, player, SEQ_2_SEMI_LIBRE);
    score += (_2 * (SCORE_2 - SCORE_PEN)) + SCORE_PEN;
    score += (nb_prise * SCORE_CAPT) + SCORE_PEN;
    return score;
}