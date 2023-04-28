/*                                                                      */
/*      dynamic memory handling routines                                */
/*              malloc() - allocate dynamic memory                      */
/*              free() - deallocate dynamic memory                      */
/*                                                                      */
/*                                                                      */
/*      alloc - allocate dynamic memory                                 */
/*                                                                      */
/*              call:                                                   */
/*                      malloc(nbytes)  nbytes is the number of bytes   */
/*                                      desired                         */
/*              returns:                                                */
/*                      a pointer to a memory area at least nbytes in   */
/*                      size or NULL if no memory is availiable         */
/*                                                                      */

#include <stdio.h>

typedef char ALIGN;  /* most stringent storage allignment for 6809   */

union header            /* free block header                            */
{
    struct
    {
        union header *ptr;      /* next free block              */
        unsigned size;          /* size of this block           */
    } s;
    ALIGN x;                        /* force allignment             */
};

typedef union header HEADER;

static HEADER base;             /* empty list to get started            */
static HEADER *allocp;          /* = NULL; last allocated block         */

char *malloc(nbytes)
unsigned nbytes;
{
    register HEADER *p, *q, *tp;
    register int nunits;

    nunits=1+(nbytes+(sizeof(HEADER)-1))/sizeof(HEADER);

    if((q=allocp) == NULL)
    {
        /* no free list yet                                     */

        base.s.ptr=allocp=q= &base;
        base.s.size=0;
    }

    for(p=q->s.ptr; ; q=p, p=p->s.ptr)
    {
        if(p->s.size >= nunits)
        {
            /* found a big enough area                      */
            /* check for exact fit                          */

            if(p->s.size == nunits)
                q->s.ptr=p->s.ptr;      /* is exact     */
            else
            {
                /* allocate tail end of this free area  */

                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            allocp=q;
            return ((char *) (p+1));
        }
        if(p == allocp)
        {
            /* looked through complete free list, ask the   */
            /* system for more memory                       */

            if((tp=sbrk(nunits * sizeof(HEADER))) == -1)
                return NULL;    /* no more memory       */

            /* make the returned block look like a block    */
            /* in the free list                             */

            tp->s.size=nunits;
            free(tp+1);
            p=allocp;
        }
    }
}

char *alloc(size)
{
    return malloc(size);
}

/*                                                                      */
/*      free - free a memory block allocated by alloc.                  */
/*                                                                      */
/*              call:                                                   */
/*                      free(ap)        ap is the memory block given    */
/*                                      by alloc                        */
/*      WARNING - this routine must only be used to free memory that    */
/*      has been allocated by malloc().                                 */
/*                                                                      */

free(ap)
char *ap;
{
    register HEADER *p,*q;

    p=(HEADER *)ap-1;                       /* point to header      */
    for(q=allocp; !(p > q && p < q->s.ptr); q=q->s.ptr)
        if(q >= q->s.ptr && (p > q || p < q->s.ptr))
            break;  /* at one end or the other              */

    if(p+p->s.size == q->s.ptr)
    {
        /* join this block to the upper free memory block       */

        p->s.size += q->s.ptr->s.size;
        p->s.ptr = q->s.ptr->s.ptr;
    }
    else
        p->s.ptr=q->s.ptr;

    if(q+q->s.size == p)
    {
        /* join to lower free memory block                      */

        q->s.size += p->s.size;
        q->s.ptr = p->s.ptr;
    }
    else
        q->s.ptr=p;

    allocp=q;
}

/*                                                                      */
/*      movmem - move a vlock of memory <length> bytes long from <from> */
/*      to <to>                                                         */
/*      movmem correctly moves memory blocks even when memory blocks    */
/*      overlap.                                                        */
/*                                                                      */

movmem(from,to,length)
char    *from,*to;
unsigned length;
{
    if(from > to)
    {
        for( ; length; --length)
            *to++= *from++;
    }
    else if(from < to)
    {
        /* move down from the top                               */

        from += length;
        to += length;

        for( ; length; --length)
            *--to= *--from;
    }
}
