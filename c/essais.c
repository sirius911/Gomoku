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
    bool coup_gagnant;
} EvalResult;

typedef struct GameState {
    char *board;
    int  captures[2]; // captures[0] pour le joueur 'B', captures[1] pour le joueur 'W'
    char currentPlayer; // 'B' pour le joueur noir, 'W' pour le joueur blanc
} GameState;


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

char adversaire(const char player){
    return (player == 'W') ? 'B' : 'W';
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
    int three_free[6][6] = {
        {0,1,1,1,0,0},
        {0,1,1,1,0,1},
        {0,1,1,1,0,2},
        {0,1,0,1,1,0},
        {0,1,1,0,1,0},
        {0,1,1,1,0,-1}
    };

    int *sequence = seq(board, x, y, dx, dy, player, 6);
    if (!sequence)
        return false;
    // print_sequence(sequence,6);
    // Vérifie si la séquence correspond à une des séquences de "trois" autorisées
    for (int i = 0; i < 6; i++) {
        bool match = true;
        for (int j = 0; j < 6; j++) {
            if (sequence[j] != three_free[i][j]) {
                match = false;
                break;
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
    if (!is_valid_position(x1, y1) || 
        !is_valid_position(x2, y2) ||
        !is_valid_position(x3, y3))
        return NULL;

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
            print("Capture detected at (%d,%d) and (%d,%d) by %c\n", 
            captured[0].col, captured[0].row, captured[1].col, captured[1].row, board[idx(x, y)]);
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

Move* proximate_moves(char *board, int *move_count, const char current_player, int x1, int y1, int x2, int y2){
    /*  XXX
        XBX
        XXX
    */
    int count = 0;
    char *copie_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour une copie
    if (copie_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(copie_board, board); // Copie l'original dans la nouvelle copie
    int directions[8][2] = {
        {-1, -1}, {0, -1}, {+1, -1},
        {-1, 0},           {+1, 0},
        {-1, +1}, {0, +1}, {+1, +1}
    };
    // on coche toutes les cases autour d'un joueur si vide
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            int index = idx(col, row);
            if (copie_board[index] != '0' && copie_board[index] != 'X'){
                for (int i = 0; i < 8; i++){
                    int dx = directions[i][0];
                    int dy = directions[i][1];
                    if (is_valid_position(col + dx, row + dy)) {
                        int index2 = idx(col + dx, row + dy);
                        if (copie_board[index2] == '0' && !check_double_three(board, col+dx, row+dy, current_player)){
                            copie_board[index2] = 'X' ;
                            count++;
                        }
                    }
                }
            }
        }
    }
    if (count == 0) {
        print("board vide");
        Move *move = (Move*) malloc(sizeof(Move));
        move->col=9;
        move->row=9;
        *move_count = 1;
        return move;
    }
    // on recupere toutes les cases X
    Move *moves = (Move*) malloc(MAX_MOVES * sizeof(Move));
    if (moves == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        return NULL;
    }
    count = 0;
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            int index = idx(col, row);
            if (copie_board[index] == 'X'){
                moves[count].col = col;
                moves[count].row = row;
                count++;
            }
        }
    }
    *move_count = count;
    free(copie_board);
    return moves;
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
        if (count >= 5)
            return true;
    }
    
    return false;
}

