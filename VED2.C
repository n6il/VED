/*  Copyright (c) 1982, 1986 by Manx Software Systems   */
/*  Copyright (c) 1982, 1986 by Jim Goodnow II      */

#include "ved.h"

/* */

int Max_mem;
char *Mem_buf;
char *End_buf;
int Top_lin, Bot_lin, Cur_lin;
char *Top_lp, *Bot_lp, *Cur_lp, *Cur_ptr;
int Bot_y, Top_y;
int Cur_y, Cur_x;
int Lin_siz;
int Width, Lwidth;
int Modflg;
int Insert;
int Rscrl;
char Fil_nam[80];
int Num, Val;
char Ybuf[1024];
int Ylen;
int Tabwidth;

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

    scr_init();
    init();
    End_buf = Mem_buf;
    Cur_ptr = Cur_lp = Mem_buf;
    *Fil_nam = 0;

    Tabwidth = 4; /* set default tab width to 4 */

    /* Parse arguments */
    for(i=1;i<argc;i++)
    {
        t=argv[i];
        if (*t == '-') {
            if(*(t+1) == 't') {;
                Tabwidth = atoi((t+2));
            }
        }
        else {
            if(*t) /* Filename was present */
              strcpy(Fil_nam, t);
        }
    }

#ifdef __linux  /* Overrides FLEX09 */

#else
#ifdef FLEX09

    if(*Fil_nam) /* Filename was present */
    {
     /* Flex does NOT like files without an extension */
    /* If user specified filename doesn't have an extension, append .txt to the filename */

    if((t=rindex(Fil_nam,'.')) == (char *)0)  /* Search for last occurence */
    {/* name doesn't contain a '.' */
        strcat(Fil_nam,".txt");   /* Append .txt to the name */
    }
    else
    { /* We got a '.' in the name , check it there are minimum one char after the '.' */
      if(strlen(t) <= 1)
          {/* filename ends with a '.' , append txt to the name */
             strcat(Fil_nam,"txt");   /* Append txt to the name */
          }
    }
    }
#else
/* Unknown OS */
#endif
#endif

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
        case KEY_REPL_CH:
            if (*Cur_ptr == KEY_CR || (c = getchar()) == KEY_CR) {
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
        case KEY_REDRW:
        case KEY_REDRW1:
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            break;
        case KEY_YANK:
            c = getchar();
            if (c == KEY_YANK || c == KEY_DELLIN)
                yank();
            if (c == KEY_DELLIN)
                lin_del();
            break;
        case KEY_PASTE:
            put();
            break;
        case KEY_RPT_SRCH:
            search(1);
            break;
        case KEY_SRCH:
            search(0);
            break;
        case KEY_BS: /* backspace */
        case KEY_MOV_LEFT: /* backspace */
            while (Num-- && Cur_ptr > Cur_lp)
                Cur_ptr--;
            break;
        case KEY_SP:
    case KEY_MOV_RIGHT:
            while (Num-- && *Cur_ptr != KEY_CR)
                Cur_ptr++;
            break;
        case KEY_PGUP: /*  page up */
            Num = Cur_lin - (SCR_BOT-1) + Cur_x;
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
        case KEY_MOV_UP: /*up arrow */
            l=Cur_ptr-Cur_lp;
            t=Cur_lp;
        case KEY_MOV_PREVLIN:
            if (Cur_lp == Mem_buf)
                break;
            Cur_ptr = Cur_lp = get_prev(Cur_lp);

        if (c==KEY_MOV_UP) /* Move one line up, keep X position  */
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
        case KEY_PGDN: /* page down */
            Num = Cur_lin + (SCR_BOT-1) + Cur_x;
            Cur_lp = Mem_buf;
            Cur_lin = 1;
            while (--Num && (t = get_next(Cur_lp)) != End_buf) {
                Cur_lp = t;
                Cur_lin++;
            }
            Cur_ptr = Cur_lp;
            redraw(Cur_lp, Cur_lin, SCR_BOT/2);
            break;
        case KEY_MOV_DN: /* down arrow */
            l=Cur_ptr-Cur_lp;
        case KEY_CR:
            if (get_next(Cur_ptr) != End_buf) {
                Cur_ptr = Cur_lp = get_next(Cur_ptr);

                if (c==KEY_MOV_DN) /* Move one line down, keep X position  */
                    Cur_ptr += (l<(i=get_next(Cur_lp)-Cur_lp-1)?l:i);

                Cur_lin++;
                Cur_y += Lin_siz;
                if (Cur_lin == Bot_lin)
                    scrl_up();
            }
            break;
        case KEY_MOV_BEGLIN:
            Cur_ptr = Cur_lp;
            break;
        case KEY_MOV_ENDLIN:
            Cur_ptr = get_next(Cur_ptr) - 1;
            break;
        case KEY_MOV_TOP_SCR:
            Cur_lp = Cur_ptr = Top_lp;
            Cur_y = Top_y;
            Cur_lin = Top_lin;
            break;
        case KEY_MOV_MID_SCR:
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
        case KEY_MOV_BOT_SCR:
            Cur_lp = Cur_ptr = get_prev(Bot_lp);
            set_param(Cur_lp);
            Cur_y = Bot_y - Lin_siz;
            Cur_lin = Bot_lin - 1;
            break;
        case KEY_GOTO:
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
        case KEY_DELCH:
            delete();
            break;
        case KEY_DELLIN:
            if (getchar() == KEY_DELLIN)
                lin_del();
            break;
        case KEY_INS_BELOW:
            set_param(Cur_lp);
            Cur_lp = Cur_ptr = get_next(Cur_ptr);
            Cur_y += Lin_siz;
            ++Cur_lin;
            t_insert(Cur_lp, 0, "\r", 1);
            if (Cur_lp == Bot_lp)
                scrl_up();
            else
                draw(Cur_lp, Cur_lin, Cur_y);
        case KEY_INS:
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
