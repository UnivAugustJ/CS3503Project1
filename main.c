#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h> // For abs()

typedef uint64_t Bitboard;
typedef enum { PLAYER_RED, PLAYER_BLACK } Player;

// Phase 1 Section: Bit Manipulation Operations

Bitboard SetBit(Bitboard value, int position) {
    if (position < 0 || position >= 64) return value;
    return value | (1ULL << position);
}

Bitboard ClearBit(Bitboard value, int position) {
    if (position < 0 || position >= 64) return value;
    return value & ~(1ULL << position);
}

int GetBit(Bitboard value, int position) {
    if (position < 0 || position >= 64) return 0;
    return (value >> position) & 1;
}

int CountBits(Bitboard value) {
    int count = 0;
    while (value > 0) {
        value &= (value - 1);
        count++;
    }
    return count;
}


// Phase 2 Section: Checkers Game Implementation

typedef struct {
    Bitboard red_pieces;
    Bitboard black_pieces;
    Bitboard red_kings;
    Bitboard black_kings;
    Player current_turn;
} CheckersGameState;

int SquareToPos(const char *square) {
    if (!square || toupper(square[0]) < 'A' || toupper(square[0]) > 'H' || square[1] < '1' || square[1] > '8') {
        return -1;
    }
    int col = toupper(square[0]) - 'A';
    int row = square[1] - '1';
    return row * 8 + col;
}

CheckersGameState StartGame() {
    CheckersGameState game;
    game.red_pieces    = 0x00000055AA55;
    game.black_pieces  = 0xAA55AA0000000000;
    game.red_kings     = 0;
    game.black_kings   = 0;
    game.current_turn  = PLAYER_RED;
    return game;
}

void DisplayBoard(const CheckersGameState *game) {
    printf("\n   A B C D E F G H\n");
    printf(" +-----------------+\n");
    for (int row = 7; row >= 0; row--) {
        printf("%d| ", row + 1);
        for (int col = 0; col < 8; col++) {
            int pos = row * 8 + col;
            char piece = '.';
            if (GetBit(game->red_kings, pos)) piece = 'K';
            else if (GetBit(game->black_kings, pos)) piece = 'Q';
            else if (GetBit(game->red_pieces, pos)) piece = 'r';
            else if (GetBit(game->black_pieces, pos)) piece = 'b';
            printf("%c ", piece);
        }
        printf("|%d\n", row + 1);
    }
    printf(" +-----------------+\n");
    printf("   A B C D E F G H\n");
    printf("Turn: %s\n", (game->current_turn == PLAYER_RED) ? "Red (r/K)" : "Black (b/Q)");
}

