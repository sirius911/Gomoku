#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define SIZE 19
#define MAX_MOVES (SIZE * SIZE)
// Utiliser INT_MIN et INT_MAX pour représenter -inf et +inf
#define MAX_EVAL INT_MAX
#define MIN_EVAL INT_MIN

typedef struct {
    int col;
    int row;
} Move;

typedef struct {
    int scoreDiff;
    int playerScore;
    int opponentScore;
    Move coup;
} EvalResult;

// compilation : gcc -dynamiclib -o libgame.dylib essais.c

void print_board(char* board) {
    printf("Board: %s\n", board);
}

// Fonction pour vérifier si une position (x, y) est valide sur le plateau.
bool is_valid_position(int x, int y) {
    return x >= 0 && x < SIZE && y >= 0 && y < SIZE;
}

// Convertit les coordonnées en index dans le tableau
int idx(int x, int y) {
    return y * SIZE + x;
}


char get(const char *board, int x, int y) {
    /*
        return char in (x,y)
    */
    int index = idx(x, y);

    if (index >= 0 && index < strlen(board)) {
        return board[index];
    }
    return '\0';
}

char *put(char *board, char c, int x, int y) {
    int index = idx(x, y);

    if (index >= 0 && index < strlen(board))
        board[index] = c;
    return board;
}

void print_sequence(const int *seq, int lenght) {
    printf("sequence = {");
    for (int a = 0; a < lenght; a++){
        printf("%d, ", seq[a]);
    }
    printf("}\n");
}

int *seq(const char *board, int x, int y, int dx, int dy, char player, int nb) {
    /*
        return a seq of nb ligne with 0 if empty, 1 if player, 2 if opponent
                X                          X
            1 0 0 0 0 0 0 0   (2,2)      [.1 0 0 0]0 0 0 0  -> [-1, 1, 0, 0]
            0 0 0 0 0 0 0 0   nb=5 ->      0 0 0 0 0 0 0 0  -> [0, 1, 0, 0]
        Y   0 0 1 0 1 0 0 0                0[0 1 0 1 0]0 0
    */

    // Allocation dynamique du tableau pour stocker la séquence
    int *sequence = (int *)malloc(nb * sizeof(int));
    if (sequence == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return NULL; // Retourne NULL en cas d'échec de l'allocation
    }
    memset(sequence, 0, nb * sizeof(int));
    for (int i = -1; i < nb-1; i++) {
        int nx = x + dx * i;
        int ny = y + dy * i;
        if (is_valid_position(nx, ny)) {
            char stone = get(board, nx, ny);
            // printf("(%d,%d)->%c - ", nx, ny, stone);
            if (stone == player) {
                sequence[i + 1] = 1;
            } else if (stone != '\0' && stone != '0') { // Suppose '0' représente une case vide
                sequence[i + 1] = 2;
            }
        }
        else
            sequence[i + 1] = -1; //bord
    }
    return sequence;
}

bool is_three(const char *board, int x, int y, int dx, int dy, char player) {
    // int sequence[6];
    // memset(sequence, 0, sizeof(sequence));
    // Les séquences de "trois" autorisées
    int three_free[5][6] = {
        {0,1,1,1,0,0},
        {0,1,1,1,0,1},
        {0,1,1,1,0,2},
        {0,1,0,1,1,0},
        {0,1,1,0,1,0}
    };

    int *sequence = seq(board, x, y, dx, dy, player, 6);
    if (!sequence)
        return false;
    // print_sequence(sequence);
    // Vérifie si la séquence correspond à une des séquences de "trois" autorisées
    for (int i = 0; i < 5; i++) {
        bool match = true;
        for (int j = 0; j < 6; j++) {
            if (sequence[j] != three_free[i][j]) {
                match = false;
                break;
                free(sequence);
                return true;
            }
        }
        if (match) {
            free(sequence);
            return true;
        }
    }
    free(sequence);
    return false;
}

bool check_double_three(char *board, int x, int y, char player) {
    int directions[6][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {-1, -1}
    };
    int three_count = 0;
    int index = idx(x, y);
    board = put(board, player, x, y);
    for (int i = 0; i < 6; i++) {
        if (is_three(board, x, y, directions[i][0], directions[i][1], player)) {
            three_count++;
            if (three_count > 1) {
                // write(1, "detecte\n", 8);
                board = put(board, '0', x, y);
                return true;
            }
        }
    }
    board = put(board, '0', x, y);
    return false;
}

