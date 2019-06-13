#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include "gfx.h"

#define IDX(n)		    (n - 1)
#define CEILDIV(n, m)   (n % m == 0 ? n / m : (n / m) + 1)

#define BOARD_ROW	    6
#define BOARD_COLM	    2
#define CUR_SCORE_ROW   4
#define MVMT_ROW        0
#define QUIT_ROW        1
#define RESET_ROW       2

#define MAGENTA		    1
#define RED		        2
#define GREEN		    3
#define CYAN		    4
#define BLUE            5
#define COLOUR_COUNT    5

#define TL_CORNER	    0
#define TR_CORNER	    3
#define HORIZ_LN	    1
#define HORIZ_DOWN	    2
#define VERT_LN	        4
#define VERT_RIGHT	    5
#define MID_CROSS	    6
#define VERT_LEFT	    7
#define BL_CORNER	    8
#define HORIZ_UP	    9
#define BR_CORNER	    10

const wchar_t *board[11] = {L"\u250F", L"\u2501", L"\u2533", L"\u2513", L"\u2503",
			   L"\u2523", L"\u254B", L"\u252B", L"\u2517", L"\u253B", L"\u251B"};

const wchar_t *numbers[10] = {L"\uff10", L"\uff11", L"\uff12", L"\uff13", L"\uff14",
			                L"\uff15", L"\uff16", L"\uff17", L"\uff18", L"\uff19"};

void draw_horiz() {
    int i = 0;
    do {
        printw("%ls", board[HORIZ_LN]);
        i++;
    } while (i < 6);
}

void draw_block(int position, int value) {
    move(CUR_SCORE_ROW, 0);
    clrtoeol();
    mvprintw(CUR_SCORE_ROW, 0, "Current moves: %d", move_count);
    int row_offset = ((position - 1) / BOARD_SIZE) * 4;
    int colm_offset = ((position - 1) % BOARD_SIZE) * 7;
    int first = value >= 10 ? value / 10 : value;
    int second = value >= 10 ? value % 10 : -1;
    int colour;
    if (value) {
        int ceiling = CEILDIV(value, BOARD_SIZE);
        int comp = value % BOARD_SIZE != 0 ? value % BOARD_SIZE : BOARD_SIZE;
        if (comp >= ceiling) 
            colour = (ceiling % COLOUR_COUNT) + 1;
        else 
            colour = ((value % BOARD_SIZE) % COLOUR_COUNT) + 1;
        attron(COLOR_PAIR(colour));
        attron(A_BOLD);
    }
    for (int i = 0; i < 3; i++) {
        if (i == 1) {
            move(BOARD_ROW + row_offset + i, BOARD_COLM + 2 + colm_offset);
            if (second == -1 && value != 0)
                printw(" %ls  ", numbers[value]);
            else if (value != 0)
                printw(" %ls%ls", numbers[first], numbers[second]);
            else 
                printw("     ");
            }
            else mvprintw(BOARD_ROW + row_offset + i, BOARD_COLM + 2 + colm_offset, "     ");
        }
    if (value) {
        attroff(COLOR_PAIR(colour));
        attroff(A_BOLD);
    }
}

void draw_board() {
    mvprintw(MVMT_ROW, 0, "wasd    Movement keys");
    mvprintw(QUIT_ROW, 0, "q       Quit the current game");
    mvprintw(RESET_ROW, 0, "r       Start new game");
    mvprintw(CUR_SCORE_ROW, 0, "Current moves: %d", 0);
    mvprintw(BOARD_ROW - 1, BOARD_COLM, "%ls", board[TL_CORNER]);
    for (int i = 0; i < BOARD_SIZE; i++) {
	draw_horiz();
	if (i != BOARD_SIZE - 1) 
	    printw("%ls", board[HORIZ_DOWN]);
    }
    printw("%ls", board[TR_CORNER]);
    for (int i = 1; i < (BOARD_SIZE * 4); i++) {
	    move(BOARD_ROW + i - 1, BOARD_COLM);
	    if (i % 4 == 0) {
            printw("%ls", board[VERT_RIGHT]);
            for (int j = 0; j < BOARD_SIZE; j++) {
                draw_horiz();
                if (j != BOARD_SIZE - 1) printw("%ls", board[MID_CROSS]);
	    }
	    printw("%ls", board[VERT_LEFT]);
	} else {
	    printw("%ls", board[VERT_LN]);
	    for (int j = 0; j < BOARD_SIZE; j++) {
                printw("      ");	
                printw("%ls", board[VERT_LN]);
            }
	    }
    }
    move(BOARD_ROW + (BOARD_SIZE * 4) - 1, BOARD_COLM);
    printw("%ls", board[BL_CORNER]);
    for (int i = 0; i < BOARD_SIZE; i++) {
        draw_horiz();
        if (i != BOARD_SIZE - 1) printw("%ls", board[HORIZ_UP]);
    }
    printw("%ls", board[BR_CORNER]);
}

void win() {
    mvprintw(BOARD_SIZE * 5 + 2, 0, "Completed with %d moves\nPress ENTER to play again", move_count);
}

void erase_win() {
    int position = BOARD_SIZE * 5 + 2;
    move(position++, 0);
    clrtoeol();
    move(position, 0);
    clrtoeol();
}

void init() {
    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);
    if (has_colors() == FALSE) {
        fprintf(stderr, "Colour is not supported in your terminal! Exiting\n");
        endwin();
        exit(1);
    }
    curs_set(0);
    noecho();
    start_color();
    draw_board();
    init_pair(RED, COLOR_WHITE, COLOR_RED);
    init_pair(GREEN, COLOR_WHITE, COLOR_GREEN);
    init_pair(CYAN, COLOR_WHITE, COLOR_CYAN);
    init_pair(MAGENTA, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(BLUE, COLOR_WHITE, COLOR_BLUE);
    refresh();
}