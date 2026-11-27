/* ========================================================================
 * Tic-Tac-Toe (2 players) - raylib
 *
 * The players choose the board size (3 to 8) and how many marks in a row
 * are needed to win on a settings screen, then play by clicking squares.
 *
 * The program has three modules, one per team member. Every part of the
 * code below is tagged, so you can search for the tag to find your part:
 *
 *   [M1 LOGIC]      Game logic : the board data (made with malloc),
 *                                createBoard(), freeBoard(), winner(), full()
 *   [M2 INTERFACE]  Interface  : everything that is DRAWN on screen
 *                                (settings text, status line, grid, X and O)
 *   [M3 FLOW]       Game flow  : the main loop - keys, clicks, turns, and
 *                                switching between settings and game
 *
 * M2 and M3 both sit inside main(), because raylib draws the whole window
 * again in every frame (60 times a second) and the loop has to read input,
 * update the game and draw in one pass. The tags show which lines belong to
 * which module.
 *
 * raylib: positions are pixels from the TOP-LEFT corner of the window
 * (x goes right, y goes DOWN).
 * ======================================================================== */
#include <stdlib.h>   /* malloc and free */
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

/* ------------------------------------------------------------------------
 * [M1 LOGIC] createBoard
 * Makes an N x N board in memory and sets every square to ' ' (empty).
 * b is a list of N rows (b[0] ... b[N-1]) and each row is a list of N
 * characters, so b[r][c] works just like a normal 2D array.
 *
 * malloc returns NULL when the computer has no memory left, so every
 * malloc result is checked before it is used.
 *
 * Returns : 0 if the board was made (success)
 *           1 if memory ran out (failure). In that case everything that was
 *             already allocated has been freed again, and b is NULL.
 * ------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------
 * [M1 LOGIC] freeBoard
 * Gives the board's memory back. Free every row first, then the list of
 * rows (if the list went first, the rows' addresses would be lost).
 * It must run while N still holds the size the board was created with.
 * ------------------------------------------------------------------------ */
void freeBoard(void)
{
    for (int r = 0; r < N; r++)
        free(b[r]);
    free(b);
}

/* ------------------------------------------------------------------------
 * [M1 LOGIC] winner
 * Returns 'X' or 'O' if someone has W in a row, else ' '
 *
 * How: from every filled square, look in 4 directions for W identical marks.
 * Four are enough because the other four are the same lines seen from the
 * other end.
 *      d[i] = { row step, column step }
 *      {0,1} right   {1,0} down   {1,1} down-right   {1,-1} down-left
 * k counts the steps away from the start square. The loop stops early if it
 * leaves the board or meets a different mark. If it was NOT stopped, k
 * reaches W, which means W marks matched, so that player has won.
 * ------------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------------
 * [M1 LOGIC] full
 * Returns 1 if the board has no empty square left, otherwise 0.
 * It is only used after winner() found no winner, so "full" means a draw.
 * ------------------------------------------------------------------------ */
int full(void)
{
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (b[r][c] == ' ') return 0;             /* found an empty square */
    return 1;
}

