/*	Copyright (c) 1982 by Manx Software Systems	*/
/*	Copyright (c) 1982 by Jim Goodnow II		*/

#include	"ved.h"

read_fil(name)
char *name;
{
	register int fd;
	register char *t, *tmp;
	char msg[60];
	register int cnt;
	char buf[257];

	maknam(msg, name);
	mesg(msg);
	tmp = Cur_ptr;
	if ((fd = open(name, 0)) < 0) {
		mesg("can't open file.");
		return(-1);
	}
	for (;;) {
		if ((cnt = read(fd, buf, 256)) <= 0)
			break;
		buf[cnt] = 0;
		t = buf;
		while (t = index(t, '\n'))
			*t++ = '\r';
		if (t_insert(tmp, 0, buf, cnt)) {
			mesg("file too big");
			break;
		}
		tmp += cnt;
		if (cnt != 256)
			break;
	}
	close(fd);
	return(0);
}

writ_fil(name)
char *name;
{
	register int fd;
	register char *tmp;
	register int cnt;
	char msg[70];

	maknam(msg, name);
	mesg(msg);
	tmp = Mem_buf;
	if ((fd = creat(name, 0)) < 0) {
		mesg("can't open file.");
		return(-1);
	}
	cnt = write(fd, tmp, End_buf - tmp);
	close(fd);
	if (cnt != End_buf - tmp) {
		mesg("error writing file!");
		return(-1);
	}
	return(0);
}

t_insert(t, len, buf, cnt)
char *t, *buf;
register int len, cnt;
{
	if (End_buf + (cnt - len) > Mem_buf + Max_mem)
		return(-1);
	blockmv(t+cnt, t+len, End_buf-t-len);
	End_buf += cnt - len;
	blockmv(t, buf, cnt);
	Modflg = 1;
	return(0);
}

maknam(buf, nam)
char *buf, *nam;
{
	*buf++ = '"';
	strcpy(buf, nam);
	strcat(buf, "\" ");
}

