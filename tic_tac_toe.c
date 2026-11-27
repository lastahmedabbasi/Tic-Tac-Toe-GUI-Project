/* Tic-Tac-Toe (2 players) - raylib
 *
 * The players choose the board size (3 to 8) and how many marks in a row
 * are needed to win on a settings screen, then play by clicking squares.
 *
 * The program has three modules, one per team member. Every part of the
 * code below is tagged, so you can search for the tag to find your part:
 *
 *   [M1 LOGIC]      Game logic : the board data (made with malloc),
 *                                createBoard(), freeBoard(), winner(), full()
 *   - Ahmed Abbasi 26K-0004
 *   [M2 INTERFACE]  Interface  : everything that is DRAWN on screen
 *                                (settings text, status line, grid, X and O)
 *   - Shella Shah Alam 26K-0008
 *   [M3 FLOW]       Game flow  : the main loop - keys, clicks, turns, and
 *                                switching between settings and game
 *   - Muhammad Mujtaba 26K-0020
 *
 * M2 and M3 both sit inside main(), because raylib draws the whole window
 * again in every frame (60 times a second) and the loop has to read input,
 * update the game and draw in one pass. The tags show which lines belong to
 * which module.
 */

#include <stdlib.h>  
#include "raylib.h"

#define MAX_N 8
#define BOARD 480      /* board width in pixels */
#define TOP   120      /* space above the board */

/* ========================================================================
 * [M1 LOGIC] Game logic
 * Holds the board and applies the rules. Nothing here draws or reads input.
 * ======================================================================== */

static char **b;               /* the board: 'X', 'O' or ' ' */
                               /* b[row][col]. It is a list of N rows, and each row is a list
                                * of N squares, all made with malloc in createBoard() */
static int  N = 3, W = 3;      /* board size, marks in a row to win */
                               /* N = rows AND columns, W = winning length (3 up to N) */

int createBoard(void)
{
    b = malloc(N * sizeof(char *));                   /* the list of N rows */
    if (b == NULL) return 1;                          /* no memory for the list */

    for (int r = 0; r < N; r++) {
        b[r] = malloc(N);                             /* one row: N squares */
        if (b[r] == NULL) {                           /* this row failed: undo everything */
            for (int k = 0; k < r; k++)
                free(b[k]);                           /* free the rows made so far */
            free(b);                                  /* then free the list of rows */
            b = NULL;
            return 1;
        }
        for (int c = 0; c < N; c++)
            b[r][c] = ' ';                            /* empty square */
    }
    return 0;                                         /* all allocations worked */
}

void freeBoard(void)
{
    for (int r = 0; r < N; r++)
        free(b[r]);
    free(b);
}

char winner(void)
{
    static const int d[4][2] = { {0,1}, {1,0}, {1,1}, {1,-1} };
    for (int r = 0; r < N; r++)                       /* every start row    */
        for (int c = 0; c < N; c++) {                 /* every start column */
            if (b[r][c] == ' ') continue;             /* empty: can't start a line here */
            for (int i = 0; i < 4; i++) {             /* each of the 4 directions */
                int k;
                for (k = 1; k < W; k++) {
                    int rr = r + d[i][0] * k, cc = c + d[i][1] * k;   /* square k steps along */
                    /* stop if off the board (checked first, so we never read outside
                     * the array) or if this square holds a different mark */
                    if (rr < 0 || rr >= N || cc < 0 || cc >= N || b[rr][cc] != b[r][c]) break;
                }
                if (k == W) return b[r][c];           /* W in a row: return who won */
            }
        }
    return ' ';                                       /* no winning line anywhere */
}

int full(void)
{
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (b[r][c] == ' ') return 0;             /* found an empty square */
    return 1;
}



    if (playing) freeBoard();                         /* [M1 LOGIC] window closed in the middle of a game */
    CloseWindow();                                    /* [M3 FLOW] close the window and quit */
    return 0;
}
