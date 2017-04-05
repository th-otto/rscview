#include "config.h"
#include <gemdos.h>


void dos_chrout(int c)
{
    putc(c, stdout);
}

void dos_conws(const char *str)
{
	while (*str)
	{
		if (*str != '\r')
			fputc(*str, stdout);
		str++;
	}
	fflush(stdout);
}


/* allocate in ST RAM only */
void *dos_alloc_stram(long nbytes)
{
    return malloc(nbytes);
}


/* get max size of available RAM in ST RAM only */
size_t dos_avail(void)
{
    return LONG_MAX;
}


/* allocate in Alt-RAM (e.g. TT RAM) if possible, otherwise ST RAM */
void *dos_alloc(long nbytes)
{
    return malloc(nbytes);
}


/* get max size of available RAM in TT RAM or ST RAM */
size_t dos_avail_anyram(void)
{
    return LONG_MAX;
}


void dos_free(void *maddr)
{
    free(maddr);
}


void dos_shrink(void *maddr, long length)
{
	void *newptr;
	
	newptr = realloc(maddr, length);
	if (newptr != maddr)
	{
		abort();
    }
}
