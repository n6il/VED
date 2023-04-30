/*  Copyright (c) 1982, 1986 by Manx Software Systems   */
/*  Copyright (c) 1982, 1986 by Jim Goodnow II      */

#include    "ved.h"

insert()
{
    char *ebuf;
    register int i, c;
    register int max, siz, len;
    register int left;
    char buf[800];
    int inscnt;
    char *Start_ptr;

    Insert = 1;
    Cur_ptr = buf + (Cur_ptr - Cur_lp);
    len = get_next(Cur_lp) - Cur_lp;
    left = (Mem_buf + Max_mem) - End_buf;
    ebuf = buf + len;
    blockmv(buf, Cur_lp, len);
    fix_msg();
    set_param(buf);
    max = siz = Lin_siz;
    mv_curs(Cur_x, Cur_y);
    for (;;) {
        inscnt = 1;
        Start_ptr = Cur_ptr;
        c = getchar();
        switch(c) {
        case KEY_ESC:      /* Escape character */
            t_insert(Cur_lp, len, buf, ebuf-buf);
            Cur_ptr = Cur_lp + (Cur_ptr - buf);
            Bot_lp += (ebuf - buf) - len;
            Insert = 0;
            fix_msg();
            set_param(Cur_lp);
            if (siz != Lin_siz || max != Lin_siz)
                draw(Cur_lp, Cur_lin, Cur_y);
            return;
        case KEY_BS:
            if (Cur_ptr == buf)
                break;
            blockmv(Cur_ptr-1, Cur_ptr, ebuf-Cur_ptr);
            --Cur_ptr;
            --ebuf;
            ++left;
            set_param(buf);
            draw_lin(Cur_ptr, Cur_x, Cur_y);
            break;
        /* case '.': */
        /*    c = getchar();    */
        case KEY_TAB:
            inscnt = Tabwidth - (Cur_x % Tabwidth);
            goto skip;
        default:
            if (c < KEY_SP && c != KEY_CR) {
                beep();
                break;
            }
skip:
            if (left < inscnt) {
                beep();
                mesg("no more room!");
                break;
            }
            blockmv(Cur_ptr + inscnt, Cur_ptr, ebuf-Cur_ptr);
            ebuf = ebuf + inscnt;
            left = left - inscnt;

            if (c == KEY_TAB)
            {
               while (inscnt--)
                   *Cur_ptr++ = KEY_SP;
               Cur_ptr--;
            }
            else
                *Cur_ptr = c;

            if (c == KEY_CR) {
                t_insert(Cur_lp, len, buf, ebuf-buf);
                draw(Cur_lp, Cur_lin, Cur_y);
                set_param(Cur_lp);
                Cur_y += Lin_siz;
                Cur_lp = get_next(Cur_lp);
                Cur_lin++;
                if (Cur_lin == Bot_lin)
                    scrl_up();
                len = get_next(Cur_lp) - Cur_lp;
                left = (Mem_buf + Max_mem) - End_buf;
                ebuf = buf + len;
                blockmv(buf, Cur_lp, len);
                set_param(buf);
                max = siz = Lin_siz;
                Cur_ptr = buf;
            }
            else {
                if (Cur_lin == Bot_lin - 1) {
                    set_param(buf);
                    if (Cur_y + Lin_siz > SCR_BOT) {
                        up_scrl(1);
                        --Cur_y;
                        if (--Top_y < SCR_TOP) {
                            set_param(Top_lp);
                            Top_y += Lin_siz;
                            Top_lp = get_next(Top_lp);
                            ++Top_lin;
                        }
                    }
                }
                draw_lin(Start_ptr, Cur_x, Cur_y);
                Cur_ptr++;
            }
            break;
        }
        set_param(buf);
        if (max < Lin_siz)
            max = Lin_siz;
        mv_curs(Cur_x, Cur_y);
    }
}

delete()
{
    register int c, l;

    l = Lin_siz;
    if (Cur_ptr == End_buf - 1)
        return;
    c = index(Cur_ptr, KEY_CR) - Cur_ptr;
    if (Num > c)
        Num = c;
    if (Num == 0)
        Num = 1;
    c = *Cur_ptr;
    t_insert(Cur_ptr, Num, Cur_ptr, 0);
    Bot_lp -= Num;
    set_param(Cur_lp);
    if (c == KEY_CR || l != Lin_siz)
        draw(Cur_lp, Cur_lin, Cur_y);
    else
        draw_lin(Cur_ptr, Cur_x, Cur_y);
}

lin_del()
{
    register char *t;

    if (Cur_lp == Mem_buf && get_next(Cur_lp) == End_buf){
        End_buf = Mem_buf;
        t_insert(Mem_buf,0,KEY_CR,1);
        Cur_ptr=Cur_lp;
        set_param(Cur_lp);
        draw_lin(Cur_lp,0,1);
        return;
    }
    t = Cur_lp;
    while (Num-- && (t = get_next(t)) != End_buf)
        ;
    t_insert(Cur_lp, t-Cur_lp, 0, 0);
    if (Cur_lp == End_buf) {
        if (Cur_lp == Mem_buf){
            t_insert(Mem_buf,0,KEY_CR,1);
            Bot_lin = 2;
            Bot_lp = End_buf;
            Cur_ptr = Cur_lp;
            set_param(Cur_lp);
            Cur_y=1;
            draw_lin(Cur_lp,0,1);
        } else {
            Bot_lin = Cur_lin--;
            Bot_lp = End_buf;
            Cur_lp = get_prev(Cur_lp);
            Cur_ptr = Cur_lp;
            set_param(Cur_lp);
            Cur_y -= Lin_siz;
        }
        if (Cur_y < Top_y)
            redraw(Cur_lp, Cur_lin, SCR_TOP);
        else {
            while (--Bot_y >= Cur_y + Lin_siz)
                draw_lin("~\r", 0, Bot_y);
            ++Bot_y;
        }
    } else {
        Cur_ptr = Cur_lp;
        draw(Cur_lp, Cur_lin, Cur_y);
    }
}

yank()
{
    register char *lp;
    register int i;

    i = Num;
    lp = Cur_lp;
    while (i-- && lp != End_buf)
        lp = get_next(lp);
    if (lp - Cur_lp > 1023) {
        mesg("yank too big");
        return;
    }
    blockmv(Ybuf, Cur_lp, Ylen = lp - Cur_lp);
}

put()
{
    if (t_insert(get_next(Cur_lp), 0, Ybuf, Ylen))
        mesg("yank buffer won't fit");
    else
        draw(Cur_lp, Cur_lin, Cur_y);
}

fix_msg()
{
    if (Insert)
        mesg("  <INSERT>");
    else
        mesg("");
}