/* ========================================================================
 * [M3 FLOW] main
 * Runs the whole program. It has two screens, chosen by the variable
 * "playing": 0 = settings screen, 1 = the game.
 * The [M2 INTERFACE] drawing code sits inside the same loop, because the
 * window is drawn again in every frame.
 * ======================================================================== */
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

            /* [M3 FLOW] arrow keys change the settings, kept inside their limits:
             * size between 3 and MAX_N, winning length between 3 and the size.
             * IsKeyPressed is true on the single frame the key goes down. */
            if (IsKeyPressed(KEY_RIGHT) && N < MAX_N) N++;
            if (IsKeyPressed(KEY_LEFT)  && N > 3)     N--;
            if (IsKeyPressed(KEY_UP)    && W < N)     W++;
            if (IsKeyPressed(KEY_DOWN)  && W > 3)     W--;
            if (W > N) W = N;       /* a smaller board can't need a longer line */

            /* [M2 INTERFACE] draw the settings screen
             * DrawText(text, x, y, fontSize, colour); TextFormat is like printf
             * but returns the finished text. */
            DrawText("SETTINGS", 190, 80, 48, DARKGRAY);
            DrawText(TextFormat("Board size:  %d x %d", N, N), 120, 220, 36, DARKBLUE);
            DrawText(TextFormat("Win with:  %d in a row", W), 120, 300, 36, DARKBLUE);
            DrawText("Left/Right = size    Up/Down = win length", 100, 420, 20, GRAY);
            DrawText("Press ENTER to start", 190, 480, 24, DARKGRAY);

            /* [M3 FLOW] Enter starts a new game: M1 builds an empty board,
             * X goes first, clear the winner, and switch to the game screen */
            if (IsKeyPressed(KEY_ENTER)) {
                /* [M1 LOGIC] make the board, all squares empty. It returns 1 if memory ran
                 * out. createBoard has already freed what it made, so just leave the loop
                 * (playing is still 0, so nothing is freed a second time below). */
                if (createBoard()) break;
                turn = 'X'; win = ' '; over = 0; playing = 1;
            }
        } else {                                          /* ---- game ---- */

            /* [M2 INTERFACE] layout numbers, shared with the click code below:
             *   cell = width of one square in pixels (the board is always BOARD wide)
             *   x0   = left edge of the board, chosen so the board is centred */
            int cell = BOARD / N, x0 = (600 - cell * N) / 2;

            /* [M3 FLOW] a click on the board.
             * IsMouseButtonPressed is true only on the frame the click starts,
             * so one click places one mark. Clicks are ignored once the game is over. */
            if (!over && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();           /* m.x, m.y = cursor in pixels */
                /* pixels -> square: take off where the board starts, then divide by
                 * the square width. c = column (across), r = row (down) */
                int c = (int)(m.x - x0) / cell, r = (int)(m.y - TOP) / cell;
                /* only use r and c if the click is inside the board AND the square is
                 * empty. The checks run left to right, so b[r][c] is only read once
                 * r and c are known to be on the board. */
                if (m.x >= x0 && m.x < x0 + N * cell && m.y >= TOP && m.y < TOP + N * cell
                    && b[r][c] == ' ') {
                    b[r][c] = turn;                       /* place the mark */
                    win = winner();                       /* [M1 LOGIC] did this move win? */
                    if (win != ' ' || full()) over = 1;   /* [M1 LOGIC] a win, or a full board = draw */
                    else turn = (turn == 'X') ? 'O' : 'X';/* game goes on: pass the turn */
                }
            }
            /* [M3 FLOW] R after a game ends goes back to the settings screen */
            if (over && IsKeyPressed(KEY_R)) playing = 0;  /* back to settings */

            /* status line */
            /* [M2 INTERFACE] the sentence at the top: who won, a draw, or whose turn it
             * is. "a ? b : c" is a short if/else. It is drawn centred:
             * x = (window width - text width) / 2 */
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
             * in the square. s is the letter as a 1-character string. An empty
             * square holds ' ', so it draws nothing visible. */
            for (int r = 0; r < N; r++)
                for (int c = 0; c < N; c++) {
                    char s[2] = { b[r][c], 0 };
                    DrawText(s, x0 + c * cell + (cell - MeasureText(s, fs)) / 2,
                                TOP + r * cell + (cell - fs) / 2, fs,
                                b[r][c] == 'X' ? DARKBLUE : MAROON);
                }

            /* [M1 LOGIC] if R was pressed above, the game is over: free the board.
             * This comes last on purpose, after the board was drawn this frame,
             * and while N still holds the size the board was created with. */
            if (!playing) freeBoard();
        }
        EndDrawing();
    }
    if (playing) freeBoard();                         /* [M1 LOGIC] window closed in the middle of a game */
    CloseWindow();                                    /* [M3 FLOW] close the window and quit */
    return 0;
}
