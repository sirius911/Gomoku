/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thoberth <thoberth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 18:50:02 by clorin            #+#    #+#             */
/*   Updated: 2024/03/19 20:57:32 by thoberth         ###   ########.fr       */
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
    int b2,b3,b4;
    int w2,w3,w4;
    bool b5, w5;
    b2 = count_sequences(board, 'B', 2);
    b3 = count_sequences(board, 'B', 3);
    b4 = count_sequences(board, 'B', 4);
    b5 = (count_sequences(board, 'B', 5) > 0);
    w2 = count_sequences(board, 'W', 2);
    w3 = count_sequences(board, 'W', 3);
    w4 = count_sequences(board, 'W', 4);
    w5 = (count_sequences(board, 'W', 5) > 0);
    print("%sBlack [2]:%d - [3]:%d = [4]:%d",entete,b2,b3,b4);
    print(" %s", b5 ? "Winner\n":"\n");
    print("%sWhite [2]:%d - [3]:%d = [4]:%d\n",entete,w2,w3,w4);
    print(" %s", w5 ? "Winner\n":"\n");
}

void print_board(const char* board, const char current_player){
	print("Current player = %c\n", current_player);
	for (int i = 0; board[i]; i++)
	{
		if (i % SIZE == 0)
			print("\n");
		print("%c ", board[i]);
	}
	print("\n\n");
}

void free_moves(Move* moves) {
    if (moves)
        free(moves);
}