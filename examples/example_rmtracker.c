#include "prosystem.h"
#include "rmtplayer.h"
#include "conio.h"

// Converted with rmt2cc7800 Alien\ 3\ Title.rmt > Alien\ 3\ Title.rmt.c
#include "resources/Alien 3 Title.rmt.c"

char i, j, k, l;

void main()
{
    clrscr();
    gotoxy(6, 0);
    textcolor(2);
    cputs("Raster Music Tracker Player");

    for (i = 0; i != 4; i++) {
        gotoxy(14 + (i << 2), 19);
        putch('1' + i);
    }

    pokey_init(RMTSTART);

    textcolor(1);

    i = 0;

    do {
        // Display the volume for each channel
        k = 14 + (i << 2);
        l = 17 - (trackn_audc[Y = i] & 0x0f);
        for (j = 2; j < l; j++) {
            gotoxy(k, j);
            putch(' ');
        }
        for (; j < 18; j++) {
            gotoxy(k, j);
            putch(127);
        }
        i++;
        if (i == 4) i = 0;

        *BACKGRND = 0x00;
        while (*MSTAT & 0x80); // Wait for end of VBLANK
        while (!(*MSTAT & 0x80)); // Wait for VBLANK
        *BACKGRND = 0x0f;
        pokey_play();
    } while(1);
}
