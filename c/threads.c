#include "game.h"
#include <pthread.h>

typedef struct {
    GameState *gameState;
    Move move;
    int depth;
    int alpha;
    int beta;
    bool maximizingPlayer;
    EvalResult result;
} ThreadData;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
bool found_winning_move = false;

void *evaluate_move(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    pthread_mutex_lock(&lock);
    bool early_exit = found_winning_move; // Vérifiez si un coup gagnant a été trouvé.
    pthread_mutex_unlock(&lock);

    if (!early_exit) {
        data->result = minmax(data->gameState, data->depth, data->alpha, data->beta, data->maximizingPlayer, data->move.col, data->move.row, data->depth);
        // Si un coup gagnant est trouvé, mettez à jour la variable partagée.
        if (data->result.coup_gagnant) {
            pthread_mutex_lock(&lock);
            found_winning_move = true;
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_exit(NULL);
}

Move play_IA_threads(GameState *gameState, int depth, bool debug) {
    DEBUG = debug;
    int best_score = MIN_EVAL;
    Move best_move = {-1, -1};
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = proximate_moves(gameState, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY);

    pthread_t threads[move_count];
    ThreadData threadData[move_count];

    for (int i = 0; i < move_count; i++) {
        threadData[i].gameState = gameState;
        threadData[i].move = moves[i];
        threadData[i].depth = depth;
        threadData[i].alpha = MIN_EVAL;
        threadData[i].beta = MAX_EVAL;
        threadData[i].maximizingPlayer = true;
        
        if(pthread_create(&threads[i], NULL, evaluate_move, &threadData[i])) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }
    print("%d threads crées\n", move_count);
    for (int i = 0; i < move_count; i++) {
        pthread_join(threads[i], NULL);
        pthread_mutex_lock(&lock);
        if (found_winning_move) {
            best_score = threadData[i].result.scoreDiff;
            best_move = moves[i];
            pthread_mutex_unlock(&lock);
            break; // Sortie anticipée si un coup gagnant a été trouvé.
        }
        pthread_mutex_unlock(&lock);
        if (threadData[i].result.scoreDiff > best_score) {
            best_score = threadData[i].result.scoreDiff;
            best_move = moves[i];
        }
    }

    free(moves);
    return best_move;
}