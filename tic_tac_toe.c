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








/* 
 * [M3 FLOW] main
 * Runs the whole program. It has two screens, chosen by the variable
 * "playing": 0 = settings screen, 1 = the game.
 * The [M2 INTERFACE] drawing code sits inside the same loop, because the
 * window is drawn again in every frame.
 */

int main(void)
{
    /* ---- [M3 FLOW] open the window and set up the program state ---- */
    InitWindow(600, 640, "Tic-Tac-Toe");
    SetTargetFPS(60);                                 /* 60 frames per second */

    int  playing = 0, over = 0;     /* playing: 0 = settings, 1 = game
                                     * over   : 1 once someone has won or the board is full */
    char turn = 'X', win = ' ';     /* turn: whose move it is
                                     * win : the winner, ' ' = nobody yet */

    while (!WindowShouldClose()) {  /* [M3] one pass of this loop = one frame; ends on Esc or closing the window */
        BeginDrawing();
        ClearBackground(RAYWHITE);                    /* wipe the previous frame */

        if (!playing) {                                   /* ---- settings ---- */


            if (IsKeyPressed(KEY_RIGHT) && N < MAX_N) N++;
            if (IsKeyPressed(KEY_LEFT)  && N > 3)     N--;
            if (IsKeyPressed(KEY_UP)    && W < N)     W++;
            if (IsKeyPressed(KEY_DOWN)  && W > 3)     W--;
            if (W > N) W = N;       /* a smaller board can't need a longer line */

            /* [M2 INTERFACE] draw the settings screen */
            DrawText("SETTINGS", 190, 80, 48, DARKGRAY);
            DrawText(TextFormat("Board size:  %d x %d", N, N), 120, 220, 36, DARKBLUE);
            DrawText(TextFormat("Win with:  %d in a row", W), 120, 300, 36, DARKBLUE);
            DrawText("Left/Right = size    Up/Down = win length", 100, 420, 20, GRAY);
            DrawText("Press ENTER to start", 190, 480, 24, DARKGRAY);
           
            /* [M3 FLOW] Enter starts a new game: M1 builds an empty board createBoard(),
             * X goes first, clear the winner, and switch to the game screen */
            if (IsKeyPressed(KEY_ENTER)) {
                if (createBoard()) break; // createBoard() returns 1 if malloc fails
                turn = 'X'; win = ' '; over = 0; playing = 1;
            }
        } else {                                          /* ---- game ---- */

            int cell = BOARD / N, x0 = (600 - cell * N) / 2;

            if (!over && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();           /* m.x, m.y = cursor in pixels */
                int c = (int)(m.x - x0) / cell, r = (int)(m.y - TOP) / cell;
                if (m.x >= x0 && m.x < x0 + N * cell && m.y >= TOP && m.y < TOP + N * cell
                    && b[r][c] == ' ') {
                    b[r][c] = turn;                       /* place the mark */
                    win = winner();                       /* [M1 LOGIC] did this move win? */
                    if (win != ' ' || full()) over = 1;   /* [M1 LOGIC] a win, or a full board = draw */
                    else turn = (turn == 'X') ? 'O' : 'X';/* game goes on: pass the turn */
                }
            }
            /* R after a game ends goes back to the settings screen */
            if (over && IsKeyPressed(KEY_R)) playing = 0;  /* back to settings */

           /* status line */
            /* [M2 INTERFACE] the sentence at the top: who won, a draw, or whose turn it
            const char *msg = win != ' ' ? TextFormat("%c wins!  (R = new game)", win)
                            : over        ? "Draw!  (R = new game)"
                                          : TextFormat("%c's turn", turn);
            DrawText(msg, (600 - MeasureText(msg, 36)) / 2, 40, 36, DARKGRAY);

            /* grid and marks */
            /* [M2 INTERFACE] grid: N squares per side need N + 1 lines each way.
             * Line i is i squares in from the board's left (or top) edge.
             * The first DrawLine in the loop is vertical, the second horizontal.
             * fs = font size of the X and O: 3/4 of a square */
            int fs = cell * 3 / 4;
            for (int i = 0; i <= N; i++) {
                DrawLine(x0 + i * cell, TOP, x0 + i * cell, TOP + N * cell, DARKGRAY);
                DrawLine(x0, TOP + i * cell, x0 + N * cell, TOP + i * cell, DARKGRAY);
            }
            /* [M2 INTERFACE] marks: each square's letter is drawn as text, centred
             * in the square. */
            for (int r = 0; r < N; r++)
                for (int c = 0; c < N; c++) {
                    char s[2] = { b[r][c], 0 };
                    DrawText(s, x0 + c * cell + (cell - MeasureText(s, fs)) / 2,
                                TOP + r * cell + (cell - fs) / 2, fs,
                                b[r][c] == 'X' ? DARKBLUE : MAROON);
                }



            if (!playing) freeBoard();
        }
        EndDrawing();
    }
    if (playing) freeBoard();                         /* in case window closed in the middle of a game */
    CloseWindow();                                    /* close the window and quit */
    return 0;
}