bool ExecuteMove(CheckersGameState *game, int from_pos, int to_pos) {
    Bitboard *player_pieces = (game->current_turn == PLAYER_RED) ? &game->red_pieces : &game->black_pieces;
    Bitboard *player_kings = (game->current_turn == PLAYER_RED) ? &game->red_kings : &game->black_kings;
    Bitboard *opponent_pieces = (game->current_turn == PLAYER_RED) ? &game->black_pieces : &game->red_pieces;
    Bitboard *opponent_kings = (game->current_turn == PLAYER_RED) ? &game->black_kings : &game->red_kings;
    Bitboard all_pieces = game->red_pieces | game->black_pieces;

    if (!GetBit(*player_pieces, from_pos)) {
        printf("Error: No piece at the starting square.\n");
        return false;
    }
    if (GetBit(all_pieces, to_pos)) {
        printf("Error: The target square is occupied.\n");
        return false;
    }

    int from_col = from_pos % 8;
    int to_col = to_pos % 8;
    int pos_diff = to_pos - from_pos;
    bool is_king = GetBit(*player_kings, from_pos);

    // Checks for captures
    if (abs(from_col - to_col) == 2 && (abs(pos_diff) == 14 || abs(pos_diff) == 18)) {
        int jumped_pos = (from_pos + to_pos) / 2;
        if (!GetBit(*opponent_pieces, jumped_pos)) {
            printf("Error: A jump must be over an opponent's piece.\n");
            return false;
        }
        if (!is_king) {
            if (game->current_turn == PLAYER_RED && pos_diff < 0)
                return false;
            if (game->current_turn == PLAYER_BLACK && pos_diff > 0)
                return false;
        }
        *player_pieces = ClearBit(*player_pieces, from_pos);
        *player_pieces = SetBit(*player_pieces, to_pos);
        if (is_king) {
            *player_kings = ClearBit(*player_kings, from_pos);
            *player_kings = SetBit(*player_kings, to_pos);
        }
        *opponent_pieces = ClearBit(*opponent_pieces, jumped_pos);
        *opponent_kings = ClearBit(*opponent_kings, jumped_pos);
        printf("Another one bites the dust...\n");
        return true;
    }

    // Checks for invalid moves
    if (abs(from_col - to_col) == 1 && (abs(pos_diff) == 7 || abs(pos_diff) == 9)) {
        if (!is_king) {
            if (game->current_turn == PLAYER_RED && pos_diff < 0) {
                printf("Error: Red regular pieces must move forward.\n");
                return false;
            }
            if (game->current_turn == PLAYER_BLACK && pos_diff > 0) {
                printf("Error: Black regular pieces must move forward.\n");
                return false;
            }
        }
        *player_pieces = ClearBit(*player_pieces, from_pos);
        *player_pieces = SetBit(*player_pieces, to_pos);
        if (is_king) {
            *player_kings = ClearBit(*player_kings, from_pos);
            *player_kings = SetBit(*player_kings, to_pos);
        }
        return true;
    }

    printf("Error: Invalid move. Must be a single diagonal step or a valid capture.\n");
    return false;
}

void CheckForPromotion(CheckersGameState *game) {
    Bitboard red_promotion_row = 0xFF00000000000000;
    Bitboard black_promotion_row = 0x00000000000000FF;

    Bitboard red_promotions = game->red_pieces & red_promotion_row & ~game->red_kings;
    if (red_promotions > 0) {
        game->red_kings |= red_promotions;
        printf("Red piece promoted to King!\n");
    }

    Bitboard black_promotions = game->black_pieces & black_promotion_row & ~game->black_kings;
    if (black_promotions > 0) {
        game->black_kings |= black_promotions;
        printf("Black piece promoted to King!\n");
    }
}

bool CheckForWin(const CheckersGameState *game) {
    if (CountBits(game->black_pieces) == 0) {
        printf("\n*** Black has no pieces left. Red wins! ***\n");
        return true;
    }
    if (CountBits(game->red_pieces) == 0) {
        printf("\n*** Red has no pieces left. Black wins! ***\n");
        return true;
    }
    return false;
}

int main() {
    CheckersGameState game = StartGame();
    char input_buffer[100];
    char from_str[3], to_str[3];

    printf("Welcome to Bitboard Checkers!\n");
    printf("Enter moves using Letter-number notation (Ex:'A3 B4' for a move or 'C3 E5' for a capture).\n");

    while (true) {
        DisplayBoard(&game);

        if (CheckForWin(&game)) {
            break;
        }

        printf("Enter move: ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            break;
        }

        if (sscanf(input_buffer, "%2s %2s", from_str, to_str) != 2) {
            printf("Invalid input format. Please try again.\n");
            continue;
        }

        int from_pos = SquareToPos(from_str);
        int to_pos = SquareToPos(to_str);

        if (from_pos == -1 || to_pos == -1) {
            printf("Invalid square name. Please use A1-H8.\n");
            continue;
        }

        if (ExecuteMove(&game, from_pos, to_pos)) {
            CheckForPromotion(&game);
            game.current_turn = (game.current_turn == PLAYER_RED) ? PLAYER_BLACK : PLAYER_RED;
        }
    }

    printf("Game over. Good game :)\n");
    return 0;
}