/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   essais.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 12:50:04 by clorin            #+#    #+#             */
/*   Updated: 2024/03/27 09:22:48 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

#define SEQ_4_LIBRE "011110"
#define SEQ_4_SEMI_LIBRE "+11110"
#define SEQ_4_TROUS "011010"
#define SEQ_3_LIBRE "01110*"
#define SEQ_3_SEMI_LIBRE "+1110*"
#define SEQ_2_SEMI_LIBRE "011+**"
#define SEQ_2_LIBRE "0110**"


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
                free(sequence);
            }
        }
    }
    return count;
}

// int count_seq_4_libre(const char *board, char player){
//     const char good[] = "011110";
//     return counter(board, player, good);
// }

// int count_seq_4_semi_libre(const char *board, char player){
//     const char good[] = "+11110";
//     return counter(board, player, good);
// }

int count_seq_4_trous(const char *board, char player){
    const char good[] = "011010"; 
    return counter(board, player, good);
}

// int count_seq_3_libre(const char *board, char player){
//     const char good[] = "01110*";
//     return counter(board, player, good)/2;
// }

// int count_seq_3_semi_libre(const char *board, char player){
//     const char good[] = "+1110*";
//     return counter(board, player, good);
// }

// int count_seq_2_semi_libre(const char *board, char player){
//     //sans distingo libre semi-libre
//     const char good[] = "011+**";
//     return counter(board, player, good);
// }

// int count_seq_2_libre(const char *board, char player){
//     const char good[] = "0110**";
//     return counter(board, player, good)/2;
// }