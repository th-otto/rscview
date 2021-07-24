/*
 * resource set indices for gem
 *
 * created by ORCS 2.18
 */

/*
 * Number of Strings:        77
 * Number of Bitblks:        14
 * Number of Iconblks:       0
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       13
 * Number of Free Strings:   25
 * Number of Free Images:    14
 * Number of Objects:        66
 * Number of Trees:          3
 * Number of Userblks:       0
 * Number of Images:         14
 * Total file size:          5478
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "gem"
#endif
#undef RSC_ID
#ifdef gem
#define RSC_ID gem
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 77
#define NUM_FRSTR 25
#define NUM_UD 0
#define NUM_IMAGES 14
#define NUM_BB 14
#define NUM_FRIMG 14
#define NUM_IB 0
#define NUM_CIB 0
#define NUM_TI 13
#define NUM_OBS 66
#define NUM_TREE 3
#endif



#define FSELECTR                           0 /* form/dialog */
#define FSTITLE                            1 /* STRING in tree FSELECTR */
#define FSDIRECT                           3 /* FBOXTEXT in tree FSELECTR */
#define FSDRVTXT                           4 /* STRING in tree FSELECTR */
#define FSSELECT                           5 /* FBOXTEXT in tree FSELECTR */
#define FSDRIVES                           6 /* IBOX in tree FSELECTR */
#define FS1STDRV                           7 /* BOXCHAR in tree FSELECTR */
#define FSLSTDRV                          32 /* BOXCHAR in tree FSELECTR */
#define FILEAREA                          33 /* IBOX in tree FSELECTR */
#define FCLSBOX                           34 /* BOXCHAR in tree FSELECTR */
#define FTITLE                            35 /* BOXTEXT in tree FSELECTR */
#define SCRLBAR                           36 /* BOX in tree FSELECTR */
#define FUPAROW                           37 /* BOXCHAR in tree FSELECTR */
#define FDNAROW                           38 /* BOXCHAR in tree FSELECTR */
#define FSVSLID                           39 /* BOX in tree FSELECTR */
#define FSVELEV                           40 /* BOX in tree FSELECTR */
#define FILEBOX                           41 /* BOX in tree FSELECTR */
#define F1NAME                            42 /* FBOXTEXT in tree FSELECTR */
#define F2NAME                            43 /* FBOXTEXT in tree FSELECTR */
#define F3NAME                            44 /* FBOXTEXT in tree FSELECTR */
#define F4NAME                            45 /* FBOXTEXT in tree FSELECTR */
#define F5NAME                            46 /* FBOXTEXT in tree FSELECTR */
#define F6NAME                            47 /* FBOXTEXT in tree FSELECTR */
#define F7NAME                            48 /* FBOXTEXT in tree FSELECTR */
#define F8NAME                            49 /* FBOXTEXT in tree FSELECTR */
#define F9NAME                            50 /* FBOXTEXT in tree FSELECTR */
#define FSOK                              51 /* BUTTON in tree FSELECTR */
#define FSCANCEL                          52 /* BUTTON in tree FSELECTR */

#define DIALERT                            1 /* form/dialog */
#define ALICON                             1 /* BOX in tree DIALERT */
#define MSGOFF                             2 /* STRING in tree DIALERT */
#define BUTOFF                             7 /* BUTTON in tree DIALERT */

#define DESKTOP                            2 /* form/dialog */
#define APPTITLE                           2 /* TEXT in tree DESKTOP */

#define AL00CRT                            0 /* Alert string */

#define AL01CRT                            1 /* Alert string */

#define AL02CRT                            2 /* Alert string */

#define AL03CRT                            3 /* Alert string */

#define AL04CRT                            4 /* Alert string */

#define AL05CRT                            5 /* Alert string */

#define AL18ERR                            6 /* Alert string */

#define AL04ERR                            7 /* Alert string */

#define AL05ERR                            8 /* Alert string */

#define AL15ERR                            9 /* Alert string */

#define AL08ERR                           10 /* Alert string */

#define ALXXERR                           11 /* Alert string */

#define ALNOFUNC                          12 /* Alert string */

#define ALRTDSWAP                         13 /* Alert string */

#define ITEMSLCT                          14 /* Free string */

#define ST9VAL                            15 /* Free string */

#define STAVAL                            16 /* Free string */

#define STNVAL                            17 /* Free string */

#define STPVAL                            18 /* Free string */

#define STLPVAL                           19 /* Free string */

#define STFVAL                            20 /* Free string */

#define STLFVAL                           21 /* Free string */

#define STLAVAL                           22 /* Free string */

#define STLNVAL                           23 /* Free string */

#define NOMEMORY                          24 /* Alert string */

#define NOTEBB                             0 /* Free image */

#define QUESTBB                            1 /* Free image */

#define STOPBB                             2 /* Free image */

#define NOTEBB_TOS                         3 /* Free image */

#define QUESTBB_TOS                        4 /* Free image */

#define STOPBB_TOS                         5 /* Free image */

/* ARROW */
#define MICE00                             6 /* Mouse cursor */

/* TEXT_CRSR */
#define MICE01                             7 /* Mouse cursor */

/* HOURGLASS */
#define MICE02                             8 /* Mouse cursor */

/* POINT_HAND */
#define MICE03                             9 /* Mouse cursor */

/* FLAT_HAND */
#define MICE04                            10 /* Mouse cursor */

/* THIN_CROSS */
#define MICE05                            11 /* Mouse cursor */

/* THICK_CROSS */
#define MICE06                            12 /* Mouse cursor */

/* OUTLN_CROSS */
#define MICE07                            13 /* Mouse cursor */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD gem_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD gem_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD gem_rsc_free(void);
#endif
