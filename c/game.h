/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thoberth <thoberth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 18:56:58 by clorin            #+#    #+#             */
/*   Updated: 2024/03/16 11:48:15 by thoberth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// game.h
#ifndef GAME_H
#define GAME_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>

#define SIZE 19
#define MAX_MOVES (SIZE * SIZE)
#define MAX_EVAL INT_MAX
#define MIN_EVAL INT_MIN

extern bool DEBUG;

typedef struct {
    int col;
    int row;
	int score;
} Move;

typedef struct {
    int scoreDiff;
    int playerScore;
    int opponentScore;
    Move coup;
    bool coup_gagnant;
} EvalResult;

typedef struct GameState {
    char *board;
    int  captures[2];
    char currentPlayer;
} GameState;

// Prototypes des fonctions utils
void print(const char *format, ...);
bool is_valid_position(int x, int y);
int idx(int x, int y);
char adversaire(const char player);
char get(const char *board, int x, int y);
char *put(char *board, char c, int x, int y);
int *seq(const char *board, int x, int y, int dx, int dy, char player, int nb);
void print_sequence(const int *seq, int lenght);
void print_sequences_board(char *board, const char *entete);
void print_board(const char *board, const char current_player);
void free_moves(Move *moves);

// Prototypes des fonctions game_logic
void findBoxElements(const char *board, int *topLeftX, int *topLeftY, int *bottomRightX, int *bottomRightY);
bool game_over(const GameState *gameState, char *winner);
Move *check_capture(const char* board, int x, int y);
char *del_captured(char *board, Move *captured);
bool is_three(const char *board, int x, int y, int dx, int dy, char player);
bool check_double_three(char *board, int x, int y, char player);
bool isAlignment(const char *board, int x, int y, char current_player);
int count_sequences(const char *board, char player, int base_taille_seq);

// Prototypes des fonctions ai_logic
EvalResult minmax(GameState *gameState, int depth, int alpha, int beta, bool maximizingPlayer, int currentMoveX, int currentMoveY);
Move play_IA(GameState *gameState, int depth, bool debug);
void analyse(GameState *gameState, bool debug);
int score_move(char *copie_board, Move *move, const char current_player);
Move *generate_possible_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2);
Move* proximate_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2);

// Protoypes des fonctions dqns SandBox.c
int nb_coups(GameState *gameState);

// Protoypes des fonctions dans heuristic.c
int heuristic(const char *copie_board, Move *move, const char current_player);
char **create_map(const char *copie_board, int col, int row, const char current_player);
void free_map(char **map);

#endif // GAME_H
