/* Screen functions for the TELEVIDEO 950 terminal
 *
 * Copyright (c) 2005 Evenson Consulting Services
 */

char curs[4];
char bep[1];
char eol[2];
char clr[2];
char ins[2];

scr_init ()
{
    curs[0] = eol[0] = clr[0] = ins[0] = 0x1b;
    curs[1] = '=';
    eol[1] = 'T';
    clr[1] = '*';
    ins[1] = 'E';
    bep[0] = 0x07;
}

scr_curs (y, x) int y, x;
{
    curs[2] = y + ' ';
    curs[3] = x + ' ';

    write (1, curs, 4);
}

scr_beep ()
{
    write (1, bep, 1);
}

scr_eol ()
{
    write (1, eol, 2);
}

scr_clear ()
{
    write (1, clr, 2);
}

scr_linsert ()
{
    write (1, ins, 2);
}

