/*
 * resource set indices for fo_al
 *
 * created by ORCS 2.16
 */

/*
 * Number of Strings:        45
 * Number of Bitblks:        0
 * Number of Iconblks:       12
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       5
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        38
 * Number of Trees:          3
 * Number of Userblks:       0
 * Number of Images:         24
 * Total file size:          5016
 */

#undef RSC_NAME
#ifndef __ALCYON__
#define RSC_NAME "fo_al"
#endif
#undef RSC_ID
#ifdef fo_al
#define RSC_ID fo_al
#else
#define RSC_ID 0
#endif

#ifndef RSC_STATIC_FILE
# define RSC_STATIC_FILE 0
#endif
#if !RSC_STATIC_FILE
#define NUM_STRINGS 45
#define NUM_FRSTR 0
#define NUM_UD 0
#define NUM_IMAGES 24
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 12
#define NUM_CIB 0
#define NUM_TI 5
#define NUM_OBS 38
#define NUM_TREE 3
#endif



#define F_ALERT                            0 /* form/dialog */
#define FA_TITLE                           1 /* BUTTON in tree F_ALERT */
#define FA_TEXT                            2 /* TEXT in tree F_ALERT */ /* max len 1 */
#define FA_BUTTON                          7 /* BUTTON in tree F_ALERT */

#define ICONS                              1 /* form/dialog */
#define IC_ICON_ATTENTION                  1 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_QUESTION                   2 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_STOP                       3 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_INFO                       4 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_DISK                       5 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_ERROR                      6 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_QUESTION2                  7 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_HALT                       8 /* ICON in tree ICONS */ /* max len 0 */
#define IC_ICON_RUBBISH                    9 /* ICON in tree ICONS */ /* max len 0 */
#define IC_TEXT_TIP                       10 /* STRING in tree ICONS */
#define IC_TEXT_ATTENTION                 11 /* STRING in tree ICONS */
#define IC_TEXT_QUESTION                  12 /* STRING in tree ICONS */
#define IC_TEXT_STOP                      13 /* STRING in tree ICONS */
#define IC_TEXT_INFO                      14 /* STRING in tree ICONS */
#define IC_TEXT_DISK                      15 /* STRING in tree ICONS */
#define IC_TEXT_ERROR                     16 /* STRING in tree ICONS */
#define IC_TEXT_QUESTION2                 17 /* STRING in tree ICONS */
#define IC_TEXT_HALT                      18 /* STRING in tree ICONS */
#define IC_TEXT_RUBBISH                   19 /* STRING in tree ICONS */

#define ICONS2                             2 /* form/dialog */




#ifdef __STDC__
#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif
extern _WORD fo_al_rsc_load(_WORD wchar, _WORD hchar);
extern _WORD fo_al_rsc_gaddr(_WORD type, _WORD idx, void *gaddr);
extern _WORD fo_al_rsc_free(void);
#endif
