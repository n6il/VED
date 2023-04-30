/*  Copyright (c) 1982, 1983, 1986 by Manx Software Systems */
/*  Copyright (c) 1982, 1983, 1986 by Jim Goodnow II        */

#include    "ved.h"

#ifdef __linux  /* Overrides FLEX09 */
#include <termios.h>
struct termios termios, orig;
#else
#ifdef FLEX09
#include <sgtty.h>
#else
/* Unknown OS */
#endif
#endif

init()
{

#ifdef __linux  /* Overrides FLEX09 */

#else
#ifdef FLEX09
    struct sgttyb ss;
#else
/* Unknown OS */
#endif
#endif

    char *malloc();

    if (Tabwidth == 0)
        Tabwidth = 4;

#ifdef __linux  /* Overrides FLEX09 */

#else
#ifdef FLEX09

    /* Allocate 1280 bytes of stack, since stackend reports StackEnd = 041e */
    setstack(1280);

#else
/* Unknown OS */
#endif
#endif

    /* Allocate mem for the editbuffer */
    Max_mem = 32000;
    while ((Mem_buf = malloc(Max_mem)) == 0)
        Max_mem -= 2000;
    free(Mem_buf);
    Max_mem -= 1024;
    Mem_buf = malloc(Max_mem);

    Width = SCR_WID;
    Lwidth = Width + 1;
    clr_scrn();

#ifdef __linux  /* Overrides FLEX09 */
    GetSetTerm(0);
#else
#ifdef FLEX09
    gtty(0, &ss);
    ss.sg_flags = CBREAK | RAW | IOSPCL;
    stty(0, &ss);
#else
/* Unknown OS */
#endif
#endif

}

#ifdef __linux  /* Overrides FLEX09 */
void
GetSetTerm(int set)
{
        /* See http://man7.org/linux/man-pages/man3/termios.3.html */
    struct termios *termiop = &orig;
    if(!set) {
        tcgetattr(0,&orig);
        termios = orig;
        termios.c_lflag    &= (~ICANON & ~ECHO & ~ISIG);
        termios.c_iflag    &= ~(IXON | ICRNL | INLCR );
        termios.c_cc[VMIN]  = 1;
        termios.c_cc[VTIME] = 0;
        termiop = &termios;
    }
    tcsetattr(0, TCSANOW, termiop);
}


quit()
{
    mv_curs(0, (SCR_BOT-1));
    pcrlf (); /* write(1, "\n\i", 2); */
    clr_scrn();
    GetSetTerm(1);
    free(Mem_buf);

    if(errflag != 0)
    {
       printf("\nError: %s\n",errmsg);
    }
    exit(0);
}
#else
#ifdef FLEX09
quit()
{
    struct sgttyb ss;

    mv_curs(0, (SCR_BOT-1));
    pcrlf (); /* write(1, "\n\i", 2); */
    free(Mem_buf);

    if(errflag != 0)
    {
       printf("\nError: %s\n",errmsg);
    }
    /* printf("StackEnd = %04x\n",stacksize()); */
    exit(0);
}
#else
/* Unknown OS */
#endif
#endif




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

/*
up_scrl(n)
int n;
{
    mv_curs(0, (SCR_BOT-1));
    while (n--)
        pcrlf (); 
    fix_msg();
}
*/

up_scrl(n)
int n;
{
    mv_curs(0, STATUS);
    scr_lindel();
    fix_msg();
    mv_curs(0, (SCR_BOT-1));
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


