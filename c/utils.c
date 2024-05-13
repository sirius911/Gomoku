/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 18:50:02 by clorin            #+#    #+#             */
/*   Updated: 2024/05/13 11:14:19 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

void print(const char *format, ...){
    if(DEBUG) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void print_stat(const char *format, ...){
    if(STAT) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

// Fonction pour vérifier si une position (x, y) est valide sur le plateau.
bool is_valid_position(int x, int y) {
    return x >= 0 && x < SIZE && y >= 0 && y < SIZE;
}

// Convertit les coordonnées en index dans le tableau
int idx(int x, int y) {
    return y * SIZE + x;
}

void r_idx(int index, int *col, int* row) {
	*col = index % SIZE;
	*row = abs(index / SIZE);
}

char adversaire(const char player){
    return (player == 'W') ? 'B' : 'W';
}

char get(const char *board, int x, int y) {
    /*
        return char in (x,y)
    */
    int index = idx(x, y);

    if (index >= 0 && index < strlen(board)) {
        return board[index];
    }
    return '\0';
}

char *put(char *board, char c, int x, int y) {
    int index = idx(x, y);

    if (index >= 0 && index < strlen(board))
        board[index] = c;
    return board;
}

void print_sequence(const int *seq, int lenght) {
    printf("sequence = {");
    for (int a = 0; a < lenght; a++){
        printf("%d, ", seq[a]);
    }
    printf("}\n");
}

int *seq(const char *board, int x, int y, int dx, int dy, char player, int nb) {
    /*
        return a seq of nb ligne with 0 if empty, 1 if player, 2 if opponent
                X                          X
            1 0 0 0 0 0 0 0   (2,2)      [.1 0 0 0]0 0 0 0  -> [-1, 1, 0, 0]
            0 0 0 0 0 0 0 0   nb=5 ->      0 0 0 0 0 0 0 0  -> [0, 1, 0, 0]
        Y   0 0 1 0 1 0 0 0                0[0 1 0 1 0]0 0
    */

    // Allocation dynamique du tableau pour stocker la séquence
    int *sequence = (int *)malloc(nb * sizeof(int));
    if (sequence == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL; // Retourne NULL en cas d'échec de l'allocation
    }
    memset(sequence, 0, nb * sizeof(int));
    for (int i = -1; i < nb-1; i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        if (is_valid_position(nx, ny)) {
            char stone = get(board, nx, ny);
            if (stone == player) {
                sequence[i + 1] = 1;
            } else if (stone != '\0' && stone != '0') { // Suppose '0' représente une case vide
                sequence[i + 1] = 2;
            }
        }
        else
            sequence[i + 1] = -1; //bord
    }
    return sequence;
}

void print_sequences_board(char *board, const char *entete) {
    int _b2_,_b2,_b3_,_b3,_b4t,_b4_,_b4;
    int _w2_,_w2,_w3_,_w3,_w4t,_w4_,_w4;
    bool b5, w5;

    _b4t = counter(board, 'B', SEQ_4_TROUS);
    _b4_ = counter(board, 'B', SEQ_4_LIBRE);
    _b4  = counter(board, 'B', SEQ_4_SEMI_LIBRE);
    _b3_ = counter(board, 'B', SEQ_3_LIBRE);
    _b3  = counter(board, 'B', SEQ_3_SEMI_LIBRE) - _b4_ - _b4;

    _b2_ = counter(board, 'B', SEQ_2_LIBRE) / 2;
    _b2  = counter(board, 'B', SEQ_2_SEMI_LIBRE) - _b4_ - _b4 - _b3_ - _b3;

    _b4_ /=2;
    _b3_ /=2;
   
    b5   = counter(board, 'B', SEQ_5) >= 1;

    _w4t = counter(board, 'W', SEQ_4_TROUS);
    _w4_ = counter(board, 'W', SEQ_4_LIBRE);
    _w4  = counter(board, 'W', SEQ_4_SEMI_LIBRE);
    _w3_ = counter(board, 'W', SEQ_3_LIBRE);
    _w3  = counter(board, 'W', SEQ_3_SEMI_LIBRE) - _w4_ - _w4;

    _w2_ = counter(board, 'W', SEQ_2_LIBRE) / 2;
    _w2  = counter(board, 'W', SEQ_2_SEMI_LIBRE) - _w4_ - _w4 - _w3_ - _w3;
    
    _w4_ /=2;
    _w3_ /=2;
    
    w5   = counter(board, 'W', SEQ_5) >= 1;

    print("%s  _BB_ : %d -   _BBX : %d\n", entete, _b2_, _b2);
    print("%s  _WW_ : %d -   _WWX : %d\n", entete, _w2_, _w2);
    print("%s _BBB_ : %d -  _BBBX : %d\n", entete, _b3_, _b3);
    print("%s _WWW_ : %d -  _WWWX : %d\n", entete, _w3_, _w3);
    print("%s_BB_B_ : %d\n", entete, _b4t);
    print("%s_WW_W_ : %d\n", entete, _w4t);
    print("%s_BBBB_ : %d - _BBBBX : %d\n", entete, _b4_, _b4);
    print("%s_WWWW_ : %d - _WWWWX : %d\n", entete, _w4_, _w4);
    print("%sBBBBB  : %s\n", entete, b5 ? "True":"False");
    print("%sWWWWW  : %s\n", entete, w5 ? "True":"False");

    print("%s** Conclusion **\n", entete);
    if (b5 || w5){
        print("%svainqueur : %s\n", entete, b5? "Black":"White");
        return;
    }
    if (_b4_ > 0 || _w4_ > 0){
        print("%sAligne 4 (imparable) : %s\n", entete, (_b4_ > 0)? "Black":"white");
        return;
    }
    if (_b4 > 0 || _w4 > 0){
        print("%sDanger alignement 4 : %s\n", entete, (_b4 > 0)? "Black":"white");
        return;
    }
    if (_b3_ > 0 || _w3_ > 0){
        print("%s Attention alignement de 3 libres: %s\n", entete, (_b3_ > 0)? "Black":"white");
        return;
    }
}

void free_moves(Move* moves) {
    if (moves){
        free(moves);
        moves = NULL;
    }
}