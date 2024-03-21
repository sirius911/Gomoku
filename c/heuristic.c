/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heuristic.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thoberth <thoberth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 19:33:01 by thoberth          #+#    #+#             */
/*   Updated: 2024/03/21 12:14:07 by thoberth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int heuristic(Move *move, const char current_player, char *board, int index){
	/**
	 * This function return a score depending on:
	 * Sequences this move can create or continue
	*/
	int best_sequence = 0;
	// print("Coordonnees = col : %d, row : %d\n", move->col, move->row);
	// for (int i = 0; i < SIZE; i++){
	// 	for (int j = 0; map[i][j]; j++) {
	// 		print("%c ", map[i][j]);
	// 	}
	// 	print("\n");
	// }

	int col = move->col, row = move->row, sequence = 0;
	
	// HORIZONTALLY
	while ((index >= (row * SIZE)) && board[index] == current_player) {
		sequence++;
		index--;
	}
	index = (row * SIZE) + col + 1;
	while ((index < ((row + 1) * SIZE)) && board[index] == current_player)
	{
		sequence++;
		index++;
	}
	if (sequence >= 5)
		return sequence;
	if (sequence > best_sequence)
		best_sequence = sequence;

	// VERTICALLY
	sequence = 0, index = idx(move->col, move->row);
	while (index >= 0 && board[index] == current_player) {
		sequence++;
		index -= SIZE;
	}
	index = (row + 1) * SIZE + col;
	while ((index < (SIZE * SIZE)) && board[index] == current_player) {
		sequence++;
		index += SIZE;
	}
	if (sequence >= 5)
		return sequence;
	if (sequence > best_sequence)
		best_sequence = sequence;

	// DIAGONALLY (from top left to bottom right)
	sequence = 0, index = idx(move->col, move->row);
	while ((index >= 0) && (index >= (row * SIZE)) && board[index] == current_player)
	{
		sequence++;
		index = (index - SIZE) - 1;
		row--;
	}
	row = move->row + 1;
	col = move->col + 1;
	index = idx(col, row);
	while ((index < (SIZE * SIZE)) && (index < ((row + 1) * SIZE)) && board[index] == current_player)
	{
		sequence++;
		row++;
		index = (index + SIZE) + 1;
	}
	if (sequence >= 5)
		return sequence;
	if (sequence > best_sequence)
		best_sequence = sequence;

	// DIAGONALLY (from top right to bottom left)
	col = move->col, row = move->row, sequence = 0, index = idx(move->col, move->row);
	while (row >= 0 && col < SIZE && board[index] == current_player)
	{
		sequence++;
		row--;
		col++;
		index = (index - SIZE) + 1;
	}
	row = move->row + 1;
	col = move->col - 1;
	index = idx(col, row);
	while (row < SIZE && col >= 0 && board[index] == current_player)
	{
		sequence++;
		row++;
		col--;
		index = (index + SIZE) - 1;
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

char** create_map(const char* copie_board){
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
			if (copie_board[index] == 'X')
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