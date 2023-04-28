/*  Copyright (c) 1982, 1983, 1986 by Manx Software Systems */
/*  Copyright (c) 1982, 1983, 1986 by Jim Goodnow II        */

#include    "ved.h"
#include <sgtty.h>

init()
{
    struct sgttyb ss;
    char *malloc();

    if (Tabwidth == 0)
        Tabwidth = 4;
    Max_mem = 32000;
    while ((Mem_buf = malloc(Max_mem)) == 0)
        Max_mem -= 2000;
    free(Mem_buf);
    Max_mem -= 1024;
    Mem_buf = malloc(Max_mem);
    Width = 51;
    Lwidth = Width + 1;
    clr_scrn();
    gtty(0, &ss);
    ss.sg_flags = CBREAK | RAW | IOSPCL;
    stty(0, &ss);
}

quit()
{
    struct sgttyb ss;

    mv_curs(0, 23);
    pcrlf (); /* write(1, "\n\i", 2); */
    exit(0);
}

/*
 *  Move the cursor to the appropriate position.
 */

mv_curs(x, y)
register int x, y;
{
    char buf[4];

    if (x >= Width) {
        y += x / Width;
        x = x % Width;
    }
    scr_curs(y,x);
}

/*
 *  This routine beeps.
 */

beep()
{
    scr_beep();
}

/*
 *  This routine puts a message on the status line.
 */

mesg(str)
char *str;
{
    mv_curs(0, STATUS);
    write(1, str, strlen(str));
    clr_eol();
}

up_scrl(n)
int n;
{
    mv_curs(0, 23);
    while (n--)
        pcrlf (); /* write(1,"\n\i", 2); */
    fix_msg();
}

getchar()
{
    char c;

    read(0, &c, 1);
    return(c);
}

/*
 *  This routine clears from the current cursor position to the end of the line.
 */

clr_eol()
{
    scr_eol();
}

clr_scrn()
{
    scr_clear();
}

dn_scrl()
{
    mv_curs(0,STATUS+1);
    scr_linsert();
}
