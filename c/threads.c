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


void *evaluate_move(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->result = minmax(data->gameState, data->depth, data->alpha, data->beta, data->maximizingPlayer, data->move.col, data->move.row, data->depth);
    pthread_exit(NULL);
}

Move play_IA_threads(GameState *gameState, int depth, bool debug) {
    DEBUG = debug;
    int best_score = MIN_EVAL;
    Move best_move = {-1, -1};
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = proximate_moves(gameState->board, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY);

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

        if (threadData[i].result.coup_gagnant || threadData[i].result.scoreDiff > best_score) {
            best_score = threadData[i].result.scoreDiff;
            best_move = moves[i];
            if (threadData[i].result.coup_gagnant) {
                break;
            }
        }
    }

    free(moves);
    return best_move;
}