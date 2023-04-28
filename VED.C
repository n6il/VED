/*  Copyright (c) 1982, 1986 by Manx Software Systems   */
/*  Copyright (c) 1982, 1986 by Jim Goodnow II      */

#include "ved.h"

int errflag=0;
int errline, errcol;
char errmsg[90];

int main(argc, argv)
int argc;
char **argv;
{
    register int c, l, i;
    register char *t;

    Rscrl = 1;

    init();
    scr_init();
    End_buf = Mem_buf;
    Cur_ptr = Cur_lp = Mem_buf;
    *Fil_nam = 0;
    while (--argc) {

        Tabwidth = 4; /* set deafult tab width to 4 */

        if (*(*++argv) == '-') {
            switch ((*argv)[1]) {
            case 't':
                Tabwidth = atoi(*argv+2);
                break;
            }
        }
        else {
            strcpy(Fil_nam, argv[0]);
        }
    }
    if (*Fil_nam == 0 || read_fil(Fil_nam) < 0)
        t_insert(Cur_ptr, 0, "\r", 1);

    Modflg = 0;
    redraw(Mem_buf, 1, SCR_TOP);
    stats();

    for (;;) {

        set_param(Cur_lp);
        mv_curs(Cur_x, Cur_y);
        c = getchar();
        if (c != '0')
            c = get_num(c);
        switch (c) {
#ifdef DEBUG
        case '=':
            debug();
            break;
#endif
        case 'r':
            if (*Cur_ptr == '\r' || (c = getchar()) == '\r') {
                beep();
                break;
            }
            l = Lin_siz;
            *Cur_ptr = c;
            Modflg = 1;
            set_param(Cur_lp);
            if (l != Lin_siz)
                draw(Cur_lp, Cur_lin, Cur_y);
            else
                draw_lin(Cur_ptr, Cur_x, Cur_y);
            break;
        case 'z':
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            break;
        case 'y':
            c = getchar();
            if (c == 'y' || c == 'd')
                yank();
            if (c == 'd')
                lin_del();
            break;
        case 'p':
            put();
            break;
        case 'n':
            search(1);
            break;
        case '/':
            search(0);
            break;
        case '.':
        case 0x13:
            while (Num-- && Cur_ptr > Cur_lp)
                Cur_ptr--;
            break;
        case ' ':
        case '\t':
        case 0x06:
            while (Num-- && *Cur_ptr != '\r')
                Cur_ptr++;
            break;
        case 0x12:      /* Page Up */
            Num = Cur_lin - 23 + Cur_x;
            if (Num < 1)
                Num = 1;
            Cur_lp = Mem_buf;
            Cur_lin = 1;
            while (--Num && (t = get_next(Cur_lp)) != End_buf) {
                Cur_lp = t;
                Cur_lin++;
            }
            Cur_ptr = Cur_lp;
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            break;
        case 0x05: /*up arrow */
            l=Cur_ptr-Cur_lp;
            t=Cur_lp;
        case '-':
            if (Cur_lp == Mem_buf)
                break;
            Cur_ptr = Cur_lp = get_prev(Cur_lp);
            if (c==0xb)
                Cur_ptr += (l<(i=t-Cur_lp-1)?l:i);
            set_param(Cur_lp);
            Cur_y -= Lin_siz;
            Cur_lin--;
            if (Cur_lin < Top_lin) {
                if (Rscrl == 0)
                    scrl_dn();
                else
                    redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            }
            break;
        case 0x16:      /* Page Down */
            Num = Cur_lin + 23 + Cur_x;
            Cur_lp = Mem_buf;
            Cur_lin = 1;
            while (--Num && (t = get_next(Cur_lp)) != End_buf) {
                Cur_lp = t;
                Cur_lin++;
            }
            Cur_ptr = Cur_lp;
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            break;
        case 0x3: /* down arrow */
            l=Cur_ptr-Cur_lp;
        case 0x0d:
            if (get_next(Cur_ptr) != End_buf) {
                Cur_ptr = Cur_lp = get_next(Cur_ptr);
                if (c==0xa)
                    Cur_ptr += (l<(i=get_next(Cur_lp)-Cur_lp-1)?l:i);
                Cur_lin++;
                Cur_y += Lin_siz;
                if (Cur_lin == Bot_lin)
                    scrl_up();
            }
            break;
        case 0x01:
            Cur_ptr = Cur_lp;
            break;
        case 0x07:
            Cur_ptr = get_next(Cur_ptr) - 1;
            break;
        case 'h':
            Cur_lp = Cur_ptr = Top_lp;
            Cur_y = Top_y;
            Cur_lin = Top_lin;
            break;
        case 'm':
            Cur_lp = Top_lp;
            Cur_y = Top_y;
            Cur_lin = Top_lin;
            while (Cur_y < SCR_BOT/2) {
                set_param(Cur_lp);
                Cur_y += Lin_siz;
                Cur_lp = get_next(Cur_lp);
                ++Cur_lin;
            }
            Cur_ptr = Cur_lp;
            break;
        case 'l':
            Cur_lp = Cur_ptr = get_prev(Bot_lp);
            set_param(Cur_lp);
            Cur_y = Bot_y - Lin_siz;
            Cur_lin = Bot_lin - 1;
            break;
        case 'g':
            if (Val == 0)
                Num = -1;
            Cur_lp = Mem_buf;
            Cur_lin = 1;
            while (--Num && (t = get_next(Cur_lp)) != End_buf) {
                Cur_lp = t;
                Cur_lin++;
            }
            Cur_ptr = Cur_lp;
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            break;
        case ':':
            colon();
            break;
        case 'x':
            delete();
            break;
        case 'd':
            if (getchar() == 'd')
                lin_del();
            break;
        case 'o':
            set_param(Cur_lp);
            Cur_lp = Cur_ptr = get_next(Cur_ptr);
            Cur_y += Lin_siz;
            ++Cur_lin;
            t_insert(Cur_lp, 0, "\r", 1);
            if (Cur_lp == Bot_lp)
                scrl_up();
            else
                draw(Cur_lp, Cur_lin, Cur_y);
        case 'i':
            insert();
            break;
        case '?':
            help();
            break;
        default:
            mesg("hit '?' for a list of commands");
            break;
        }
    }
}

#ifdef DEBUG
debug()
{
    mesg("b,c,t");
    switch (getchar()) {
    case 'b':
        sprintf(Ybuf, "bot:lp=%d lin=%d y=%d",
                            Bot_lp-Mem_buf, Bot_lin, Bot_y);
        mesg(Ybuf);
        break;
    case 'c':
        sprintf(Ybuf, "cur:lp=%d lin=%d y=%d",
                            Cur_lp-Mem_buf, Cur_lin, Cur_y);
        mesg(Ybuf);
        break;
    case 't':
        sprintf(Ybuf, "top:lp=%d lin=%d y=%d",
                            Top_lp-Mem_buf, Top_lin, Top_y);
        mesg(Ybuf);
        break;
    }
    db();
}
#asm
    public  db_
db_ lda #0
    rts
#endasm
#endif
