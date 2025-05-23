#include "prosystem.h"
#define DMA_CHECK
#define VERTICAL_SCROLLING
#define _MS_DL_SIZE 64
#define _MS_DL_MALLOC(y) ((y == 6 || y == 7 || y == 8)?_MS_DL_SIZE * 2:_MS_DL_SIZE)
#include "multisprite.h"

// Generated from sprites7800 "Bubble Bobble.yaml" > "Bubble Bobble.c"
#include "resources/Bubble Bobble.c"

char i, counter, xpos, ypos;
char *ptr;
char xchest;

#define NB_SPRITES 32 
ramchip short sp_xpos[NB_SPRITES], sp_ypos[NB_SPRITES];
ramchip char sp_direction[NB_SPRITES];

const signed short dx[24] = {300, 289, 259, 212, 149, 77, 0, -77, -150, -212, -259, -289, -300, -289, -259, -212, -149, -77, 0, 77, 149, 212, 259, 289};
const signed short dy[24] = {0, 124, 240, 339, 415, 463, 480, 463, 415, 339, 240, 124, 0, -124, -239, -339, -415, -463, -480, -463, -415, -339, -240, -124};
const char horizontal_pingpong[24] = { 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13 };
const char vertical_pingpong[24] = { 0, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

const char background[22] = { 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 };
#define LTR(x) (((x) - ' ') * 2)
const char hello_world[] = { LTR('H'), LTR('E'), LTR('L'), LTR('L'), LTR('O'), LTR(' '), LTR('W'), LTR('O'), LTR('R'), LTR('L'), LTR('D') };

void main()
{
    counter = 0;

    multisprite_init();
    multisprite_set_charbase(tiles);
   
    // Set up a full background 
    for (counter = 0; counter < _MS_DLL_ARRAY_SIZE; counter++) {
        if (counter & 2) {
            ptr = background + 2;
        } else {
            ptr = background;
        }
        multisprite_display_tiles(0, _MS_DLL_ARRAY_SIZE - 1 - counter, ptr, 20, 1);
    }
    multisprite_save();

    *P0C1 = multisprite_color(0x1c); // Setup Palette 0: Yellow
    *P0C2 = multisprite_color(0xc5); // Green
    *P0C3 = 0x0f; // White
   
    *P1C1 = multisprite_color(0x55); // Dark pink
    *P1C2 = multisprite_color(0x5B); // Light pink

    *P2C1 = multisprite_color(0x32);
    *P2C2 = multisprite_color(0x3D);
    *P2C3 = multisprite_color(0x37);

    *P3C1 = multisprite_color(0x92);
    *P3C2 = multisprite_color(0x97);
    *P3C3 = multisprite_color(0x9D);

    // Initialize sprites
    for (ypos = 0, xpos = 0, i = 0, X = 0; X != NB_SPRITES; xpos++, ypos++, X++) {
        sp_xpos[X] = xpos << 8;
        sp_ypos[X] = ypos << 8;
        sp_direction[X] = i++;
        if (i == 24) i = 0;
    }

    // Main loop
    do {
        // Prepare scrolling data
        if (multisprite_vscroll_buffer_empty()) {
            if (counter & 2) {
                ptr = background + 2;
            } else {
                ptr = background;
            }
            multisprite_vscroll_buffer_tiles(0, ptr, 20, 1);
            multisprite_vscroll_buffer_sprite(xchest, chest, 2, 3);
            xchest += 45;
            counter++;
        }

        while (*MSTAT & 0x80); 
        multisprite_flip();
        multisprite_vertical_scrolling(1);
        multisprite_reserve_dma(104, sizeof(hello_world), 2);
        for (i = 0; i != NB_SPRITES; i++) {
            X = i;
            Y = sp_direction[X];
            sp_xpos[X] += dx[Y];
            sp_ypos[X] += dy[Y];
            xpos = sp_xpos[X] >> 8;
            ypos = sp_ypos[X] >> 8;
            if ((xpos < 5 && (dx[Y] >> 8) < 0) || 
                (xpos >= 150 && (dx[Y] >> 8) >= 0)) {
                sp_direction[X] = horizontal_pingpong[Y];
            }
            if ((ypos < 5 && (dy[Y] >> 8) < 0) || 
                (ypos >= MS_YMAX - 20 && (dy[Y] >> 8) >= 0)) {
                sp_direction[X] = vertical_pingpong[Y];
            }
            multisprite_display_sprite(xpos, ypos, bb_char1, 2, 0); 
        }
        for (xpos = 40, i = 0; i != sizeof(hello_world); xpos += 8, i++) {
            ptr = chars0 + hello_world[X = i];
            multisprite_display_sprite_fast(xpos, 104, ptr, 2, 2);
        }
    } while(1);
}
