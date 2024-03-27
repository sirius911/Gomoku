/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 18:56:58 by clorin            #+#    #+#             */
/*   Updated: 2024/03/27 09:23:23 by clorin           ###   ########.fr       */
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
#define WIN_MOVE -1

#define SEQ_4_LIBRE "011110"
#define SEQ_4_SEMI_LIBRE "+11110"
#define SEQ_4_TROUS "011010"
#define SEQ_3_LIBRE "01110*"
#define SEQ_3_SEMI_LIBRE "+1110*"
#define SEQ_2_SEMI_LIBRE "011+**"
#define SEQ_2_LIBRE "0110**"

extern bool DEBUG;  
extern bool STAT;

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
    bool coup_perdant;
} EvalResult;

typedef struct GameState {
    char *board;
    int  captures[2];
    char currentPlayer;
} GameState;

// Prototypes des fonctions utils
void print(const char *format, ...);
void print_stat(const char *format, ...);
bool is_valid_position(int x, int y);
int idx(int x, int y);
void r_idx(int index, int *col, int *row);
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
void    free_gameState(GameState *game);
int _evaluate_player(const GameState *gameState, char player);
int _evaluate_opponent(const GameState *gameState, char opponent);
GameState *apply_move(const GameState *original_gameState, int x, int y);
EvalResult minmax(GameState *gameState, int depth, int alpha, int beta, bool maximizingPlayer, int currentMoveX, int currentMoveY, int maxDepth);
Move play_IA(GameState *gameState, int depth, bool debug, bool stat);
Move play_IA_threads(GameState *gameState, int depth, bool debug);
void analyse(GameState *gameState, bool debug);
bool score_move(GameState* gameState, char *board, int index, Move *move, const char current_player);
Move *generate_possible_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2);
Move* proximate_moves(GameState *gameState, int *move_count, const char current_player, int x1, int y1, int x2, int y2);

// Protoypes des fonctions dqns SandBox.c
int nb_coups(GameState *gameState);
int value_coup(GameState *gameState, int currentMoveX, int currentMoveY);
int value_coup2(GameState *gameState, int currentMoveX, int currentMoveY);

// Protoypes des fonctions dans heuristic.c
int heuristic(Move *move, const char current_player, char *board, int index);
bool check_capture_score(char *board, Move *move, char current_player, char opponent_player);
int compare_age(void const *a, void const *b);
char **create_map(const char *copie_board);
void free_map(char **map);

// fichiers essais.c
int counter(const char *board, const char player, const char good[6]);
int count_seq_4_trous(const char *board, char player);

#endif // GAME_H
