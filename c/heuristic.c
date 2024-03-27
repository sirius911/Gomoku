/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heuristic.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thoberth <thoberth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/15 19:33:01 by thoberth          #+#    #+#             */
/*   Updated: 2024/03/25 17:07:23 by thoberth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

int heuristic(Move *move, const char current_player, char *board, int index){
	/**
	 * This function return a score depending on:
	 * Sequences this move can create or continue
	*/
	int score_total = 0;

	char opponent_player = (current_player=='B')?'W':'B';
	int col = move->col, row = move->row, sequence = 0;
	int extrem1 = 0, extrem2 = 0;

	// HORIZONTALLY
	while ((index >= (row * SIZE)) && board[index] == current_player) {
		sequence++;
		index--;
	}
	while ((index >= (row * SIZE)) && board[index] != opponent_player){
		extrem1++;
		index--;
	}
	index = (row * SIZE) + col + 1;
	while ((index < ((row + 1) * SIZE)) && board[index] == current_player) {
		sequence++;
		index++;
	}
	while ((index < ((row + 1) * SIZE)) && board[index] != opponent_player){
		extrem2++;
		index++;
	}
	if ((sequence = verif_sequence(sequence, extrem1, extrem2)) == WIN_MOVE)
		return WIN_MOVE;
	score_total += sequence;

	// VERTICALLY
	sequence = 0, index = idx(move->col, move->row);
	extrem1 = 0, extrem2 = 0;
	while (index >= 0 && board[index] == current_player) {
		sequence++;
		index -= SIZE;
	}
	while (index >= 0 && board[index] != opponent_player){
		extrem1++;
		index -= SIZE;
	}
	index = (row + 1) * SIZE + col;
	while ((index < (SIZE * SIZE)) && board[index] == current_player) {
		sequence++;
		index += SIZE;
	}
	while ((index < (SIZE * SIZE)) && board[index] != opponent_player){
		extrem2++;
		index += SIZE;
	}
	if ((sequence = verif_sequence(sequence, extrem1, extrem2)) == WIN_MOVE)
		return WIN_MOVE;
	score_total += sequence;

	// DIAGONALLY (from top left to bottom right)
	sequence = 0, index = idx(move->col, move->row);
	extrem1 = 0, extrem2 = 0;
	while ((index >= 0) && (index >= (row * SIZE)) && board[index] == current_player)
	{
		sequence++;
		index = (index - SIZE) - 1;
		row--;
	}
	while ((index >= 0) && (index >= (row * SIZE)) && board[index] != opponent_player) {
		extrem1++;
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
	while ((index < (SIZE * SIZE)) && (index < ((row + 1) * SIZE)) && board[index] != opponent_player){
		extrem2++;
		row++;
		index = (index + SIZE) + 1;
	}
	if ((sequence = verif_sequence(sequence, extrem1, extrem2)) == WIN_MOVE)
		return WIN_MOVE;
	score_total += sequence;

	// DIAGONALLY (from top right to bottom left)
	extrem1 = 0, extrem2 = 0;
	col = move->col, row = move->row, sequence = 0, index = idx(move->col, move->row);
	while (row >= 0 && col < SIZE && board[index] == current_player)
	{
		sequence++;
		row--;
		col++;
		index = (index - SIZE) + 1;
	}
	while (row >= 0 && col < SIZE && board[index] != opponent_player) {
		extrem1++;
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
	while (row < SIZE && col >= 0 && board[index] != opponent_player){
		extrem2++;
		row++;
		col--;
		index = (index + SIZE) - 1;
	}
	if ((sequence = verif_sequence(sequence, extrem1, extrem2)) == WIN_MOVE)
		return WIN_MOVE;
	score_total += sequence;
	return score_total;
}

// Fonction de comparaison pour qsort
int compare_age(const void *a, const void *b)
{
	const Move *moveA = (const Move *)a;
	const Move *moveB = (const Move *)b;

	// Compare les scores
	return (moveB->score - moveA->score);
}

bool check_capture_score(char *board, Move *move, char current_player, char opponent_player) {
	int col = move->col, row = move->row, index = idx(col, row);

	// VERTICALLY
	if (((index - 3) >= (row * SIZE)) && board[index - 3] == current_player) {
		if (board[index - 2] == opponent_player && board[index - 1] == opponent_player) {
			return true;
		}
	}
	if (((index + 3) < ((row + 1) * SIZE)) && board[index + 3] == current_player) {
		if (board[index + 2] == opponent_player && board[index + 1] == opponent_player) {
			return true;
		}
	}

	// HORIZONTALLY
	if (((index - (3 * SIZE)) >= 0) && board[index - (3 * SIZE)] == current_player) {
		if (board[index - (2 * SIZE)] == opponent_player && board[index - SIZE] == opponent_player) {
			return true;
		}
	}
	if (((index + (3 * SIZE)) < (SIZE * SIZE)) && board[index + (3 * SIZE)] == current_player) {
		if (board[index + (2 * SIZE)] == opponent_player && board[index + SIZE] == opponent_player) {
			return true;
		}
	}

	// DIAGONALLY
	if ((col - 3) >= 0 && (row - 3) >= 0 && board[index - (3 * SIZE) - 3] == current_player) {
		if (board[index - (2 * SIZE) - 2] == opponent_player && board[index - SIZE - 1] == opponent_player) {
			return true;
		}
	}
	if ((col + 3) < SIZE && (row + 3) < SIZE && board[index + (3 * SIZE) + 3] == current_player) {
		if (board[index + (2 * SIZE) + 2] == opponent_player && board[index + SIZE + 1] == opponent_player) {
			return true;
		}
	}

	if ((col + 3) < SIZE && (row - 3) >= 0 && board[index - (3 * SIZE) + 3] == current_player) {
		if (board[index - (2 * SIZE) + 2] == opponent_player && board[index - SIZE + 1] == opponent_player) {
			return true;
		}
	}
	if ((col - 3) >= 0 && (row + 3) < SIZE && board[index + (3 * SIZE) - 3] == current_player) {
		if (board[index + (2 * SIZE) - 2] == opponent_player && board[index + SIZE - 1] == opponent_player) {
			return true;
		}
	}
	return false;
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

int verif_sequence(int sequence, int extrem1, int extrem2) {
	if (sequence >= 5)
		return WIN_MOVE;
	if (sequence == 2 && (extrem1 + extrem2) >= 3)
		return 10;
	else if (sequence == 3 && (extrem1 + extrem2) >= 2)
		return 50;
	else if (sequence == 4 && (extrem1 || extrem2))
	{
		if (extrem1 && extrem2)
			return 300;
		else
			return 200;
	}
	return 0;
}
