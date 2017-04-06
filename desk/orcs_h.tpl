/*
 * Template file for C include files
 * 
 * ORCS version 2.14 08.02.2015
 * 
 * %N will be replaced by the RSC filename (without path)
 * %n will be replaced by the RSC filename
 *    (in lowercase and without .rsc extension)
 * %R will be replaced by the RSC filename
 *    (according to namerules and without .rsc extension)
 * %h will be replaced by the extension of include files
 * %d marks the place where the actual data is written
 * %D will output the definitions of the object names
 * 
 * %s: Number of all strings
 * %S: Number of free strings
 * %U: Number of USERDEFs
 * %i: Number of all images
 * %B: Number of BITBLKs
 * %F: Number of free images
 * %I: Number of ICONBLKs
 * %C: Number of CICONBLKs
 * %c: Number of CICONs
 * %t: Number of TEDINFOs
 * %O: Number of OBJECTs
 * %T: Number of TREEs
 * %%: the '%'-character
 */

%BEGIN%
/*
 * resource set indices for %n
 *
 * created by ORCS %V
 */

/*
 * Number of Strings:        %s
 * Number of Bitblks:        %B
 * Number of Iconblks:       %I
 * Number of Color Iconblks: %C
 * Number of Color Icons:    %c
 * Number of Tedinfos:       %t
 * Number of Free Strings:   %S
 * Number of Free Images:    %F
 * Number of Objects:        %O
 * Number of Trees:          %T
 * Number of Userblks:       %U
 * Number of Images:         %i
 * Total file size:          %f
 */

%D

#ifndef TARGET_192
#define RS_NOBS %Oc
#define RS_NTREE %Tc
#define RS_NTED %tc
#define RS_NIB %Ic
#define RS_NBB %Bc
#else
#define RS_NOBS %O
#define RS_NTREE %T
#define RS_NTED %t
#define RS_NIB %I
#define RS_NBB %B
#endif


/* The following arrays live in RAM */
extern OBJECT  desk_rs_obj[];
extern TEDINFO desk_rs_tedinfo[];

/* This array lives in ROM and points to RAM data */
extern OBJECT * const desk_rs_trees[];

/* The following resource data live in ROM */
extern const char * const desk_rs_fstr[];
extern const BITBLK       desk_rs_bitblk[];


extern void %n_rsc_init(void);
