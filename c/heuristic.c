/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heuristic.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thoberth <thoberth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 19:33:01 by thoberth          #+#    #+#             */
/*   Updated: 2024/03/16 12:35:00 by thoberth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int heuristic(const char *copie_board, Move *move, const char current_player){
	/**
	 * This function return a score depending on:
	 * Sequences this move can create or continue
	*/
	// int index_board = idx(move->col, move->row);
	int best_sequence = 1;
	char **map = create_map(copie_board, move->col, move->row, current_player); // deplacer cette ligne dans la fonction apellante

	// VERTICALLY
	int col = move->col, row = move->row, sequence = 1;
	while (col >= 0 && map[col][row] == current_player)
	{
		sequence++;
		col--;
	}
	col = move->col;
	while (col < SIZE && map[col][row] == current_player)
	{
		sequence++;
		col++;
	}
	print("sequences verticalle %d\n", sequence);

	free_map(map);
	return 0;
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
			if (col == x && row == y)
				map[x][y] = current_player;
			else if (copie_board[index] == 'X')
				map[x][y] = '0';
			else
				map[x][y] = copie_board[index];
			index++;
		}
	}
	// print("Coordonnees = %d %d\n", col, row);
	// for (int r = 0; r < SIZE; r++){
	// 	print("%s\n", map[r]);
	// }
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