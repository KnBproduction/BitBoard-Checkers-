#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bit_ops.h"

#define RED_START   0x00000FFF00000000ULL
#define BLACK_START 0x00000000000FFF00ULL
#define VALID_SQUARES 0x55AA55AA55AA55AAULL

typedef struct {
    unsigned long long r_peices;
    unsigned long long r_kings;
    unsigned long long b_peices;
    unsigned long long b_kings;
    int red_turn;
} GameState;

int CountBits(unsigned long long x) {
    int c = 0;
    while (x) { c += x & 1ULL; x >>= 1ULL; }
    return c;
}

void PrintBoard(GameState *g) {
    printf("\nBoard (top row printed first). Symbols: r/R red, b/B black, '.' empty\n\n");
    for (int r = 7; r >= 0; --r) {
        for (int c = 0; c < 8; ++c) {
            int pos = r * 8 + c;
            unsigned long long mask = 1ULL << pos;
            char ch = '.';
            if (g->r_peices & mask) ch = 'r';
            if (g->b_peices & mask) ch = 'b';
            if (g->r_kings & mask) ch = 'R';
            if (g->b_kings & mask) ch = 'B';
            printf(" %c", ch);
        }
        printf("   ");
        for (int c = 0; c < 8; ++c) printf("%2d ", r * 8 + c);
        printf("\n");
    }
    printf("\n");
}

int IsOccupied(GameState *g, int pos) {
    unsigned long long all = g->r_peices | g->r_kings | g->b_peices | g->b_kings;
    return (all >> pos) & 1ULL;
}

int IsRed(GameState *g, int pos) {
    unsigned long long all_red = g->r_peices | g->r_kings;
    return (all_red >> pos) & 1ULL;
}

int IsBlack(GameState *g, int pos) {
    unsigned long long all_black = g->b_peices | g->b_kings;
    return (all_black >> pos) & 1ULL;
}

int IsKing(GameState *g, int pos) {
    unsigned long long kings = g->r_kings | g->b_kings;
    return (kings >> pos) & 1ULL;
}

int MovePiece(GameState *g, int from, int to) {
    if (from < 0 || from > 63 || to < 0 || to > 63) return 0;
    if (!(VALID_SQUARES & (1ULL << to))) return 0;
    if (IsOccupied(g, to)) return 0;

    int isRed = IsRed(g, from);
    if ((g->red_turn && !isRed) || (!g->red_turn && isRed)) return 0;

    unsigned long long fromMask = 1ULL << from;
    unsigned long long toMask = 1ULL << to;
    int isKing = IsKing(g, from);

    g->r_peices &= ~fromMask;
    g->b_peices &= ~fromMask;
    g->r_kings &= ~fromMask;
    g->b_kings &= ~fromMask;

    if (isRed) {
        if (isKing) g->r_kings |= toMask;
        else g->r_peices |= toMask;
    } else {
        if (isKing) g->b_kings |= toMask;
        else g->b_peices |= toMask;
    }

    int diff = to - from;
    if (diff == 14 || diff == 18 || diff == -14 || diff == -18) {
        int mid = (from + to) / 2;
        unsigned long long midMask = 1ULL << mid;
        g->r_peices &= ~midMask;
        g->b_peices &= ~midMask;
        g->r_kings &= ~midMask;
        g->b_kings &= ~midMask;
    }

    for (int i = 0; i < 8; i++) {
        if ((g->r_peices >> (56 + i)) & 1ULL) {
            g->r_peices &= ~(1ULL << (56 + i));
            g->r_kings |= (1ULL << (56 + i));
        }
        if ((g->b_peices >> i) & 1ULL) {
            g->b_peices &= ~(1ULL << i);
            g->b_kings |= (1ULL << i);
        }
    }

    g->red_turn = !g->red_turn;
    return 1;
}

int CheckWin(GameState *g) {
    int redCount = CountBits(g->r_peices | g->r_kings);
    int blackCount = CountBits(g->b_peices | g->b_kings);
    if (redCount == 0) return 2;
    if (blackCount == 0) return 1;
    return 0;
}

int SaveGame(GameState *g, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return 0;
    fwrite(&g->r_peices, sizeof(unsigned long long), 1, f);
    fwrite(&g->r_kings, sizeof(unsigned long long), 1, f);
    fwrite(&g->b_peices, sizeof(unsigned long long), 1, f);
    fwrite(&g->b_kings, sizeof(unsigned long long), 1, f);
    fwrite(&g->red_turn, sizeof(int), 1, f);
    fclose(f);
    return 1;
}

int LoadGame(GameState *g, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;
    fread(&g->r_peices, sizeof(unsigned long long), 1, f);
    fread(&g->r_kings, sizeof(unsigned long long), 1, f);
    fread(&g->b_peices, sizeof(unsigned long long), 1, f);
    fread(&g->b_kings, sizeof(unsigned long long), 1, f);
    fread(&g->red_turn, sizeof(int), 1, f);
    fclose(f);
    return 1;
}

int main(int argc, char **argv) {
    GameState game;
    game.r_peices = RED_START;
    game.r_kings = 0ULL;
    game.b_peices = BLACK_START;
    game.b_kings = 0ULL;
    game.red_turn = 1;

    if (argc == 2) {
        if (!LoadGame(&game, argv[1])) {
            printf("Failed to load game file '%s'\n", argv[1]);
        } else {
            printf("Loaded game from '%s'\n", argv[1]);
        }
    }

    printf("BitBoard Checkers (Phase 2)\n");
    printf("Commands:\n");
    printf(" move <from> <to>\n save <filename>\n load <filename>\n quit\n");
    PrintBoard(&game);

    char command[128];
    while (1) {
        printf("%s's turn. > ", game.red_turn ? "Red" : "Black");
        if (!fgets(command, sizeof(command), stdin)) break;
        char cmd[16];
        int a, b;
        if (sscanf(command, "%15s %d %d", cmd, &a, &b) >= 1) {
            if (strcmp(cmd, "move") == 0) {
                if (!MovePiece(&game, a, b)) {
                    printf("Invalid move.\n");
                } else {
                    PrintBoard(&game);
                    game.red_turn = !game.red_turn;
                }
            } else if (strcmp(cmd, "save") == 0) {
                char fname[96];
                if (sscanf(command + 5, "%95s", fname) == 1) {
                    if (SaveGame(&game, fname)) printf("Saved to %s\n", fname);
                    else printf("Failed to save to %s\n", fname);
                } else {
                    printf("Usage: save <filename>\n");
                }
            } else if (strcmp(cmd, "load") == 0) {
                char fname[96];
                if (sscanf(command + 5, "%95s", fname) == 1) {
                    if (LoadGame(&game, fname)) {
                        printf("Loaded %s\n", fname);
                        PrintBoard(&game);
                    } else {
                        printf("Failed to load %s\n", fname);
                    }
                } else {
                    printf("Usage: load <filename>\n");
                }
            } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
                printf("Exiting.\n");
                break;
            } else {
                printf("Unknown command.\n");
            }
            int winner = CheckWin(&game);
            if (winner) {
                printf("%s wins!\n", winner == 1 ? "Red" : "Black");
                break;
            }
        }
    }
    return 0;
}