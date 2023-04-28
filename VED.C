/*	Copyright (c) 1982 by Manx Software Systems	*/
/*	Copyright (c) 1982 by Jim Goodnow II		*/

#include	"ved.h"

main(argc, argv)
int argc;
char **argv;
{
	register int c, l;
	register char *t;

	init();
	End_buf = Mem_buf;
	Cur_ptr = Cur_lp = Mem_buf;
	*Fil_nam = 0;
	while (--argc) {
		if (*(*++argv) == '-') {
			switch ((*argv)[1]) {
			case 't':
				Tabwidth = atoi(*argv+2);
				break;
			}
		}
		else {
			strcpy(Fil_nam, argv[0]);
			break;
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
		case '':
			while (Num-- && Cur_ptr > Cur_lp)
				Cur_ptr--;
			break;
		case ' ':
		case '\t':
			while (Num-- && *Cur_ptr != '\r')
				Cur_ptr++;
			break;
		case '-':
			if (Cur_lp == Mem_buf)
				break;
			Cur_ptr = Cur_lp = get_prev(Cur_lp);
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
		case '\r':
			if (get_next(Cur_ptr) != End_buf) {
				Cur_ptr = Cur_lp = get_next(Cur_ptr);
				Cur_lin++;
				Cur_y += Lin_siz;
				if (Cur_lin == Bot_lin)
					scrl_up();
			}
			break;
		case '0':
			Cur_ptr = Cur_lp;
			break;
		case '$':
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

