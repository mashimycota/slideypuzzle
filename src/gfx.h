#ifndef GFX_H
#define GFX_H

extern unsigned short BOARD_SIZE;

extern unsigned int move_count;

void draw_block(int position, int value);

void win();

void erase_win();

void init();

#endif