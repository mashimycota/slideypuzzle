#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include "gfx.h"

#define IDX(val)        (val - 1)
#define SETBIT(A, n)    (A |= 1 << n)
#define CLEARBIT(A, n)	(A &= ~(1 << n))
#define WIN_COND(size)	((1 << ((size * size) - 1)) - 1)

typedef struct block_t {
    unsigned char val;
    unsigned char correct_val;
    struct block_t *up;
    struct block_t *right;
    struct block_t *down;
    struct block_t *left;
} block;
    
unsigned int correct_spots;

unsigned int move_count;

unsigned short BOARD_SIZE;

void check_block(block *b) {
    if (b->correct_val == b->val) {
	    SETBIT(correct_spots, IDX(b->val));
    } else {
	    CLEARBIT(correct_spots, IDX(b->val));
    }
}

block *swap(block *b1, block *b2) {
    int temp = b1->val;
    b1->val = b2->val;
    b2->val = temp;
    check_block(b1);
    check_block(b2);
    draw_block(b1->correct_val, b1->val);
    draw_block(b2->correct_val, b2->val);
    return b2;
}

// init blocks and their pointers
block **init_blocks() {
    block **arr = malloc(sizeof(block *) * BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++) {
	    arr[i] = malloc(sizeof(block) * BOARD_SIZE);
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            arr[i][j].correct_val = (i * BOARD_SIZE) + j + 1;
            arr[i][j].val = (i * BOARD_SIZE) + j + 1;		
            arr[i][j].up = i != 0 ? &arr[i - 1][j] : NULL;
            arr[i][j].down = i != IDX(BOARD_SIZE) ? &arr[i + 1][j] : NULL;
            arr[i][j].right = j != IDX(BOARD_SIZE) ? &arr[i][j + 1] : NULL;
            arr[i][j].left = j != 0 ? &arr[i][j - 1] : NULL;
        
        }
    }
    // Set empty block
    arr[IDX(BOARD_SIZE)][IDX(BOARD_SIZE)].val = 0;
    return arr;
}

// Makes num random moves. If pointer to a direction is NULL, move
// in the opposite direction instead.
block *scramble(block **board, int num) {
    block *cur = &board[IDX(BOARD_SIZE)][IDX(BOARD_SIZE)];
    block *swp;
    srand(time(NULL));
    do {
        int dir = rand() % 4;
        switch (dir) {
            case 0:
                mv_left:
                if (!cur->left) {
                    goto mv_right;
                }
                swp = cur->left;
                break;
            case 1:
                mv_up:
                if (!cur->up) {
                    goto mv_down;
                }
                swp = cur->up;
                break;
            case 2:
                mv_right:
                if (!cur->right) {
                    goto mv_left;
                }
                swp = cur->right;
                break;
            case 3:
                mv_down:
                if (!cur->down) {
                    goto mv_up;
                }
                swp = cur->down;
                break;
            }
        cur = swap(cur, swp);
        num--;
    } while (num > 0);
    return cur;
}

void redraw(block **board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int value = board[i][j].val;
            int correct = board[i][j].correct_val;
            draw_block(correct, value);
        }
    }
}

// Return 0 if game is exited, -1 if request to restart
int play(block **board) {
    correct_spots = 0;
    move_count = 0;
    block *cur_ptr = scramble(board, BOARD_SIZE * 1000);
    refresh();
    char c;
    while (correct_spots != WIN_COND(BOARD_SIZE)) {
        c = getch();
        switch (c) {
            case ('w'):
                move_count++;
                cur_ptr = cur_ptr->up != NULL ? swap(cur_ptr, cur_ptr->up) : cur_ptr;
                break;
            case ('a'):
                move_count++;
                cur_ptr = cur_ptr->left != NULL ? swap(cur_ptr, cur_ptr->left) : cur_ptr;
                break;
            case ('s'):
                move_count++;
                cur_ptr = cur_ptr->down != NULL ? swap(cur_ptr, cur_ptr->down) : cur_ptr;
                break;
            case ('d'):
                move_count++;
                cur_ptr = cur_ptr->right != NULL ? swap(cur_ptr, cur_ptr->right) : cur_ptr;
                break;
            case ('q'):
                return 0;
            case ('r'):
                return -1;
            default:
                break;
        }
	refresh();
    }
    return move_count;
}

void freemem(block **board) {
    endwin();
    for (int i = 0; i < BOARD_SIZE; i++) {
	    free(board[i]);
    }
    free(board);
}

int main(int argc, char **argv){
    if (argc != 2) {
        fprintf(stderr, "Usage: ./slidey [size of grid (2-16)]\nControls: WASD, Q to Exit, R to restart\n");
        exit(1);
    }
    char *ptr;
    BOARD_SIZE = strtol(argv[1], &ptr, 10);
    if (BOARD_SIZE < 2 || BOARD_SIZE > 16) {
        fprintf(stderr, "Board sizes from 2 to 16 are supported\n");
        exit(1);
    }

    play: ;
    block **board = init_blocks();
    init();
    int mov = play(board);
    if (mov == -1) 
        goto play;
    else if (mov) 
        win();
    if (getch() == 10) {
        erase_win();
        goto play;
    }
    
    freemem(board);
    return 0;
}