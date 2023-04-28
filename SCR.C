/* Screen functions for the FHL CoCo FLEX
 *
 * 24 Apr 2023
 * Michael Furman <n6il@ocs.net>
 */

#include "ved.h"

char curs[3];
char bep[1];
char eol[1];
char clr[1];

scr_init ()
{
    curs[0] = 0x14;
    eol[0] = 0x05;
    clr[0] = 0x02;
    bep[0] = 0x07;
}

scr_curs (y, x) int y, x;
{
    curs[1] = y + ' ';
    curs[2] = x + ' ';

    write (1, curs, 3);
}

scr_beep ()
{
    write (1, bep, 1);
}

scr_eol ()
{
    write (1, eol, 1);
}

scr_clear ()
{
    write (1, clr, 1);
}

scr_linsert ()
{
    int y, cy;
    char *lp;

    y = Bot_y;
    cy = Cur_y;
    lp = Cur_lp;

    while (y > Cur_y ) {
	    lp = get_prev(lp);
	    draw(lp, 0, y--);
    }
    mv_curs(cy,0);
    scr_eol();
}
