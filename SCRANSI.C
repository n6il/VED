/* 
*
*  Screen functions for the ansi terminal
*
*/

char bep[1];
char eol[3];
char clr[4];
char hom[3];
char ins[3];
char dell[3];

scr_init ()
{
   eol[0] = clr[0] = ins[0] = hom[0] = dell[0] = 0x1b;
   eol[1] = clr[1] = ins[1] = hom[1] = dell[1] = 0x5b;

   eol[2] = 'K';  /* Erase to EOL */

   clr[2] = '2';  /* Clear screen & pos curser at 1,1 */
   clr[3] = 'J';  /* Clear screen & pos curser at 1,1 */

   hom[2] = 'H';  /* Cursor Home */

   ins[2] = 'L'; /* Insert Line */

   dell[2] = 'M'; /* Delete Line */

   bep[0] = 0x07;
}

scr_curs (y, x) int y, x;
{
   static char str[20];
   x++; y++;
   sprintf(str,"\033[%03d;%03dH",y,x);
   write (1, str, strlen(str));
}

scr_beep ()
{
   write (1, bep, sizeof(bep));
}

scr_eol ()
{
   write (1, eol, sizeof(eol));
}

scr_clear ()
{
   write (1, clr, sizeof(clr));
}

scr_linsert ()
{
    write (1, ins, sizeof(ins));
}

scr_lindel()
{
    write (1, dell, sizeof(dell));
}

