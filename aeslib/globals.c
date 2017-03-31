#include "gem_aesP.h"

/*
 *  global data related to AES stuff
 */
 
/** global aes variable: Application ID initialized by appl_init */
short gl_apid;

/** global aes variable: AES version initialized by appl_init */
short gl_ap_version;

/** global aes variable: aes global array, used by all 
    "old style" aes functions (function without "mt_")
 */
short aes_global[AES_GLOBMAX];
