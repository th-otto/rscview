#include "aes.h"
#include "gem_rsc.h"
#include "gem_rsc.rsh"
#include "s_endian.h"

OBJECT **aes_rsc_tree = (OBJECT **)rs_trindex;
const char *const *aes_rsc_string = rs_frstr;
const BITBLK *const *aes_rsc_bitblk = rs_frimg;

_BOOL rsc_read(void)
{
    /* Copy data from ROM to RAM: */
	memcpy(rs_object, rs_object_rom, sizeof(rs_object));
	memcpy(rs_tedinfo, rs_tedinfo_rom, sizeof(rs_tedinfo));
	return TRUE;
}

void rsc_free(void)
{
}
