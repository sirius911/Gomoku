/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heuristic.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thoberth <thoberth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 19:33:01 by thoberth          #+#    #+#             */
/*   Updated: 2024/03/17 23:29:26 by thoberth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int heuristic(const char *copie_board, Move *move, const char current_player, char **map){
	/**
	 * This function return a score depending on:
	 * Sequences this move can create or continue
	*/
	int best_sequence = 1;
	// print("Coordonnees = col : %d, row : %d\n", move->col, move->row);
	// for (int i = 0; i < SIZE; i++){
	// 	for (int j = 0; map[i][j]; j++) {
	// 		print("%c ", map[i][j]);
	// 	}
	// 	print("\n");
	// }

	// HORIZONTALLY
	int col = move->col, row = move->row, sequence = 0;
	while (col >= 0 && map[row][col] == current_player) {
		sequence++;
		col--;
	}
	col = move->col + 1;
	while (col < SIZE && map[row][col] == current_player) {
		sequence++;
		col++;
	}
	if (sequence > best_sequence)
		best_sequence = sequence;

	// VERTICALLY
	col = move->col, row = move->row, sequence = 0;
	while (row >= 0 && map[row][col] == current_player) {
		sequence++;
		row--;
	}
	row = move->row + 1;
	while (row < SIZE && map[row][col] == current_player) {
		sequence++;
		row++;
	}
	if (sequence > best_sequence)
		best_sequence = sequence;

	// DIAGONALLY (from top left to bottom right)
	col = move->col, row = move->row, sequence = 0;
	while (row >= 0 && col >= 0 && map[row][col] == current_player)
	{
		sequence++;
		row--;
		col--;
	}
	row = move->row + 1;
	col = move->col + 1;
	while (row < SIZE && col < SIZE && map[row][col] == current_player)
	{
		sequence++;
		row++;
		col++;
	}
	if (sequence > best_sequence)
		best_sequence = sequence;

	// DIAGONALLY (from top right to bottom left)
	col = move->col, row = move->row, sequence = 0;
	while (row >= 0 && col < SIZE && map[row][col] == current_player)
	{
		sequence++;
		row--;
		col++;
	}
	row = move->row + 1;
	col = move->col - 1;
	while (row < SIZE && col >= 0 && map[row][col] == current_player)
	{
		sequence++;
		row++;
		col--;
	}
	if (sequence > best_sequence)
		best_sequence = sequence;

	return best_sequence;
}

// Fonction de comparaison pour qsort
int compare_age(const void *a, const void *b)
{
	const Move *moveA = (const Move *)a;
	const Move *moveB = (const Move *)b;

	// Compare les scores
	return (moveB->score - moveA->score);
}

char** create_map(const char* copie_board, int col, int row, const char current_player){
	char **map = (char**)malloc(SIZE * sizeof(char *));
	if(map == NULL) {
		fprintf(stderr, "Allocation de mémoire échouée\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < SIZE; i++) {
		map[i] = (char*)malloc(SIZE * sizeof(char));
		if (map[i] == NULL) {
			fprintf(stderr, "Allocation de mémoire échouée\n");
			exit(EXIT_FAILURE);
		}
	}
	int index = 0;
	for (int x = 0; x < SIZE; x++){
		for (int y = 0; y < SIZE; y++){
			if (col == y && row == x)
				map[x][y] = current_player;
			else if (copie_board[index] == 'X')
				map[x][y] = '0';
			else
				map[x][y] = copie_board[index];
			index++;
		}
	}
	return map;
}

void free_map(char **map){
	// Libération de la mémoire
	for (int i = 0; i < SIZE; i++)
	{
		free(map[i]);
	}
	free(map);
}