// Fonction pour scanner le plateau et trouver les éléments les plus extrêmes
void findBoxElements(const char *board, int *topLeftX, int *topLeftY, int *bottomRightX, int *bottomRightY) {
    *topLeftX = SIZE; // Initialisé à une valeur hors limites pour la comparaison
    *topLeftY = SIZE;
    *bottomRightX = -1; // Initialisé à une valeur hors limites pour la comparaison
    *bottomRightY = -1;

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y * SIZE + x] != '0') {
                // Mise à jour des coordonnées de l'élément le plus haut à gauche
                if (y < *topLeftY || (y == *topLeftY && x < *topLeftX)) {
                    *topLeftX = x;
                    *topLeftY = y;
                }
                // Mise à jour des coordonnées de l'élément le plus bas à droite
                if (y > *bottomRightY || (y == *bottomRightY && x > *bottomRightX)) {
                    *bottomRightX = x;
                    *bottomRightY = y;
                }
            }
        }
    }
    *topLeftX -= 1;
    *topLeftY -= 1;
    *bottomRightX += 1;
    *bottomRightY += 1;

    if (*topLeftX < 0)
        *topLeftX = 0;
    if (*topLeftY < 0)
        *topLeftY =0;
    if (*bottomRightX >= SIZE)
        *bottomRightX = SIZE-1;
    if (*bottomRightY >= SIZE)
        *bottomRightY = SIZE-1;
}

Move* generate_possible_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2) {
    /*
        return a array of struc Move with possible moves in (x1,y1)(x2,y2)
    */
    int maxMoves = abs(x2 - x1 + 1) * abs(y2 - y1 + 1);
    Move *moves = (Move*) malloc(maxMoves * sizeof(Move));
    int count = 0;

    for (int row = y1; row <= y2; ++row) {
        for (int col = x1; col <= x2; ++col) {
            int index = idx(col, row);
            if (board[index] == '0' && ! check_double_three(board, col, row, current_player)) { // '0' représente une case vide
                if (count < MAX_MOVES) {
                    moves[count].col = col;
                    moves[count].row = row;
                    count++;
                }
            }
        }
    }

    *move_count = count;
    return moves;
}

// La fonction générique pour compter les séquences de pierres de taille spécifique.
int count_sequences(const char *board, char player, int base_taille_seq) {
    const int nb_dir = 8;
    const int nb_good = 3;
    const int taille_seq = base_taille_seq + 2; // Ajustement pour inclure les espaces de contrôle
    int directions[nb_dir][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };

    // Ajuster la définition des goods pour la nouvelle taille_seq
    int good[nb_good][taille_seq];
    // ligne 0
    int pos = 0;
    good[0][pos++] = 0; // L'espace vide avant la séquence de pierres
    for (int i = 0; i < base_taille_seq; i++)
        good[0][pos++] = 1; // Les pierres du joueur
    good[0][pos] = 0; // L'espace vide après la séquence de pierres
    // ligne 1
    pos = 0;
    good[1][pos++] = 0; // L'espace vide avant la séquence de pierres
    for (int i = 0; i < base_taille_seq; i++)
        good[1][pos++] = 1; // Les pierres du joueur
    good[1][pos] = 2; //pierre de l'opposant
    // ligne 3
    pos = 0;
    good[2][pos++] = -1 ; //bord
    for (int i = 0; i < base_taille_seq; i++)
        good[2][pos++] = 1; // Les pierres du joueur
    good[2][pos] = 0; // L'espace vide après la séquence de pierres

    int count = 0;
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[idx(x, y)] != player) continue;
            for (int d = 0; d < nb_dir; d++) {
                int *sequence = seq(board, x, y, directions[d][1], directions[d][0], player, taille_seq);
                // Vérifie si la séquence correspond à une des séquences de "good" autorisées
                for (int i = 0; i < nb_good; i++) {
                    bool match = true;
                    for (int j = 0; j < taille_seq; j++) {
                        if (sequence[j] != good[i][j]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        count++;
                        break;
                    }
                }
                free(sequence);
            }
        }
    }
    return count;
}

