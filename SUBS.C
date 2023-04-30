/*  Copyright (c) 1982, 1986 by Manx Software Systems   */
/*  Copyright (c) 1982, 1986 by Jim Goodnow II      */

#include    "ved.h"

pcrlf ()
{

#ifdef __linux  /* Overrides FLEX09 */
    write(1, "\015\012",2); 
#else
#ifdef FLEX09
#asm
    jsr $CD24
#endasm
#else
/* Unknown OS */
#endif
#endif

}

colon()
{
    char buf[80];
    register char *cp;
    register int c;
    static int nowrite=0;
#ifdef EXECFLAG
    extern int execflag;
#endif

    cp = buf;
    *cp++ = ':';
    for (;;) {
        *cp = 0;
        mesg(buf);
        c = getchar();
        if (c == KEY_CR)
            break;
        if (c == KEY_BS) {
            if (cp > buf+1)
                cp--;
            continue;
        }
        if (c < KEY_SP)
            continue;
        *cp++ = c;
    }
    cp = buf+1;
    switch (*cp) {
    case 'f':
        stats();
        return;
    case 'q':
        while ((c=*++cp) && c != KEY_SP)
            if (c == '!')
                nowrite=1;
#ifdef EXECFLAG
            else if (c == 'x')
                execflag=0;
#endif
        if (Modflg == 0 || nowrite)
            quit();
        mesg("file modified - use q! to override");
        return;
    case 'w':
        if (cp[1]=='q'){
#ifdef EXECFLAG
            if (cp[2]=='x')
                execflag=0;
#endif
            if (writ_fil(Fil_nam) == 0)
                quit();
            return;
        }
        while (*++cp == KEY_SP)
            ;
        if (*cp == 0)
            cp = Fil_nam;
        if (writ_fil(cp) == 0)
            Modflg = 0;
        return;
    case 'r':
        while (*++cp == KEY_SP)
            ;
        read_fil(cp);
        draw(Cur_lp, Cur_lin, Cur_y);
        return;
    case 'e':
        if (Modflg && *++cp != '!') {
            mesg("file modified - use e! to override");
            return;
        }
        if (*(cp+1) == '!')
            ++cp;
        while (*++cp == KEY_SP)
            ;
        Cur_lp = Cur_ptr = End_buf = Mem_buf;
        if (read_fil(cp) < 0)
            t_insert(Cur_ptr, 0, KEY_CR, 1);
        Modflg = 0;
        strcpy(Fil_nam, cp);
        redraw(Mem_buf, 1, SCR_TOP);
        stats();
        return;
    }
}

search(flg)
int flg;
{
    static char srch[80];
    char buf[80];
    register char *tp, *cp;
    register int c;

    cp = buf;
    *cp++ = KEY_SRCH;
    for (;;) {
        *cp = 0;
        mesg(buf);
        if (flg)
            break;
        c = getchar();
        if (c == KEY_CR)
            break;
        if (c == KEY_BS) {
            if (cp > buf+1)
                cp--;
            continue;
        }
        if (c < KEY_SP)
            continue;
        *cp++ = c;
    }
    if (buf[1])
        strcpy(srch, buf+1);
    if (srch[0] == 0) {
        mesg("no previous search string");
        return;
    }
    *End_buf = 0;
    tp = Cur_ptr + 1;
    c = strlen(srch);
    for (;;) {
        cp = index(tp, srch[0]);
        if (cp == 0) {
            mesg("no match");
            return;
        }
        if (strncmp(cp, srch, c) == 0) {
            Cur_lp = get_prev(get_next(Cur_ptr = cp));
            if (Cur_lp >= Top_lp && Cur_lp < Bot_lp) {
                cp = Top_lp;
                Cur_lin = Top_lin;
                Cur_y = Top_y;
                while (cp < Cur_lp) {
                    set_param(cp);
                    Cur_y += Lin_siz;
                    Cur_lin++;
                    cp = get_next(cp);
                }
                return;
            }
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            return;
        }
        tp = cp + 1;
    }
}

get_num(c)
int c;
{
    char buf[10];
    register char *cp;

    Num = 0;
    cp = buf;
    for (;;) {
        if (c < '0' || c > '9') {
            Val = Num;
            if (Num == 0)
                Num = 1;
            return(c);
        }
        Num = Num * 10 + c - '0';
        *cp++ = c;
        *cp = 0;
        mesg(buf);
        mv_curs(Cur_x, Cur_y);
        c = getchar();
    }
}

stats()
{
    char buf[80];
    register char *cp;
    register int c;

    maknam(buf, Fil_nam);
    strcat(buf, "line ");
    itoa(buf+strlen(buf), Cur_lin);
    strcat(buf, " of ");
    cp = Mem_buf;
    c = 1;
    while ((cp = get_next(cp)) != End_buf)
        ++c;
    itoa(buf+strlen(buf), c);
    mesg(buf);
}

itoa(buf, num)
char *buf;
int num;
{
    register int flg = 0;
    register int i, j = 1000;

    while (j) {
        i = num/j;
        num %= j;
        j /= 10;
        if (i == 0 && flg == 0)
            continue;
        *buf++ = i + '0';
        flg = 1;
    }
    if (flg == 0)
        *buf++ = '0';
    *buf = 0;
}

char *helpstr[] = {
"i/Esc o/Esc enter/leave INSERT mode",
"return/-    beginning of next/previous line",
"k/j         line up/down",
"spc or l    next character",
"bspc or h   previous character",
"0(zero)/$   beginning/end of line",
"Ctl-B/Ctl-F page backwards/forward",
"T/M/B       top/middle/bottom of screen",
"g           go to line",
"/string     search for string",
"n           repeat last search",
"Ctrl-R or z redraw the screen",
"r           replace character",
"x           delete characters",
"dd          delete lines",
"yy          yank lines",
"yd          yank and delete lines",
"p           paste yanked lines",
":q[!][x]    quit",
":w or :r    write file or read file",
":e          edit new file",
":f          file stats",
0
};

help()
{
    register char **cp;
    char msg[80];

    clr_scrn();
    mv_curs(0, SCR_TOP+1);
    for (cp=helpstr;*cp;cp++)
    {
        write(1, *cp, strlen(*cp));
        pcrlf ();
    }
    
    sprintf(msg,"%s %s - hit return to continue",PROGRAM_NAME,VERSION);
    mesg(msg);

    while (getchar() != KEY_CR)
        ;
    clr_scrn();
    mesg("");
    redraw(Cur_lp, Cur_lin, Cur_y);
}

blockmv(dst,src,cnt)
char *dst, *src;
{
    movmem(src,dst,cnt);
}
