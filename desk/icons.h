/*
 * resource set indices for icons
 *
 * created by ORCS 2.14
 */

/*
 * Number of Strings:        41
 * Number of Bitblks:        0
 * Number of Iconblks:       41
 * Number of Color Iconblks: 0
 * Number of Color Icons:    0
 * Number of Tedinfos:       0
 * Number of Free Strings:   0
 * Number of Free Images:    0
 * Number of Objects:        43
 * Number of Trees:          2
 * Number of Userblks:       0
 * Number of Images:         82
 * Total file size:          13402
 */


#define BASIC      0 /* form/dialog */

#define APPS       1 /* form/dialog */
#define ICON_BEG   1 /* ICON in tree APPS */ /* max len 12 */
#define ICON_END  33 /* ICON in tree APPS */ /* max len 12 */



#ifndef TARGET_192
#define RS_NOBS 43c
#define RS_NTREE 2c
#define RS_NTED 0c
#define RS_NIB 41c
#define RS_NBB 0c
#else
#define RS_NOBS 43
#define RS_NTREE 2
#define RS_NTED 0
#define RS_NIB 41
#define RS_NBB 0
#endif


/* The following arrays live in RAM */
extern OBJECT  desk_rs_obj[];
extern TEDINFO desk_rs_tedinfo[];

/* This array lives in ROM and points to RAM data */
extern OBJECT * const desk_rs_trees[];

/* The following resource data live in ROM */
extern const char * const desk_rs_fstr[];
extern const BITBLK       desk_rs_bitblk[];


extern void icons_rsc_init(void);
