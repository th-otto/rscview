#include "aes.h"


ACCPD *gl_pacc[MAX_ACCS];		/* total of 6 desk acc, 1 from rom  */
_WORD gl_naccs;
char *gl_adacc[MAX_ACCS];		/* addresses of accessories */
char *sys_adacc;
char const stacc[] = "\\*.ACC";
_WORD used_acc;							/* currently number of acc  */
