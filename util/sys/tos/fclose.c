#include "config.h"
#include <gemdos.h>
#include <fcntl.h>
#include <errno.h>


#if defined(__PUREC__)
/*
 * Work around a bug in Pure-C library, not closing
 * the filehandle in case of error
 */
void purec_fclose(FILE *fp)
{
	if (ffp != stdout && (ffp->Flags & 3))
	{
		close(ffp->Handle);
		if (ffp->Flags & 0x08)
			free(ffp->BufStart);
		ffp->Flags = 0;
	}
}
#endif
