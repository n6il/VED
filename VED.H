#define SCR_TOP 1
#define SCR_BOT 24
#define STATUS  0

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

char *get_next(), *get_prev(), *index();
