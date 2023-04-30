/*
* Define FLEX09 as OS
*/
#define FLEX09

#define SCR_TOP 1
#define SCR_BOT 24
#define SCR_WID 80
#define STATUS  0


/*
*
* KeyFunction defines.
*
*/

#define    PROGRAM_NAME        "VED2"
#define    VERSION            "1.06"    


#ifdef __linux  /* Overrides FLEX09 */
#define    KEY_BS            0x7f        /* DEL Key */
#else
#ifdef FLEX09
#define    KEY_BS            0x08        /* ^h or BS */
#else
/* Unknown OS */
#endif
#endif


#define    KEY_TAB            0x09        /* ^i or TAB */
#define    KEY_LF            0x0a        /* ^j or LF */
#define    KEY_CR            0x0d        /* ^m or CR */
#define    KEY_ESC            0x1b        /* ESC */
#define    KEY_SP            0x20        /* Space */

#define    KEY_PGUP        0x02        /* ^b - Scroll one page backwards/up */
#define    KEY_PGDN        0x06        /* ^f - Scroll one page forward/down */

#define KEY_MOV_PREVLIN        '-'        /* - Move to beginning of previous line */
#define KEY_MOV_NEXTLIN        KEY_CR        /* - Move to beginning of next line */

#define KEY_MOV_DN        'j'        /* - Move to one line down , keep x position if possible */
#define KEY_MOV_UP        'k'        /* - Move to one line down , keep x position if possible */

#define KEY_MOV_LEFT        'h'        /* - Move cursor left */
#define KEY_MOV_RIGHT        'l'        /* - Move cursor right */

#define KEY_MOV_BEGLIN        '0'        /* - Move cursor to the beginning of current line */
#define KEY_MOV_ENDLIN        '$'        /* - Move cursor to the end of current line */    

#define KEY_MOV_BEGLIN1        0x02        /* - Move cursor to the beginning of current line */    
#define KEY_MOV_ENDLIN1        0x05        /* - Move cursor to the end of current line */

#define KEY_MOV_TOP_SCR        'T'        /* - Move cursor to the top of the screen */
#define KEY_MOV_MID_SCR        'M'        /* - Move cursor to the middle of the screen */
#define KEY_MOV_BOT_SCR        'B'        /* - Move cursor to the bottom of the screen */


#define    KEY_REDRW        0x12        /* ^r - Redraw the screen */
#define    KEY_REDRW1        'z'        /* - Redraw the screen */
#define    KEY_YANK        'y'        /* - yank (copy) line(s) */
#define    KEY_PASTE        'p'        /* - paste (yanked) line(s) */
#define    KEY_DELLIN        'd'        /* - delete line(s) */    
#define    KEY_DELCH        'x'        /* - delete character on cursors position */
#define KEY_SRCH        '/'        /* - Search for pattern */
#define KEY_RPT_SRCH        'n'        /* - Repeat last search pattern */
#define KEY_REPL_CH        'r'        /* - replace character on cursors position */
#define KEY_GOTO        'g'        /* - goto line */
#define KEY_INS         'i'        /* - Switch to insert mode, on current cursor position */
#define KEY_INS_BELOW        'o'        /* - Insert new line, goto beginning of inserted line and switch to insert mode */





/*
*
* Declare vars
*
*/
extern int Max_mem;
extern char *Mem_buf;
extern char *End_buf;
extern int Top_lin, Bot_lin, Cur_lin;
extern char *Top_lp, *Bot_lp, *Cur_lp, *Cur_ptr;
extern int Bot_y, Top_y;
extern int Cur_y, Cur_x;
extern int Lin_siz;
extern int Width, Lwidth;
extern int Modflg;
extern int Insert;
extern int Rscrl;
extern char Fil_nam[80];
extern int Num, Val;
extern char Ybuf[1024];
extern int Ylen;
extern int Tabwidth;
extern int errflag;
extern int errline, errcol;
extern char errmsg[90];

/*
*
* Declare funcs
*
*/
char *get_next(), *get_prev(), *index(), *rindex();





