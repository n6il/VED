/*	Copyright (c) 1982, 1983 by Manx Software Systems	*/
/*	Copyright (c) 1982, 1983 by Jim Goodnow II		*/

#include	"ved.h"
#include	"kbctl.h"

init()
{
	Tabwidth = ioctl(1, KB_TAB, 0);
	Max_mem = 30000;
	while ((Mem_buf = malloc(Max_mem)) == 0)
		Max_mem -= 2000;
	free(Mem_buf);
	Max_mem -= 1024;
	Mem_buf = malloc(Max_mem);
	Width = ioctl(1, KB_WID);
	Lwidth = Width + 1;
	clr_scrn();
	ioctl(1, KB_ECHO, 0);
	ioctl(1, KB_CRLF, 0);
	ioctl(1, KB_INTR, 0);
	ioctl(1, KB_EOF, 0);
	Rscrl = dn_scrl();
}

quit()
{
	mv_curs(0, 23);
	write(1, "\n\r", 2);
	exit(0);
}

/*
 *	Move the cursor to the appropriate position.
 */

mv_curs(x, y)
register int x, y;
{
	char buf[4];

	if (x >= Width) {
		y += x / Width;
		x = x % Width;
	}
	ioctl(1, KB_CURS,x | (y<<8));
}

/*
 *	This routine beeps.
 */

beep()
{
	write(1, "\007", 1);
}

/*
 *	This routine puts a message on the status line.
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
		write(1,"\n", 1);
	fix_msg();
}

getchar()
{
	int c;

	read(0, &c, 1);
	return(c&0xff);
}

/*
 *	This routine clears from the current cursor position to the end of the line.
 */

clr_eol()
{
	ioctl(1, KB_CLEOL);
}

clr_scrn()
{
	ioctl(1, KB_CLEAR);
}

dn_scrl()
{
	return(ioctl(1, KB_RSCRL));
}

