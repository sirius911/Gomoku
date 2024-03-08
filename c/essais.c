#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <stdarg.h>

#define SIZE 19
#define MAX_MOVES (SIZE * SIZE)
// Utiliser INT_MIN et INT_MAX pour représenter -inf et +inf
#define MAX_EVAL INT_MAX
#define MIN_EVAL INT_MIN

bool DEBUG = false;

typedef struct {
    int col;
    int row;
} Move;

typedef struct {
    int scoreDiff;
    int playerScore;
    int opponentScore;
    Move coup;
    int captured_black;
    int captured_white;
} EvalResult;

void print(const char *format, ...){
    if(DEBUG) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
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

// void print_sequence(const int *seq, int lenght) {
//     printf("sequence = {");
//     for (int a = 0; a < lenght; a++){
//         printf("%d, ", seq[a]);
//     }
//     printf("}\n");
// }

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
    board = put(board, player, x, y);
    for (int i = 0; i < 6; i++) {
        if (is_three(board, x, y, directions[i][0], directions[i][1], player)) {
            three_count++;
            if (three_count > 1) {
                board = put(board, '0', x, y);
                return true;
            }
        }
    }
    board = put(board, '0', x, y);
    return false;
}

void free_moves(Move* moves) {
    if (moves)
        free(moves);
}


Move* capture(const char* board, int x, int y, int dx, int dy) {
    // Calcul des positions suivantes selon la direction
    int x1 = x + dx;
    int y1 = y + dy;
    int x2 = x + 2 * dx;
    int y2 = y + 2 * dy;
    int x3 = x + 3 * dx;
    int y3 = y + 3 * dy;

    // Vérifier si les positions sont dans les limites du plateau
    if (x1 < 0 || x1 >= SIZE || y1 < 0 || y1 >= SIZE ||
        x2 < 0 || x2 >= SIZE || y2 < 0 || y2 >= SIZE ||
        x3 < 0 || x3 >= SIZE || y3 < 0 || y3 >= SIZE) {
        return NULL;
    }

    // Accéder aux positions sur le plateau
    char pos1 = board[idx(x1, y1)];
    char pos2 = board[idx(x2, y2)];
    char pos3 = board[idx(x3, y3)];
    char currentPlayer = board[idx(x, y)];

    // Vérifier la séquence de capture: joueur-opposant-opposant-joueur
    if (pos1 != currentPlayer && pos1 != '0' && pos1 == pos2 && pos3 == currentPlayer) {
        Move* captured = malloc(2 * sizeof(Move));
        if (captured == NULL)
            return NULL;
        captured[0].col = x1;
        captured[0].row = y1;
        captured[1].col = x2;
        captured[1].row = y2;
        return captured;
    }
    return NULL;
}

Move *check_capture(const char* board, int x, int y) {
    // Définir les directions à vérifier
    int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};
    Move* captured = NULL;
    for (int i = 0; i < 8; ++i) {
        int dx = directions[i][0];
        int dy = directions[i][1];
        captured = capture(board, x, y, dx, dy);

        if (captured != NULL) {
            print("Capture detected at (%d,%d) and (%d,%d)\n", captured[0].col, captured[0].row, captured[1].col, captured[1].row);
            return captured;
        }
    }
    return NULL;
}


// Fonction pour scanner le plateau et trouver les éléments les plus extrêmes
void findBoxElements(const char *board, int *topLeftX, int *topLeftY, int *bottomRightX, int *bottomRightY) {
    *topLeftX = SIZE; // Initialisé à une valeur maximale pour trouver le minimum
    *topLeftY = SIZE;
    *bottomRightX = -1; // Initialisé à une valeur minimale pour trouver le maximum
    *bottomRightY = -1;
    bool isEmpty = true;

    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[y * SIZE + x] != '0') {
                isEmpty = false;
                if (x < *topLeftX) {
                    *topLeftX = x;
                }
                if (y < *topLeftY) {
                    *topLeftY = y;
                }
                if (x > *bottomRightX) {
                    *bottomRightX = x;
                }
                if (y > *bottomRightY) {
                    *bottomRightY = y;
                }
            }
        }
    }
    if (isEmpty) {
        *topLeftX = 0;
        *topLeftY = 0;
        *bottomRightX = SIZE - 1;
        *bottomRightY = SIZE - 1;
    }
    else {
        *topLeftX = (*topLeftX > 0) ? (*topLeftX - 1) : 0;
        *topLeftY = (*topLeftY > 0) ? (*topLeftY - 1) : 0;
        *bottomRightX = (*bottomRightX < SIZE - 1) ? (*bottomRightX + 1) : SIZE - 1;
        *bottomRightY = (*bottomRightY < SIZE - 1) ? (*bottomRightY + 1) : SIZE - 1;
    }
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
    int directions[8][2] = {
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
bool game_over(const char *board, char *winner) {
    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            char current_player = board[idx(x, y)];
            if (current_player != '0' && isAlignment(board, x, y, current_player)) {
                *winner = current_player;
                print("Un alignement gagnant a été trouvé pour %c\n", *winner);
                return true; // Un alignement gagnant a été trouvé
            }
        }
    }

    // Vérifiez si le plateau est entièrement rempli pour le match nul
    for (int i = 0; i < SIZE * SIZE; ++i) {
        if (board[i] == '0')
            return false;
    }
    print("Match null\n");
    return true;
}

