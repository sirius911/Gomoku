/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clorin <clorin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 18:51:51 by clorin            #+#    #+#             */
/*   Updated: 2024/03/24 15:45:35 by clorin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "game.h"

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

char *del_captured(char *board, Move *captured) {
    if (!board || !captured) return board;
    int index1 = idx(captured[0].col, captured[0].row);
    int index2 = idx(captured[1].col, captured[1].row);
    board[index1] = '0'; 
    board[index2] = '0';
    return board;
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

int count_seq_4_trous(const char *board, char player){
    int count = 0;
    const int nb_dir = 8;
    int taille_seq = 6;
    int directions[8][2] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
        {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
    };
    int good[6] = {0,1,1,0,1,0};
    for (int y = 0; y < SIZE; ++y) {
        for (int x = 0; x < SIZE; ++x) {
            if (board[idx(x, y)] != player) continue;
            for (int d = 0; d < nb_dir; d++) {
                int *sequence = seq(board, x, y, directions[d][1], directions[d][0], player, taille_seq);
                // Vérifie si la séquence correspond à une des séquences de "good" autorisées
                // for (int i = 0; i < nb_good; i++) {
                    bool match = true;
                    for (int j = 0; j < taille_seq; j++) {
                        if (sequence[j] != good[j]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        count++;
                        break;
                    }
                // }
                free(sequence);
            }
        }
    }
    return count;
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