// Fonction pour vérifier l'alignement gagnant.
bool isAlignment(const char *board, int x, int y, char current_player) {
    int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}}; // Horizontal, vertical, diagonal down, diagonal up
    
    for (int d = 0; d < 4; ++d) {
        int dx = directions[d][0], dy = directions[d][1];
        int count = 1; // Compter le pion actuel
        
        // Vérifier dans la direction (dx, dy)
        int i = x + dx, j = y + dy;
        while (is_valid_position(i, j) && board[idx(i, j)] == current_player) {
            count += 1;
            i += dx;
            j += dy;
        }
        
        // Vérifier dans la direction opposée (-dx, -dy)
        i = x - dx;
        j = y - dy;
        while (is_valid_position(i, j) && board[idx(i, j)] == current_player) {
            count += 1;
            i -= dx;
            j -= dy;
        }
        
        // Vérifier si le compteur atteint au moins 5
        if (count >= 5) {
            // printf("%c wins\n", current_player);
            return true;
        }
    }
    
    return false;
}

// Fonction pour vérifier si le jeu est terminé.
bool game_over(const char *board) {
    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            char current_player = board[idx(x, y)];
            if (current_player != '0' && isAlignment(board, x, y, current_player)) {
                printf("Un alignement gagnant a été trouvé\n");
                return true; // Un alignement gagnant a été trouvé
            }
        }
    }

    // Vérifiez si le plateau est entièrement rempli pour le match nul
    for (int i = 0; i < SIZE * SIZE; ++i) {
        if (board[i] == '0')
            return false;
    }
    printf("Match null\n");
    return true;
}

// Crée une copie profonde du plateau et applique un mouvement sur cette copie
char* apply_move(const char *original_board, int x, int y, char player) {
    char *new_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour la nouvelle copie
    if (new_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_board, original_board); // Copie l'original dans la nouvelle copie

    int index = idx(x, y);
    new_board[index] = player;

    return new_board; // Retourne la nouvelle copie avec le mouvement appliqué
}

int _evaluate_player(const char *board, char player) {
    int score = 0;
    // Ajoutez des points pour chaque séquence favorable à player
    score += count_sequences(board, player, 2) * 10;
    score += count_sequences(board, player, 3) * 50;
    score += count_sequences(board, player, 4) * 100;

    return score;
}
int _evaluate_opponent(const char *board, char opponent) {
    int score = 0;
    // Ajoutez des points pour chaque séquence favorable à player
    score += count_sequences(board, opponent, 2) * 100;
    score += count_sequences(board, opponent, 3) * 500;
    score += count_sequences(board, opponent, 4) * 1000;

    return score;
}

EvalResult evaluate(const char *board, char player) {
    EvalResult result;
    int scorePlayer = _evaluate_player(board, player);

    char opponent = (player == 'W') ? 'B' : 'W';
    int scoreOpponent = _evaluate_opponent(board, opponent);

    result.scoreDiff = scorePlayer - scoreOpponent;
    result.playerScore = scorePlayer;
    result.opponentScore = scoreOpponent;

    return result;
}