char *del_captured(char *board, Move *captured) {
    if (!board || !captured) return board;
    int index1 = idx(captured[0].col, captured[0].row);
    int index2 = idx(captured[1].col, captured[1].row);
    board[index1] = '0'; 
    board[index2] = '0';
    return board;
}

// Crée une copie profonde du plateau et applique un mouvement sur cette copie
// et eventuellemnt les captures
char* apply_move(const char *original_board, int x, int y, char player) {
    char *new_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour la nouvelle copie
    if (new_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_board, original_board); // Copie l'original dans la nouvelle copie

    int index = idx(x, y);
    new_board[index] = player;
    Move *captured = check_capture(new_board, x, y);
    if (captured)
        new_board = del_captured(new_board, captured);
    return new_board; // Retourne la nouvelle copie avec le mouvement appliqué
}

int _evaluate_player(const char *board, char player) {
    int score = 0;
    if (count_sequences(board, player, 4) >= 1 || count_sequences(board, player, 5) > 0)
        return MAX_EVAL;

    score += count_sequences(board, player, 2) * 10;
    score += count_sequences(board, player, 3) * 50;
    score += count_sequences(board, player, 4) *10000;
    return score;
}
int _evaluate_opponent(const char *board, char opponent) {
    int score = 0;
    if (count_sequences(board, opponent, 5) > 0)
        return MAX_EVAL;

    score += count_sequences(board, opponent, 2) * 20;
    score += count_sequences(board, opponent, 3) * 100;
    score += count_sequences(board, opponent, 4) * 100000;
    return score;
}

void print_sequences_board(char *board, const char *entete) {
    int b2,b3,b4;
    int w2,w3,w4;
    bool b5, w5;
    b2 = count_sequences(board, 'B', 2);
    b3 = count_sequences(board, 'B', 3);
    b4 = count_sequences(board, 'B', 4);
    b5 = (count_sequences(board, 'B', 5) > 0);
    w2 = count_sequences(board, 'W', 2);
    w3 = count_sequences(board, 'W', 3);
    w4 = count_sequences(board, 'W', 4);
    w5 = (count_sequences(board, 'W', 5) > 0);
    print("%sBlack [2]:%d - [3]:%d = [4]:%d",entete,b2,b3,b4);
    print(" %s", b5 ? "Winner\n":"\n");
    print("%sWhite [2]:%d - [3]:%d = [4]:%d\n",entete,w2,w3,w4);
    print(" %s", w5 ? "Winner\n":"\n");
}


