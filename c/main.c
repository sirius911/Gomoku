#include "game.h"
#include <time.h>
#include <limits.h>
#include <float.h>

void header(void){
    for(int i=1; i <= 5; i++){
        printf("                  %d ", i);
    }
    printf("\n");
    for(int i=0; i < 5; i++){
        printf("1 2 3 4 5 6 7 8 9 0 ");
    }
    printf("\n");
    fflush(stdout);
}

GameState* init_game_state() {
    GameState *gameState = malloc(sizeof(GameState));
    if (gameState == NULL) {
        perror("Failed to allocate memory for game state");
        exit(EXIT_FAILURE);
    }

    gameState->board = malloc(SIZE * SIZE * sizeof(char) + 1);
    if (gameState->board == NULL) {
        perror("Failed to allocate memory for board");
        free(gameState);
        exit(EXIT_FAILURE);
    }

    int i = 0;
    for (i = 0; i < SIZE * SIZE; i++) {
        gameState->board[i] = '0';
    }
    gameState->board[i] = '\0';
    gameState->captures[0] = 0;
    gameState->captures[1] = 0;
    gameState->currentPlayer = 'B'; // 'B' commence
    return gameState;
}

void game_loop(GameState *gameState, int depth, bool debug, bool stat, bool threads) {
    char winner = '0';
    int nb_coup = 0;
    double min_time = DBL_MAX;
    double max_time = 0;
    double total_time = 0;
    struct timespec start_time, end_time, begin_game, end_game;
    Move move;
    header();
    // Démarrer le chronomètre pour la partie entière
    clock_gettime(CLOCK_MONOTONIC, &begin_game);
    while (!game_over(gameState, &winner)) {
        // Démarrer le chronomètre pour le coup actuel
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        if (threads)
            move = play_IA_threads(gameState, depth, debug);
        else
            move = play_IA(gameState, depth, debug, stat);
        // Arrêter le chronomètre pour le coup actuel
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        if (is_valid_position(move.col, move.row))
            gameState->board[idx(move.col, move.row)] = gameState->currentPlayer;
        else
            break;
        Move *captured = check_capture(gameState->board, move.col, move.row);
        if (captured){
            gameState->board = del_captured(gameState->board, captured);
            if (gameState->currentPlayer == 'B')
                gameState->captures[0] += 2;
            else
                gameState->captures[1] += 2;
            free_moves(captured); 
        }
 
        double coup_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        total_time += coup_time;
        if (coup_time < min_time) min_time = coup_time;
        if (coup_time > max_time) max_time = coup_time;

        printf("%c.", gameState->currentPlayer);
        fflush(stdout);
        gameState->currentPlayer = adversaire(gameState->currentPlayer);
        nb_coup++;
    }
    // Arrêter le chronomètre pour la partie entière
    clock_gettime(CLOCK_MONOTONIC, &end_game);
    
    double game_duration = (end_game.tv_sec - begin_game.tv_sec) + (end_game.tv_nsec - begin_game.tv_nsec) / 1e9;
    double average_time = nb_coup > 0 ? total_time / nb_coup : 0;
    printf("\nWinner is %c\n", winner);
    printf("Temps moyen pour une partie de %d coups (ia=%d) Threads=%s => %.2f s/coup Max = %.2fs Min = %.2fs durée partie = %.2fs",
    nb_coup, depth, threads ? "True" : "False", average_time, max_time, min_time, game_duration);
}


int main(int argc, char *argv[]) {
    int ia_level = 1; // Niveau d'IA par défaut
    bool threads = false; // Désactivé par défaut
    bool debug = false; // Débogage désactivé par défaut
    int opt;

    while ((opt = getopt(argc, argv, "i:td")) != -1) {
        switch (opt) {
            case 'i':
                ia_level = atoi(optarg);
                break;
            case 't':
                threads = true;
                break;
            case 'd':
                debug = true; // Activer le débogage
                break;
            default:
                fprintf(stderr, "Usage: %s [-i ia_level] [-t] [-d]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Affichez les options sélectionnées pour le débogage
    printf("IA Level: %d, Threads: %s, Debug: %s\n", 
           ia_level, 
           threads ? "Enabled" : "Disabled", 
           debug ? "Enabled" : "Disabled");
    GameState *gameState = init_game_state();
    game_loop(gameState, ia_level, debug, false, threads);
    free_gameState(gameState);
    return 0;
}