EvalResult minmax(char *board, int depth, int alpha, int beta, bool maximizingPlayer, char current_player, int currentMoveX, int currentMoveY) {
    if (depth == 0 || game_over(board)) {
        EvalResult result = evaluate(board, current_player);
        char opponent = (current_player == 'W') ? 'B' : 'W';
        printf("%c joue (%d,%d), Score: %d, Adversaire %c, Score: %d\n", current_player, currentMoveX, currentMoveY , result.playerScore, opponent, result.opponentScore);
        return result;
    }

    char opponent = (current_player == 'W') ? 'B' : 'W';
    EvalResult bestResult;

    if (maximizingPlayer) {
        int maxEval = MIN_EVAL;
        int move_count;
        bestResult.playerScore = MIN_EVAL; // Le pire score pour le joueur
        bestResult.opponentScore = MIN_EVAL; // Optionnellement, le pire score pour l'adversaire,
        
        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = generate_possible_moves(board, &move_count, current_player, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            // printf("%c(%d,%d)\n",current_player,moves[i].col, moves[i].row);
            char *child_board = apply_move(board, moves[i].col, moves[i].row, current_player);
            // EvalResult result;
            
            EvalResult result = minmax(child_board, depth - 1, alpha, beta, !maximizingPlayer, opponent, moves[i].col, moves[i].row);
            
            if(result.scoreDiff > maxEval) {
                maxEval = result.scoreDiff;
                bestResult.coup = moves[i];
                bestResult = result;
            }
            // maxEval = (eval > maxEval) ? eval : maxEval;
            
            alpha = (result.scoreDiff > alpha) ? result.scoreDiff : alpha;
            // if (beta <= alpha) {
            //     free(child_board);
            //     break;
            // }
            free(child_board);
        }
        free(moves);
        printf("\tBest Max %c(%d,%d), Opponent: %c, Eval Score: %d, Player Score: %d, Opponent Score: %d\n",
        current_player, bestResult.coup.col, bestResult.coup.row, opponent, bestResult.scoreDiff, bestResult.playerScore, bestResult.opponentScore);
        return bestResult; // ou convertissez en valeur appropriée si nécessaire

    } else {
        int minEval = MAX_EVAL;
        int move_count;
        bestResult.playerScore = MAX_EVAL; // Le meilleur score pour le joueur, à minimiser
        bestResult.opponentScore = MAX_EVAL; // Optionnellement, le meilleur score pour l'adversaire
        
        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = generate_possible_moves(board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            // printf("%c(%d,%d)\n",opponent,moves[i].col, moves[i].row);
            char *child_board = apply_move(board, moves[i].col, moves[i].row, opponent);
            // EvalResult result;
            EvalResult result = minmax(child_board, depth - 1, alpha, beta, !maximizingPlayer, current_player, moves[i].col, moves[i].row);
            if (result.scoreDiff < minEval) {
                minEval = result.scoreDiff;
                result.coup = moves[i];
                bestResult = result;
            }
            // minEval = (eval < minEval) ? eval : minEval;
            beta = (result.scoreDiff < beta) ? result.scoreDiff : beta;
            
            // if (beta <= alpha) {
            //     free(child_board);
            //     break;
            // }
            free(child_board);
        }
        free(moves);
        printf("\tBest Min %c(%d,%d), Opponent: %c, Eval Score: %d, Player Score: %d, Opponent Score: %d\n",
        current_player, bestResult.coup.col, bestResult.coup.row, opponent, bestResult.scoreDiff, bestResult.playerScore, bestResult.opponentScore);
        return bestResult; // ou convertissez en valeur appropriée si nécessaire

    }
    // return bestResult;
}

Move play_IA(char *board, char current_player, int depth) {
    int best_score = MIN_EVAL; // Utilisez MIN_EVAL qui est INT_MIN
    Move best_move = {-1, -1}; // Initialisez best_move à une valeur non valide
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = generate_possible_moves(board, &move_count, current_player, topLeftX,topLeftY,bottomRightX,bottomRightY);

    for (int i = 0; i < move_count; i++) {
        char *child_board = apply_move(board, moves[i].col, moves[i].row, current_player);
        EvalResult result;
        printf("\n ***** Coup IA : (%d, %d) *****\n", moves[i].col, moves[i].row);
        result = minmax(child_board, depth, MIN_EVAL, MAX_EVAL, true, current_player, moves[i].col, moves[i].row);
        printf("\n---> Coup: (%d, %d), Score : %d - Score IA: %d, Score Adversaire: %d", moves[i].col, moves[i].row, result.scoreDiff, result.playerScore,result.opponentScore);
        if (result.scoreDiff > best_score) {
            best_score = result.scoreDiff;
            best_move = moves[i];
            printf(" +");
        }
        printf("\n");
        free(child_board); // N'oubliez pas de libérer la mémoire allouée
    }

    if (best_move.col != -1 && best_move.row != -1) {
        printf("\n*** best = (%d, %d) with score = %d\n", best_move.col, best_move.row, best_score);
    } else {
        printf("Aucun mouvement possible trouvé\n");
    }

    free(moves); // Libérez la liste des mouvements possibles après utilisation
    return best_move;
}

void essais(char *board, char current_player) {
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    if (topLeftX < SIZE && topLeftY < SIZE) { // Vérifie que nous avons trouvé un élément non vide
        int move_count;
        Move *moves = generate_possible_moves(board, &move_count, current_player, topLeftX,topLeftY,bottomRightX,bottomRightY);
        
        printf("Analyse sur (%d,%d)x(%d, %d) : %d coups\n", topLeftX, topLeftY, bottomRightX, bottomRightY, move_count);
        for (int i = 0; i < move_count; i++) {
             printf("(%d, %d) ", moves[i].col, moves[i].row);
        }
        printf("\n");
    } else {
        printf("Aucun élément non vide trouvé sur le plateau.\n");
    }
}