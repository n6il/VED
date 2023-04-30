/*  Copyright (c) 1982,1986 by Manx Software Systems    */
/*  Copyright (c) 1982, 1986 by Jim Goodnow II      */

#include    "ved.h"

/*
 *  This routine draws the line pointed to by lp at position y.
 */

draw_lin(lp, d, y)
char *lp;
register int d;
int y;
{
    char c;
    register int i;

    mv_curs(d, y);
    for (;;) {
        c = *lp++;
        if (c < KEY_SP) {
            switch(c) {
            case KEY_TAB:
                i = Tabwidth;
                while (i <= d)
                    i += Tabwidth;
                write(1, "                     ", i-d);
                d = i;
                break;
            case KEY_CR:
                clr_eol();
                return;
            default:
                write(1, "^", 1);
                c |= 0x40;
                write(1, &c, 1);
                d += 2;
                break;
            }
        }
        else {
            write(1, &c, 1);
            d++;
        }
    }
}

/*
 *  This routine draws the entire screen with lp as close to pos as possible.
 */

redraw(lp, lin, y)
char *lp;
int lin, y;
{
    register char *cp;

    Top_lp = lp;
    Top_y = y;
    Top_lin = lin;
    for (;;) {
        if (Top_y == SCR_TOP || Top_lp == Mem_buf)
            break;
        cp = get_prev(Top_lp);
        set_param(cp);
        if (Top_y - Lin_siz < SCR_TOP)
            break;
        Top_y -= Lin_siz;
        Top_lin--;
        Top_lp = cp;
    }
    Top_y = SCR_TOP;
    draw(Top_lp, Top_lin, Top_y);
}

draw(lp, lin, y)
char *lp;
int lin;
register int y;
{
    Bot_lin = lin;
    Bot_lp = lp;
    for (;;) {
        if (y >= SCR_BOT)
            break;
        if (Bot_lp >= End_buf)
            draw_lin("~\r", 0, y++);
        else {
            set_param(Bot_lp);
            if (y + Lin_siz > SCR_BOT)
                draw_lin("@\r", 0, y++);
            else {
                draw_lin(Bot_lp, 0, y);
                if (Cur_lp == Bot_lp) {
                    Cur_y = y;
                    Cur_lin = Bot_lin;
                }
                Bot_y = y += Lin_siz;
                Bot_lp = get_next(Bot_lp);
                ++Bot_lin;
            }
        }
    }
}

/*
 *  This routine determines the size of a line.
 */

set_param(lp)
char *lp;
{
    register int c, d;

    d = 0;
    for (;;) {
        if (Cur_ptr == lp)
            Cur_x = d;
        c = *lp++;
        if (c < KEY_SP) {
            switch(c) {
            case KEY_TAB:
                c = Tabwidth;
                while (c <= d)
                    c += Tabwidth;
                d = c;
                break;
            case KEY_CR:
                d++;
                Lin_siz = d / Lwidth + 1;
                return;
            default:
                d += 2;
                break;
            }
        }
        else {
            d++;
        }
    }
}

/*
 *  This routine finds the next line.
 */

char *
get_next(lp)
register char *lp;
{
    *End_buf = 0;
    lp = index(lp, KEY_CR);
    if (lp == 0)
        lp = End_buf;
    else
        ++lp;
    return(lp);
}

/*
 *  This routine finds the previous line.
 */

char *
get_prev(lp)
register char *lp;
{
    lp--;
    while (*--lp != KEY_CR && lp >= Mem_buf)
        ;
    return(lp+1);
}

/*
 *  This routine scrolls up and sets up everything appropriately.
 */

scrl_up()
{
    register int want;

    set_param(Bot_lp);
    want = Lin_siz - (SCR_BOT - Bot_y);
    if (want < 0)
        want = 0;
    up_scrl(want);
    draw(Bot_lp, Bot_lin, Bot_y - want);
    Top_y -= want;
    while (Top_y < SCR_TOP) {
        set_param(Top_lp);
        Top_y += Lin_siz;
        Top_lp = get_next(Top_lp);
        Top_lin++;
    }
}

/*
 *  This routine scrolls the top line onto the screen.
 */

scrl_dn()
{
    while (Cur_y < SCR_TOP) {
        ++Cur_y;
        dn_scrl();
        if (++Bot_y > SCR_BOT) {
            Bot_lp = get_prev(Bot_lp);
            --Bot_lin;
            set_param(Bot_lp);
            Bot_y -= Lin_siz;
            if (Bot_y < SCR_BOT)
                draw(Bot_lp, Bot_lin, Bot_y);
        }
    }
    Top_lp = Cur_lp;
    Top_y = SCR_TOP;
    Top_lin = Cur_lin;
    draw_lin(Cur_lp, 0, SCR_TOP);
}