bool game_over(const GameState *gameState, char *winner) {
    // captures
    if (gameState->captures[0] >= 10){
        print("B gagne par capture\n");
        *winner = 'B';
        return true;
    }
    if (gameState->captures[1] >= 10){
        print("W gagne par capture\n");
        *winner = 'W';
        return true;
    }
    // Alignement   
    for (int x = 0; x < SIZE; ++x) {
        for (int y = 0; y < SIZE; ++y) {
            char current_player = gameState->board[idx(x, y)];
            if (current_player != '0' && isAlignment(gameState->board, x, y, current_player)) {
                *winner = current_player;
                print("Un alignement gagnant a été trouvé pour %c\n", *winner);
                return true; // Un alignement gagnant a été trouvé
            }
        }
    }

    // Vérifiez si le plateau est entièrement rempli pour le match nul
    for (int i = 0; i < SIZE * SIZE; ++i) {
        if (gameState->board[i] == '0')
            return false;
    }
    print("Match null le plateau est plein!\n");
    *winner = 'N';
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

GameState *apply_move(const GameState *original_gameState, int x, int y) {
    int capture0 = original_gameState->captures[0];
    int capture1 = original_gameState->captures[1];

    GameState *new_gameState = malloc(sizeof (GameState)); // Alloue de la mémoire pour la nouvelle copie
    if (new_gameState == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    char *new_board = malloc(SIZE * SIZE + 1); // Alloue de la mémoire pour la nouvelle copie
    if (new_board == NULL) {
        fprintf(stderr, "Allocation de mémoire échouée\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_board, original_gameState->board); // Copie l'original dans la nouvelle copie
    new_gameState->board = new_board;
    new_gameState->captures[0] = capture0;
    new_gameState->captures[1] = capture1;
    int index = idx(x, y);
    new_gameState->board[index] = original_gameState->currentPlayer;
    new_gameState->currentPlayer = (original_gameState->currentPlayer == 'W') ? 'B' : 'W';
    Move *captured = check_capture(new_gameState->board, x, y);
    if (captured){
        new_gameState->board = del_captured(new_gameState->board, captured);
        if (original_gameState->currentPlayer == 'B')
            new_gameState->captures[0] += 2;
        else
            new_gameState->captures[1] += 2;
    }
    return new_gameState; // Retourne la nouvelle copie avec le mouvement appliqué
}

int _evaluate_player(const GameState *gameState, char player) {
    int score = 0;
    int num_player =( player == 'B')? 0:1;
    if (count_sequences(gameState->board, player, 4) >= 1 || 
        count_sequences(gameState->board, player, 5) > 0 ||
        gameState->captures[num_player] >= 10)
        return MAX_EVAL;

    score += count_sequences(gameState->board, player, 2) * 10;
    score += count_sequences(gameState->board, player, 3) * 50;
    score += count_sequences(gameState->board, player, 4) *10000;

    score += gameState->captures[num_player] * 100 ;
    return score;
}
int _evaluate_opponent(const GameState *gameState, char opponent) {
    int score = 0;
    int num_player =( opponent == 'B')? 0:1;
    if (count_sequences(gameState->board, opponent, 5) > 0 ||
        gameState->captures[num_player] >= 10)
        return MAX_EVAL;

    score += count_sequences(gameState->board, opponent, 2) * 20;
    score += count_sequences(gameState->board, opponent, 3) * 100;
    score += count_sequences(gameState->board, opponent, 4) * 100000;

    score += gameState->captures[num_player] * 50 ;
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

void    free_gameState(GameState *game) {
    if (game) {
        if (game->board)
            free(game->board);
        free(game);
    }
}

EvalResult minmax(GameState *gameState, int depth, int alpha, int beta, bool maximizingPlayer, int currentMoveX, int currentMoveY) {
    char winner = '0';
    GameState *child_gameState = apply_move(gameState, currentMoveX, currentMoveY);
    if (game_over(child_gameState, &winner) || depth == 0) {

        char opponent = (gameState->currentPlayer == 'W') ? 'B' : 'W';

        EvalResult result;
        result.coup_gagnant = false;
        if (winner != '0'){
            // un gagnant ou match null
            // TODO gestion de winner == 'N' -> match Null
            if (maximizingPlayer){
                result.scoreDiff = INT_MAX - depth; // Favoriser les victoires plus rapides
                result.playerScore = INT_MAX - depth;
                result.opponentScore = INT_MIN + depth;
                result.coup_gagnant = true;
            } else {
                result.scoreDiff = INT_MIN + depth; // La pénalité est moindre pour les défaites tardives
                result.playerScore = INT_MIN + depth;
                result.opponentScore = INT_MAX - depth;
            }
        } else {
            // Pas de vainqueur ou profondeur atteinte, évaluer la position
            if (maximizingPlayer) {
                result.playerScore = _evaluate_player(child_gameState, gameState->currentPlayer);
                result.opponentScore = _evaluate_opponent(child_gameState, opponent);
            } else {
                result.playerScore = _evaluate_player(child_gameState, opponent);
                result.opponentScore = _evaluate_opponent(child_gameState, gameState->currentPlayer);
            }
            result.scoreDiff = result.playerScore - result.opponentScore;
        }
        print("\tScore : %c= %d,  %c= %d -> diff= %d\n\t capture B:%d, W:%d\n",
            gameState->currentPlayer , result.playerScore, opponent, result.opponentScore, result.scoreDiff, gameState->captures[0], gameState->captures[1]);

        free_gameState(child_gameState);
        return result;
    }

    char opponent = adversaire(gameState->currentPlayer);
    EvalResult bestResult;

    if (maximizingPlayer) {
        int maxEval = MIN_EVAL;
        int move_count;
        bestResult.playerScore = MIN_EVAL; // Le pire score pour le joueur

        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(child_gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = proximate_moves(child_gameState->board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            print("\t+ Si %c(%d,%d)\n",opponent,moves[i].col, moves[i].row);

            EvalResult result = minmax(child_gameState, depth - 1, alpha, beta, !maximizingPlayer, moves[i].col, moves[i].row);

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
        child_gameState->currentPlayer, bestResult.coup.col, bestResult.coup.row,
        bestResult.scoreDiff,
        child_gameState->currentPlayer, bestResult.playerScore,
        adversaire(child_gameState->currentPlayer), bestResult.opponentScore);
    } else {
        int minEval = MAX_EVAL;
        int move_count;
        bestResult.playerScore = MAX_EVAL; // Le meilleur score pour le joueur, à minimiser
        bestResult.opponentScore = INT_MIN; // Optionnellement, le meilleur score pour l'adversaire
        
        
        int topLeftX, topLeftY, bottomRightX, bottomRightY;
        findBoxElements(child_gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
        Move *moves = proximate_moves(child_gameState->board, &move_count, opponent, topLeftX,topLeftY,bottomRightX,bottomRightY);
        for (int i = 0; i < move_count; i++) {
            print("\t\t-%c(%d,%d)\n",opponent,moves[i].col, moves[i].row);
           EvalResult result = minmax(child_gameState, depth - 1, alpha, beta, !maximizingPlayer, moves[i].col, moves[i].row);
 
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
        print("\tMeilleur coup pour %c(%d,%d), Score: %d, %c=%d, %c=%d\n",
        opponent, bestResult.coup.col, bestResult.coup.row, 
        bestResult.scoreDiff,
        opponent, bestResult.playerScore,
        adversaire(opponent), bestResult.opponentScore);
    }
    return bestResult;
}

Move play_IA(GameState *gameState, int depth, bool debug) {
    DEBUG = debug;
    int best_score = MIN_EVAL; // Utilisez MIN_EVAL qui est INT_MIN
    Move best_move = {-1, -1}; // Initialisez best_move à une valeur non valide
    int move_count;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;
    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    Move *moves = proximate_moves(gameState->board, &move_count, gameState->currentPlayer, topLeftX,topLeftY,bottomRightX,bottomRightY);
    for (int i = 0; i < move_count; i++) {
        EvalResult result;
        print("\n ***** Coup IA : %c(%d, %d) *****\n", gameState->currentPlayer, moves[i].col, moves[i].row);
        result = minmax(gameState, depth, MIN_EVAL, MAX_EVAL, true, moves[i].col, moves[i].row);
        print("\n---> Coup: (%d, %d), Score : %d - Score IA: %d, Score Adversaire: %d %s",
            moves[i].col, moves[i].row, result.scoreDiff, result.playerScore,result.opponentScore, result.coup_gagnant? "Coup gagnant":"");
        print("\n---------------\n");
        if (result.coup_gagnant){
            best_score = result.scoreDiff;
            best_move = moves[i];
            break;
        }
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

void analyse(GameState *gameState, bool debug) {
    DEBUG = debug;
    int topLeftX, topLeftY, bottomRightX, bottomRightY;

    findBoxElements(gameState->board, &topLeftX, &topLeftY, &bottomRightX, &bottomRightY);
    if (topLeftX < SIZE && topLeftY < SIZE) {
        int move_count;
        Move *moves = proximate_moves(gameState->board, &move_count, gameState->currentPlayer, topLeftX, topLeftY, bottomRightX, bottomRightY);
        
        print("Analyse sur (%d,%d)x(%d, %d) : %d coups possibles\n", topLeftX, topLeftY, bottomRightX, bottomRightY, move_count);
        print_sequences_board(gameState->board, "");
        for (int i = 0; i < move_count; i++) {
            print("(%d, %d) ", moves[i].col, moves[i].row);
        }
        print("\n");
        free(moves);
    } else {
        print("Aucun élément non vide trouvé sur le plateau.\n");
    }
    print("Capture pour Black:%d, pour White:%d\n", gameState->captures[0], gameState->captures[1]);
}
