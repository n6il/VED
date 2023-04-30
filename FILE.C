/*  Copyright (c) 1982, 1986 by Manx Software Systems    */
/*  Copyright (c) 1982, 1986 by Jim Goodnow II           */

#include    "ved.h"

#ifdef __linux  /* Overrides FLEX09 */
#define MODE 0666
#else
#ifdef FLEX09
#define MODE 0x00  /* 0xC3 - file access mode for ProDOS */
                   /* 0x00 - text file for FLEX          */
#else
/* Unknown OS */
#endif
#endif


read_fil(name)
char *name;
{
    register int fd;
    register char *t, *tmp;
    char msg[60];
    register int cnt;
    static char buf[257];

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

        /* Convert LF to CR even on linux */
        /* Use Octal LF ,as '\n' will be xlated to '\r' on FLEX */
        while (t = index(t, '\012')) 
            *t++ = '\r';

        if (t_insert(tmp, 0, buf, cnt)) {
            strcpy(errmsg,"file too big");
            mesg(errmsg);
            errflag=1;
            close(fd);
            quit();
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
    if ((fd = creat(name, MODE)) < 0) {
        mesg("can't open file.");
        return(-1);
    }

#ifdef __linux  /* Overrides FLEX09 */

    /* Convert CR to LF on Linux */
    int size = (End_buf - tmp);

    for(cnt = 0; cnt < size; cnt++)
    {
       if(*tmp == 0x0d)
       {
          *tmp = 0x0a;
       }
       tmp++;
    }
    tmp = Mem_buf;
#else
#ifdef FLEX09
 
#else
/* Unknown OS */
#endif
#endif

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
