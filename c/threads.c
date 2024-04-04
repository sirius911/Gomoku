#include "game.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>


// Structure pour passer des données aux threads
typedef struct {
    GameState *gameState;
    Move move;
    int depth;
    int alpha;
    int beta;
    bool maximizingPlayer;
    EvalResult result;
} ThreadData;

// Fonction pour copier l'état du jeu
GameState *copy_game_state(const GameState *src) {
    GameState *dst = malloc(sizeof(GameState));
    if (!dst) {
        perror("Failed to allocate memory for game state copy");
        exit(EXIT_FAILURE);
    }
    *dst = *src; // Copie superficielle des champs simples
    dst->board = malloc((SIZE * SIZE + 1) * sizeof(char));
    if (!dst->board) {
        perror("Failed to allocate memory for board copy");
        free(dst);
        exit(EXIT_FAILURE);
    }
    memcpy(dst->board, src->board, (SIZE * SIZE + 1) * sizeof(char)); // Copie profonde du plateau
    return dst;
}

// Fonction exécutée par chaque thread pour évaluer un mouvement
void *evaluate_move(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    GameState *newGameState = apply_move(data->gameState, data->move.col, data->move.row);
    data->result = minmax(newGameState, data->depth, data->alpha, data->beta, data->maximizingPlayer, data->move.col, data->move.row);
    free_gameState(newGameState); // Libérer la copie du GameState
    pthread_exit(NULL);
}

Move play_IA_threads(GameState *gameState, int depth, bool debug) {
    DEBUG = debug;
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = proximate_moves(gameState, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY);

    pthread_t threads[move_count];
    ThreadData threadData[move_count];

    // Initialiser et lancer les threads
    print("Création de %d threads\n", move_count);
    for (int i = 0; i < move_count; i++) {
        threadData[i] = (ThreadData){gameState, moves[i], (depth * 2) - 1, MIN_EVAL, MAX_EVAL, false};
        if (pthread_create(&threads[i], NULL, evaluate_move, &threadData[i]) != 0) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }
    // Attendre la fin de tous les threads
    for (int i = 0; i < move_count; i++) {
        pthread_join(threads[i], NULL);
    }
    EvalResult bestEval = {.scoreDiff = MIN_EVAL};
    Move best_move = {-1, -1};
    for (int i = 0; i < move_count; i++) {
        if (threadData[i].result.coup_gagnant){
            bestEval = threadData[i].result;
            best_move = threadData[i].move;
            break;
        }
        if (threadData[i].result.scoreDiff > bestEval.scoreDiff) {
            bestEval = threadData[i].result;
            best_move = threadData[i].move;
        }
    }
    free_moves(moves);
    return best_move;
}