EvalResult minmax(char *board, int depth, int alpha, int beta, bool maximizingPlayer, char current_player, int currentMoveX, int currentMoveY) {
    char winner = '0';
    char *child_board = apply_move(board, currentMoveX, currentMoveY, current_player);
    if (game_over(child_board, &winner) || depth == 0) {

        char opponent = (current_player == 'W') ? 'B' : 'W';

        EvalResult result;
        if (winner != '0'){
            if (maximizingPlayer){
                result.scoreDiff = INT_MAX - depth; // Favoriser les victoires plus rapides
                result.playerScore = INT_MAX - depth;
                result.opponentScore = INT_MIN + depth;
            } else {
                result.scoreDiff = INT_MIN + depth; // La pénalité est moindre pour les défaites tardives
                result.playerScore = INT_MIN + depth;
                result.opponentScore = INT_MAX - depth;
            }
        } else {
            // Pas de vainqueur ou profondeur atteinte, évaluer la position
            if (maximizingPlayer) {
                result.playerScore = _evaluate_player(child_board, current_player);
                result.opponentScore = _evaluate_opponent(child_board, opponent);
            } else {
                result.playerScore = _evaluate_player(child_board, opponent);
                result.opponentScore = _evaluate_opponent(child_board, current_player);
            }
            result.scoreDiff = result.playerScore - result.opponentScore;
        }
        print("\tScore : %c= %d,  %c= %d -> diff= %d",
            current_player , result.playerScore, opponent, result.opponentScore, result.scoreDiff);

        free(child_board);
        return result;
    }

    char opponent = (current_player == 'W') ? 'B' : 'W';
    EvalResult bestResult;

    if (maximizingPlayer) {
        int maxEval = MIN_EVAL;
        int move_count;
        bestResult.playerScore = MIN_EVAL; // Le pire score pour le joueur

        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(child_board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = generate_possible_moves(child_board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            print("\t+ Si %c(%d,%d)\n",opponent,moves[i].col, moves[i].row);

            EvalResult result = minmax(child_board, depth - 1, alpha, beta, !maximizingPlayer, opponent, moves[i].col, moves[i].row);

            if(result.scoreDiff > maxEval) {
                maxEval = result.scoreDiff;
                result.coup = moves[i];
                bestResult = result;
                print(" best move!\n");
            }
            else
                print("\n");
            
            alpha = (result.scoreDiff > alpha) ? result.scoreDiff : alpha;
            if (beta <= alpha) {
                print("- break \n");
                break;
            }
        }
        free(moves);
        print("\t\tMeilleur coup pour %c(%d,%d), score=%d  %c=%d, %c=%d\n",
        current_player, bestResult.coup.col, bestResult.coup.row, bestResult.scoreDiff, current_player, bestResult.playerScore, opponent, bestResult.opponentScore);
    } else {
        int minEval = MAX_EVAL;
        int move_count;
        bestResult.playerScore = MAX_EVAL; // Le meilleur score pour le joueur, à minimiser
        bestResult.opponentScore = INT_MIN; // Optionnellement, le meilleur score pour l'adversaire
        
        
        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(child_board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = generate_possible_moves(child_board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            print("\t\t-%c(%d,%d)\n",opponent,moves[i].col, moves[i].row);
           EvalResult result = minmax(child_board, depth - 1, alpha, beta, !maximizingPlayer, opponent, moves[i].col, moves[i].row);
 
            if (result.scoreDiff < minEval) {
                minEval = result.scoreDiff;
                result.coup = moves[i];
                bestResult = result;
                print(" *worst move !\n");
            }
            else
                print("\n");
           beta = (result.scoreDiff < beta) ? result.scoreDiff : beta;
            
            if (beta <= alpha) {
                print("- break -\n");
                break;
            }
        }
        free(moves);
        print("\tMeilleur coup pour adversaire: %c(%d,%d), Score: %d\n",
        opponent, bestResult.coup.col, bestResult.coup.row, bestResult.scoreDiff);
    }
    return bestResult;
}

Move play_IA(char *board, char current_player, int depth, bool debug) {
    DEBUG = debug;
    int best_score = MIN_EVAL; // Utilisez MIN_EVAL qui est INT_MIN
    Move best_move = {-1, -1}; // Initialisez best_move à une valeur non valide
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = generate_possible_moves(board, &move_count, current_player, topLeftX,topLeftY,bottomRightX,bottomRightY);

    for (int i = 0; i < move_count; i++) {
        EvalResult result;
        print("\n ***** Coup IA : %c(%d, %d) *****\n", current_player, moves[i].col, moves[i].row);
        result = minmax(board, depth, MIN_EVAL, MAX_EVAL, true, current_player, moves[i].col, moves[i].row);
        print("\n---> Coup: (%d, %d), Score : %d - Score IA: %d, Score Adversaire: %d",
            moves[i].col, moves[i].row, result.scoreDiff, result.playerScore,result.opponentScore);
        print("\n---------------\n");
        if (result.scoreDiff > best_score) {
            best_score = result.scoreDiff;
            best_move = moves[i];
        }
        print("\n");
    }

    if (best_move.col != -1 && best_move.row != -1) {
        print("\n*** best = (%d, %d) with score = %d\n", best_move.col, best_move.row, best_score);
    } else {
        print("Aucun mouvement possible trouvé\n");
    }

    free(moves); // Libérez la liste des mouvements possibles après utilisation
    return best_move;
}

void analyse(char *board, char current_player, bool debug) {
    DEBUG = debug;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;

    findBoxElements(board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    if (topLeftX < SIZE && topLeftY < SIZE) { // Vérifie que nous avons trouvé un élément non vide
        int move_count;
        Move *moves = generate_possible_moves(board, &move_count, current_player, topLeftX,topLeftY,bottomRightX,bottomRightY);
        
        print("Analyse sur (%d,%d)x(%d, %d) : %d coups possibles\n", topLeftX, topLeftY, bottomRightX, bottomRightY, move_count);
        for (int i = 0; i < move_count; i++) {
             print("(%d, %d) ", moves[i].col, moves[i].row);
        }
        print("\n");
    } else {
        print("Aucun élément non vide trouvé sur le plateau.\n");
    }
}