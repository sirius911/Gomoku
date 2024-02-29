#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 19
#define MAX_MOVES (SIZE * SIZE)

typedef struct {
    int col;
    int row;
} Move;

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


Move* generate_possible_moves(char *board, int *move_count, const char current_player) {
    /*
        return a array of struc Move with possible moves
    */
    Move *moves = (Move*) malloc(MAX_MOVES * sizeof(Move));
    int count = 0;

    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
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

// /*
// int good[x][4] = {
//         {0,1,1,0}, // compté 2x <->
//         {0,1,1,2},
//     };
// */


// int count_sequences2(const char *board, char player) {
//     // compte les seq de deux stones
//     const int nb_dir = 6;
//     const int taille_seq = 4;
//     const int nb_good = 3;
//     int directions[nb_dir][2] = {
//         {1, 0}, {0, 1}, {-1, 0}, {0, -1},
//         {1, 1}, {-1, -1}
//     };
//     int good[nb_good][taille_seq] = {
//         {0,1,1,0}, // compté 2x <->
//         {0,1,1,2},
//         {-1,1,1,0}
//     };
//     int count = 0;
//     for (int y = 0; y < SIZE; ++y) {
//         for (int x = 0; x < SIZE; ++x) {
//             if (board[idx(x, y)] != player)
//                 continue;
//             for (int d = 0; d < nb_dir; d++) {
//                 int *sequence = seq(board, x, y, directions[d][1], directions[d][0], player, taille_seq);
//                 //print_sequence(sequence, 4);
                
//                 // Vérifie si la séquence correspond à une des séquences de "good" autorisées
//                 for (int i = 0; i < nb_good; i++) {
//                     bool match = true;
//                     for (int j = 0; j < taille_seq; j++) {
//                         if (sequence[j] != good[i][j]) {
//                             match = false;
//                             break;
//                         }
//                     }
//                     if (match) {
//                         count ++;
//                         break;
//                     }
//                 }
//                 free(sequence);
//             }
//         }
//     }
//     return count;
// }


// int count_sequences3(const char *board, char player) {
//     // compte les seq de trois stones
//     const int nb_dir = 6;
//     const int taille_seq = 5;
//     const int nb_good = 3;
//     int directions[nb_dir][2] = {
//         {1, 0}, {0, 1}, {-1, 0}, {0, -1},
//         {1, 1}, {-1, -1}
//     };
//     int good[nb_good][taille_seq] = {
//         {0,1,1,1,0}, // compté 2x <->
//         {2,1,1,1,0},
//         {-1,1,1,1,0}
//     };
//     int count = 0;
//     for (int y = 0; y < SIZE; ++y) {
//         for (int x = 0; x < SIZE; ++x) {
//             if (board[idx(x, y)] != player)
//                 continue;
//             for (int d = 0; d < nb_dir; d++) {
//                 int *sequence = seq(board, x, y, directions[d][1], directions[d][0], player, taille_seq);
//                 // print_sequence(sequence, taille_seq);
                
//                 // Vérifie si la séquence correspond à une des séquences de "good" autorisées
//                 for (int i = 0; i < nb_good; i++) {
//                     bool match = true;
//                     for (int j = 0; j < taille_seq; j++) {
//                         if (sequence[j] != good[i][j]) {
//                             match = false;
//                             break;
//                         }
//                     }
//                     if (match) {
//                         count ++;
//                         break;
//                     }
//                 }
//                 free(sequence);
//             }
//         }
//     }
//     return count;
// }

// int count_sequences4(const char *board, char player) {
//     // compte les seq de 4 stones
//     const int nb_dir = 6;
//     const int taille_seq = 6;
//     const int nb_good = 3;

//     int directions[nb_dir][2] = {
//         {1, 0}, {0, 1}, {-1, 0}, {0, -1},
//         {1, 1}, {-1, -1}
//     };
//     int good[nb_good][taille_seq] = {
//         {0,1,1,1,1,0}, // compté 2x <->
//         {2,1,1,1,1,0},
//         {-1,1,1,1,1,0}
//     };
//     int count = 0;
//     for (int y = 0; y < SIZE; ++y) {
//         for (int x = 0; x < SIZE; ++x) {
//             if (board[idx(x, y)] != player)
//                 continue;
//             for (int d = 0; d < nb_dir; d++) {
//                 int *sequence = seq(board, x, y, directions[d][1], directions[d][0], player, taille_seq);
//                 // print_sequence(sequence, taille_seq);
                
//                 // Vérifie si la séquence correspond à une des séquences de "good" autorisées
//                 for (int i = 0; i < nb_good; i++) {
//                     bool match = true;
//                     for (int j = 0; j < taille_seq; j++) {
//                         if (sequence[j] != good[i][j]) {
//                             match = false;
//                             break;
//                         }
//                     }
//                     if (match) {
//                         count ++;
//                         break;
//                     }
//                 }
//                 free(sequence);
//             }
//         }
//     }
//     return count;
// }
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
            printf("%c wins\n", current_player);
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
    // bool full = true;
    for (int i = 0; i < SIZE * SIZE; ++i) {
        if (board[i] == '0')
            return false;
    }
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


int essais(char *board, const char current_player, int sequence_length) {
    // int move_count;

    // Move *possible_moves = generate_possible_moves(board, &move_count, current_player);

    // for (int i = 0; i < move_count; ++i) {
    //     printf("(%d, %d)\n", possible_moves[i].col, possible_moves[i].row);
    // }
    
    // printf("Mouvements possibles pour %c: %d\n", current_player, move_count);
    // Libérer la mémoire allouée pour possible_moves
    // free(possible_moves);

    int  count_seq = 0;
    // count_seq = count_sequences(board, current_player, sequence_length);
    // game_over(board);
    return count_seq;
}