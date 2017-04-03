#include "aes.h"
#include "gem_rsc.h"
#include "gem_rsc.rsh"


OBJECT **aes_rsc_tree = (OBJECT **)rs_trindex;
const char *const *aes_rsc_string = rs_frstr;
const BITBLK *const *aes_rsc_bitblk = rs_frimg;

_BOOL rsc_read(void)
{
	return TRUE;
}
