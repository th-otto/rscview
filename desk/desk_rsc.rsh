/*
 * desk_rsc.c
 *
 * Generated from desktop.rsc and desktop.rso by ORCS 2.14
 *
 * Copyright 2013-2016 The EmuTOS development team
 *
 * This software is licenced under the GNU General Public License.
 * Please see LICENSE.TXT for further information.
 */

#include "config.h"
#include "string.h"
#include "portab.h"
#include "obdefs.h"
#include "../desk/deskmain.h"
#include "gemrslib.h"
#include "desk_rsc.h"
#include "nls.h"

#ifndef OS_NORMAL
#  define OS_NORMAL 0x0000
#endif
#ifndef OS_SELECTED
#  define OS_SELECTED 0x0001
#endif
#ifndef OS_CROSSED
#  define OS_CROSSED 0x0002
#endif
#ifndef OS_CHECKED
#  define OS_CHECKED 0x0004
#endif
#ifndef OS_DISABLED
#  define OS_DISABLED 0x0008
#endif
#ifndef OS_OUTLINED
#  define OS_OUTLINED 0x0010
#endif
#ifndef OS_SHADOWED
#  define OS_SHADOWED 0x0020
#endif
#ifndef OS_WHITEBAK
#  define OS_WHITEBAK 0x0040
#endif
#ifndef OS_DRAW3D
#  define OS_DRAW3D 0x0080
#endif

#ifndef OF_NONE
#  define OF_NONE 0x0000
#endif
#ifndef OF_SELECTABLE
#  define OF_SELECTABLE 0x0001
#endif
#ifndef OF_DEFAULT
#  define OF_DEFAULT 0x0002
#endif
#ifndef OF_EXIT
#  define OF_EXIT 0x0004
#endif
#ifndef OF_EDITABLE
#  define OF_EDITABLE 0x0008
#endif
#ifndef OF_RBUTTON
#  define OF_RBUTTON 0x0010
#endif
#ifndef OF_LASTOB
#  define OF_LASTOB 0x0020
#endif
#ifndef OF_TOUCHEXIT
#  define OF_TOUCHEXIT 0x0040
#endif
#ifndef OF_HIDETREE
#  define OF_HIDETREE 0x0080
#endif
#ifndef OF_INDIRECT
#  define OF_INDIRECT 0x0100
#endif
#ifndef OF_FL3DIND
#  define OF_FL3DIND 0x0200
#endif
#ifndef OF_FL3DBAK
#  define OF_FL3DBAK 0x0400
#endif
#ifndef OF_FL3DACT
#  define OF_FL3DACT 0x0600
#endif
#ifndef OF_MOVEABLE
#  define OF_MOVEABLE 0x0800
#endif
#ifndef OF_POPUP
#  define OF_POPUP 0x1000
#endif

#ifndef R_CICONBLK
#  define R_CICONBLK 17
#endif
#ifndef R_CICON
#  define R_CICON 18
#endif

#ifndef G_SWBUTTON
#  define G_SWBUTTON 34
#endif
#ifndef G_POPUP
#  define G_POPUP 35
#endif
#ifndef G_EDIT
#  define G_EDIT 37
#endif
#ifndef G_SHORTCUT
#  define G_SHORTCUT 38
#endif
#ifndef G_SLIST
#  define G_SLIST 39
#endif
#ifndef G_EXTBOX
#  define G_EXTBOX 40
#endif
#ifndef G_OBLINK
#  define G_OBLINK 41
#endif

#ifndef _WORD
#  ifdef WORD
#    define _WORD WORD
#  else
#    define _WORD short
#  endif
#endif

#ifndef _UBYTE
#  define _UBYTE char
#endif

#ifndef _LONG
#  ifdef LONG
#    define _LONG LONG
#  else
#    define _LONG long
#  endif
#endif

#ifndef C_UNION
#  define C_UNION(x) (_LONG)(x)
#endif

#ifndef SHORT
#  define SHORT short
#endif

#ifndef CP
#  define CP (SHORT *)
#endif

#ifndef CW
#  define CW (_WORD *)
#endif


#include "desk_rsc.h"

#ifndef TARGET_192
#define RS_NOBS 204
#define RS_NTREE 12
#define RS_NTED 25
#define RS_NIB 0
#define RS_NBB 2
#else
#define RS_NOBS 295
#define RS_NTREE 17
#define RS_NTED 27
#define RS_NIB 0
#define RS_NBB 2
#endif
#define NUM_TI   RS_NTED
#define NUM_OBS  RS_NOBS
#define NUM_BB   RS_NBB
#define NUM_TREE RS_NTREE

static char desktop_string_0[] = N_(" Desk ");
static char desktop_string_1[] = N_(" File ");
static char desktop_string_2[] = N_(" View ");
static char desktop_string_3[] = N_(" Options ");
static char desktop_string_4[] = N_("  Desktop info...   ");
static char desktop_string_5[] = "--------------------";
static char desktop_string_6[] = "1                 ";
static char desktop_string_7[] = "2                 ";
static char desktop_string_8[] = "3                 ";
static char desktop_string_9[] = "4                 ";
static char desktop_string_10[] = "5                 ";
static char desktop_string_11[] = "6                 ";
static char desktop_string_12[] = N_("  Open            ^O ");
static char desktop_string_13[] = N_("  Info/Rename...  ^I ");
static char desktop_string_14[] = "---------------------";
static char desktop_string_15[] = N_("  New Folder...   ^N ");
static char desktop_string_16[] = N_("  Close           ^U ");
static char desktop_string_17[] = N_("  Close window    ^W ");
static char desktop_string_18[] = "---------------------";
static char desktop_string_19[] = N_("  Delete...       ^D ");
static char desktop_string_20[] = N_("  Format...          ");
static char desktop_string_21[] = "---------------------";
static char desktop_string_22[] = N_("  Execute EmuCON  ^Z ");
static char desktop_string_23[] = N_("  Shutdown        ^Q ");
static char desktop_string_24[] = N_("  Show as text  ^T ");
static char desktop_string_25[] = "-------------------";
static char desktop_string_26[] = N_("  Sort by name     ");
static char desktop_string_27[] = N_("  Sort by type     ");
static char desktop_string_28[] = N_("  Sort by size     ");
static char desktop_string_29[] = N_("  Sort by date     ");
static char desktop_string_30[] = N_("  No sort          ");
static char desktop_string_31[] = N_("  Install icon...         ");
static char desktop_string_32[] = N_("  Install application...  ");
static char desktop_string_33[] = N_("  Install devices         ");
static char desktop_string_34[] = N_("  Remove desktop icon     ");
static char desktop_string_35[] = "--------------------------";
static char desktop_string_36[] = N_("  Set preferences...      ");
static char desktop_string_37[] = N_("  Save desktop...      ^S ");
static char desktop_string_38[] = N_("  Change resolution... ^R ");
static char desktop_string_39[] = "0";
static char desktop_string_40[] = "@2345678901";
static char desktop_string_41[] = N_("Name: ________.___");
static char desktop_string_42[] = "fffffffffff";
static char desktop_string_43[] = "@2345678\0\0";
static char desktop_string_44[] = N_("Size: __________");
static char desktop_string_45[] = "9999999999";
static char desktop_string_46[] = "@54321\0\0\0\0";
static char desktop_string_47[] = N_("Date: __________");
static char desktop_string_48[] = "XXXXXXXXXX";
static char desktop_string_49[] = "@12345";
static char desktop_string_50[] = N_("Time: __:__ __");
static char desktop_string_51[] = "9999aa";
static char desktop_string_52[] = "@1234";
static char desktop_string_53[] = N_("Number of folders: _____");
static char desktop_string_54[] = "99999";
static char desktop_string_55[] = "@8765";
static char desktop_string_56[] = N_("Number of files: _____");
static char desktop_string_57[] = "99999";
static char desktop_string_58[] = N_("Attribute:");
static char desktop_string_59[] = N_("Read/Write");
static char desktop_string_60[] = N_("Read-Only");
static char desktop_string_61[] = N_("OK");
static char desktop_string_62[] = N_("Cancel");
static char desktop_string_63[] = N_("DISK INFORMATION");
static char desktop_string_64[] = "@";
static char desktop_string_65[] = N_("Drive identifier: _:");
static char desktop_string_66[] = "A";
static char desktop_string_67[] = "@2345678901";
static char desktop_string_68[] = N_("Disk label: ________.___");
static char desktop_string_69[] = "fffffffffff";
static char desktop_string_70[] = "@1234";
static char desktop_string_71[] = N_("Number of folders: _____");
static char desktop_string_72[] = "99999";
static char desktop_string_73[] = "@2345";
static char desktop_string_74[] = N_("Number of files: _____");
static char desktop_string_75[] = "99999";
static char desktop_string_76[] = "@7654321\0\0";
static char desktop_string_77[] = N_("Bytes used: __________");
static char desktop_string_78[] = "9999999999";
static char desktop_string_79[] = "@1010101\0\0";
static char desktop_string_80[] = N_("Bytes available: __________");
static char desktop_string_81[] = "9999999999";
static char desktop_string_82[] = N_("OK");
static char desktop_string_83[] = "- EmuTOS -";
static char desktop_string_84[] = N_("Version");
static char desktop_string_85[] = "0.0.0";
static char desktop_string_86[] = N_("Copyright \275 2001-");
static char desktop_string_87[] = "9999";
static char desktop_string_88[] = N_("The EmuTOS development team");
static char desktop_string_89[] = "http://emutos.sourceforge.net/";
static char desktop_string_90[] = N_("Based on \'GPLed\' sources");
static char desktop_string_91[] = "";
static char desktop_string_92[] = N_("\275 1987 Digital Research, Inc.");
static char desktop_string_93[] = N_("\275 1999 Caldera Thin Clients, Inc.");
static char desktop_string_94[] = N_("\275 2001 Lineo, Inc.");
static char desktop_string_95[] = N_("EmuTOS is distributed under the GPL");
static char desktop_string_96[] = N_("See doc\\license.txt for details");
static char desktop_string_97[] = N_("OK");
static char desktop_string_98[] = N_("OPEN APPLICATION");
static char desktop_string_99[] = "@2345678901";
static char desktop_string_100[] = N_("Name: ________.___");
static char desktop_string_101[] = "fffffffffff";
static char desktop_string_102[] = N_("Parameters:");
static char desktop_string_103[] = "@1234567890123456789012345678901234567";
static char desktop_string_104[] = "______________________________________";
static char desktop_string_105[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
static char desktop_string_106[] = N_("OK");
static char desktop_string_107[] = N_("Cancel");
static char desktop_string_108[] = N_("INSTALL DESKTOP ICON");
static char desktop_string_109[] = "@";
static char desktop_string_110[] = N_("Identifier: _:");
static char desktop_string_111[] = "A";
static char desktop_string_112[] = "@1234876512\0";
static char desktop_string_113[] = N_("Label: ____________");
static char desktop_string_114[] = "XXXXXXXXXXXX";
static char desktop_string_115[] = N_("Item type:");
static char desktop_string_116[] = N_("Drive");
static char desktop_string_117[] = N_("Trash can");
static char desktop_string_118[] = N_("Printer");
static char desktop_string_119[] = "\001";
static char desktop_string_120[] = "\002";
static char desktop_string_121[] = N_("Icon:");
static char desktop_string_122[] = N_("OK");
static char desktop_string_123[] = N_("Skip");
static char desktop_string_124[] = N_("Cancel");
static char desktop_string_125[] = N_("INSTALL APPLICATION");
static char desktop_string_126[] = "@1234876512";
static char desktop_string_127[] = N_("Application name: ________.___");
static char desktop_string_128[] = "FFFFFFFFFFF";
static char desktop_string_129[] = "@1234876512";
static char desktop_string_130[] = N_("Arguments: ___________");
static char desktop_string_131[] = "XXXXXXXXXXX";
static char desktop_string_132[] = "@12";
static char desktop_string_133[] = N_("Document type: ___");
static char desktop_string_134[] = "FFF";
static char desktop_string_135[] = "@1";
static char desktop_string_136[] = N_("Install as: F__");
static char desktop_string_137[] = "99";
static char desktop_string_138[] = N_("Boot status:");
static char desktop_string_139[] = N_("Auto");
static char desktop_string_140[] = N_("Normal");
static char desktop_string_141[] = N_("Application type:");
static char desktop_string_142[] = "TOS";
static char desktop_string_143[] = "TTP";
static char desktop_string_144[] = "GEM";
static char desktop_string_145[] = "GTP";
static char desktop_string_146[] = N_("Default dir:");
static char desktop_string_147[] = N_("Application");
static char desktop_string_148[] = N_("Window");
static char desktop_string_149[] = N_("Parameter:");
static char desktop_string_150[] = N_("Full path");
static char desktop_string_151[] = N_("File name");
static char desktop_string_152[] = N_("Install");
static char desktop_string_153[] = N_("Remove");
static char desktop_string_154[] = N_("Skip");
static char desktop_string_155[] = N_("Cancel");
static char desktop_string_156[] = "0";
static char desktop_string_157[] = "@234\0";
static char desktop_string_158[] = N_("Number of folders: _____");
static char desktop_string_159[] = "99999";
static char desktop_string_160[] = "@432\0";
static char desktop_string_161[] = N_("Number of files: _____");
static char desktop_string_162[] = "99999";
static char desktop_string_163[] = N_("OK");
static char desktop_string_164[] = N_("Cancel");
static char desktop_string_165[] = N_("NAME CONFLICT DURING COPY");
static char desktop_string_166[] = "@3456354890";
static char desktop_string_167[] = N_("Current name: ________.___");
static char desktop_string_168[] = "fffffffffff";
static char desktop_string_169[] = "@5436354890";
static char desktop_string_170[] = N_("Copy\'s name: ________.___");
static char desktop_string_171[] = "fffffffffff";
static char desktop_string_172[] = N_("OK");
static char desktop_string_173[] = N_("Skip");
static char desktop_string_174[] = N_("Stop");
static char desktop_string_175[] = N_("NEW FOLDER");
static char desktop_string_176[] = "@1726354890";
static char desktop_string_177[] = N_("Name: ________.___");
static char desktop_string_178[] = "fffffffffff";
static char desktop_string_179[] = N_("OK");
static char desktop_string_180[] = N_("Cancel");
static char desktop_string_181[] = N_("SET PREFERENCES");
static char desktop_string_182[] = N_("Confirm deletes:");
static char desktop_string_183[] = N_("Yes");
static char desktop_string_184[] = N_("No");
static char desktop_string_185[] = N_("Confirm copies:");
static char desktop_string_186[] = N_("Yes");
static char desktop_string_187[] = N_("No");
static char desktop_string_188[] = N_("Confirm overwrites:");
static char desktop_string_189[] = N_("Yes");
static char desktop_string_190[] = N_("No");
static char desktop_string_191[] = N_("Sound effects:");
static char desktop_string_192[] = N_("On");
static char desktop_string_193[] = N_("Off");
static char desktop_string_194[] = N_("More preferences");
static char desktop_string_195[] = N_("OK");
static char desktop_string_196[] = N_("Cancel");
static char desktop_string_197[] = N_("SET PREFERENCES");
static char desktop_string_198[] = N_("Double-click speed:");
static char desktop_string_199[] = "1";
static char desktop_string_200[] = "2";
static char desktop_string_201[] = "3";
static char desktop_string_202[] = "4";
static char desktop_string_203[] = "5";
static char desktop_string_204[] = N_("To drop down menus:");
static char desktop_string_205[] = N_("Click");
static char desktop_string_206[] = N_("No click");
static char desktop_string_207[] = N_("Time format:");
static char desktop_string_208[] = N_("Default");
static char desktop_string_209[] = N_("12H");
static char desktop_string_210[] = N_("24H");
static char desktop_string_211[] = N_("Date format:");
static char desktop_string_212[] = N_("Default");
static char desktop_string_213[] = N_("MDY");
static char desktop_string_214[] = N_("DMY");
static char desktop_string_215[] = N_("OK");
static char desktop_string_216[] = N_("Cancel");
#ifndef TARGET_192
static char desktop_string_217[] = N_("SET SCREEN RESOLUTION");
static char desktop_string_218[] = "ST Low";
static char desktop_string_219[] = "ST Medium";
static char desktop_string_220[] = "ST High";
static char desktop_string_221[] = N_("TT Low");
static char desktop_string_222[] = N_("TT Medium");
static char desktop_string_223[] = N_("OK");
static char desktop_string_224[] = N_("Cancel");
static char desktop_string_225[] = N_("SET SCREEN RESOLUTION");
static char desktop_string_226[] = "2    4   16   256";
static char desktop_string_227[] = "TC";
static char desktop_string_228[] = "";
static char desktop_string_229[] = "";
static char desktop_string_230[] = "";
static char desktop_string_231[] = "";
static char desktop_string_232[] = "";
static char desktop_string_233[] = "";
static char desktop_string_234[] = "";
static char desktop_string_235[] = "";
static char desktop_string_236[] = "";
static char desktop_string_237[] = "";
static char desktop_string_238[] = "";
static char desktop_string_239[] = "";
static char desktop_string_240[] = "";
static char desktop_string_241[] = "";
static char desktop_string_242[] = "";
static char desktop_string_243[] = "";
static char desktop_string_244[] = N_("High");
static char desktop_string_245[] = N_("Medium");
static char desktop_string_246[] = N_("Low");
static char desktop_string_247[] = "640 x 480";
static char desktop_string_248[] = "640 x 240";
static char desktop_string_249[] = "320 x 480";
static char desktop_string_250[] = "320 x 240";
static char desktop_string_251[] = "ST Compatible";
static char desktop_string_252[] = N_("OK");
static char desktop_string_253[] = N_("Cancel");
static char desktop_string_254[] = N_("INSTALL WINDOW ICON");
static char desktop_string_255[] = "@1234876512";
static char desktop_string_256[] = N_("Name: ________.___");
static char desktop_string_257[] = "FFFFFFFFFFF";
static char desktop_string_258[] = N_("Item type:");
static char desktop_string_259[] = N_("Folder");
static char desktop_string_260[] = N_("File");
static char desktop_string_261[] = "\001";
static char desktop_string_262[] = "\002";
static char desktop_string_263[] = N_("Icon:");
static char desktop_string_264[] = N_("Install");
static char desktop_string_265[] = N_("Remove");
static char desktop_string_266[] = N_("Skip");
static char desktop_string_267[] = N_("Cancel");
static char desktop_string_268[] = N_("SET SCREEN RESOLUTION");
static char desktop_string_269[] = N_("Monochrome only");
static char desktop_string_270[] = "Amiga";
static char desktop_string_271[] = N_("640x512");
static char desktop_string_272[] = N_("640x256");
static char desktop_string_273[] = N_("320x512");
static char desktop_string_274[] = N_("320x256");
static char desktop_string_275[] = N_("640x480");
static char desktop_string_276[] = N_("640x240");
static char desktop_string_277[] = N_("320x480");
static char desktop_string_278[] = N_("320x240");
static char desktop_string_279[] = "Falcon";
static char desktop_string_280[] = N_("640x200");
static char desktop_string_281[] = N_("320x400");
static char desktop_string_282[] = N_("320x200");
static char desktop_string_283[] = "ST";
static char desktop_string_284[] = N_("640x400");
static char desktop_string_285[] = N_("OK");
static char desktop_string_286[] = N_("Cancel");
static char desktop_string_287[] = N_("FORMAT FLOPPY DISKETTE");
static char desktop_string_288[] = N_("Drive:");
static char desktop_string_289[] = "A";
static char desktop_string_290[] = "B";
static char desktop_string_291[] = "\0\0\0\0\0\0\0\0\0\0\0";
static char desktop_string_292[] = N_("Label: ________.___");
static char desktop_string_293[] = "fffffffffff";
static char desktop_string_294[] = N_("Single sided");
static char desktop_string_295[] = N_("Double sided");
static char desktop_string_296[] = N_("High density");
static char desktop_string_297[] = N_("OK");
static char desktop_string_298[] = N_("Cancel");
#endif
static char desktop_string_299[] = N_("%ld bytes used in %d items.");
static char desktop_string_300[] = N_("  Show as icons ^T");
static char desktop_string_301[] = N_("  Show as text  ^T");
static char desktop_string_302[] = N_("FILE INFORMATION");
static char desktop_string_303[] = N_("FOLDER INFORMATION");
static char desktop_string_304[] = N_("COPY FILE(S)");
static char desktop_string_305[] = N_("MOVE FILE(S)");
static char desktop_string_306[] = N_("DELETE FILE(S)");
static char desktop_string_307[] = "am";
static char desktop_string_308[] = "pm";
static char desktop_string_309[] = "640 x 400";
static char desktop_string_310[] = "640 x 200";
static char desktop_string_311[] = "320 x 400";
static char desktop_string_312[] = "320 x 200";
static char desktop_string_313[] = N_("DISK");
static char desktop_string_314[] = N_("TRASH");
static char desktop_string_315[] = N_("-More-");
static char desktop_string_316[] = N_("-End of file-");
static char desktop_string_317[] = N_("-File read error-");
static char desktop_string_318[] = N_("[2][Switch resolution?][Yes|No]");
static char desktop_string_319[] = N_("[2][Show this document?][Show|Cancel]");
static char desktop_string_320[] = N_("[1][This document type has no|associated application.][  OK  ]");
static char desktop_string_321[] = N_("[1][The GEM Desktop has no more|windows.  Please close a|window that you are not|using.][  OK  ]");
static char desktop_string_322[] = N_("[3][You cannot copy a parent folder|into one of its child folders.][  OK  ]");
static char desktop_string_323[] = N_("[3][Invalid copy operation.][  OK  ]");
static char desktop_string_324[] = N_("[2][Abort this operation?][  Yes  |  No  ]");
static char desktop_string_325[] = N_("[2][Do you want to delete all|the files on drive %c: ?][Yes| No ]");
static char desktop_string_326[] = N_("[1][Sorry, the desktop cannot|install any more icons|or applications.][  OK  ]");
static char desktop_string_327[] = N_("[2][You cannot create a folder|with that name. Please retry|with a new name, or cancel.][Cancel|Retry]");
static char desktop_string_328[] = N_("[1][This disk does not have enough|room for this operation.][  OK  ]");
static char desktop_string_329[] = N_("[3][Cannot create EMUDESK.INF.][  OK  ]");
static char desktop_string_330[] = N_("[3][You cannot create or|access a folder this far|down the directory path.][  OK  ]");
static char desktop_string_331[] = N_("[3][You cannot create or access|a file or folder with that|name, because the pathname|is too long.][  OK  ]");
static char desktop_string_332[] = N_("[1][You cannot open the trash|can icon into a window.][  OK  ]");
static char desktop_string_333[] = N_("[1][The trash can is used to|permanently delete files|or folders.][  OK  ]");
static char desktop_string_334[] = N_("[1][You cannot drag applications,|files or folders onto the|Desktop.][  OK  ]");
static char desktop_string_335[] = N_("[1][You cannot drag the trash can|icon into a window.][  OK  ]");
static char desktop_string_336[] = N_("[1][You cannot place this|icon over another icon.][  OK  ]");
static char desktop_string_337[] = N_("[2][Save Desktop?][  OK  |Cancel]");
static char desktop_string_338[] = N_("[2][Do you want to remove icons|or delete files?][Icons|Files|Cancel]");
static char desktop_string_339[] = N_("[1][Invalid function key value.][  OK  ]");
static char desktop_string_340[] = N_("[2][Function key already assigned.|Do you want to reassign it?][  OK  |Cancel]");
static char desktop_string_341[] = N_("[3][Not enough memory to|initialise EmuDesk.|You must reboot.][OK]");
static char desktop_string_342[] = N_("[2][%s not found.|Do you want to remove the|icon or locate the icon?][Remove|Locate|Cancel]");
#ifndef TARGET_192
static char desktop_string_343[] = N_("[1][Please select icon type.][Desktop|Window|Cancel]");
static char desktop_string_344[] = N_("[1][No matching file type.][  OK  ]");
static char desktop_string_345[] = N_("[3][Formatting error. The target|disk may be write-protected|or unusable.][Retry|Abort]");
static char desktop_string_346[] = N_("[1][This disk has %ld bytes|available to the user.][ Continue | Quit ]");
#endif


/* data of DEICON */
static _UBYTE IMAGE0[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xD0, 0x00, 0x00, 0x0B, 0xD0, 0x00, 0x00, 0x1B, 0xD8, 0x00, 
0x00, 0x1B, 0xD8, 0x00, 0x00, 0x2B, 0xD4, 0x00, 0x00, 0x3B, 0xDC, 0x00, 0x00, 0x6B, 0xD6, 0x00, 
0x00, 0x5B, 0xDA, 0x00, 0x00, 0xAB, 0xD5, 0x00, 0x00, 0xDB, 0xDB, 0x00, 0x01, 0xAB, 0xD5, 0x80, 
0x01, 0x5B, 0xDA, 0x80, 0x02, 0xAB, 0xD5, 0x40, 0x03, 0x53, 0xCA, 0xC0, 0x06, 0xA3, 0xC5, 0x60, 
0x05, 0x43, 0xC2, 0xA0, 0x0A, 0x83, 0xC1, 0x50, 0x0D, 0x03, 0xC0, 0xB0, 0x1A, 0x03, 0xC0, 0x58, 
0x14, 0x03, 0xC0, 0x28, 0x28, 0x03, 0xC0, 0x14, 0x30, 0x03, 0xC0, 0x0C, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static _UBYTE IMAGE1[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xD0, 0x00, 0x00, 0x0B, 0xD0, 0x00, 0x00, 0x1B, 0xD8, 0x00, 
0x00, 0x1B, 0xD8, 0x00, 0x00, 0x2B, 0xD4, 0x00, 0x00, 0x3B, 0xDC, 0x00, 0x00, 0x6B, 0xD6, 0x00, 
0x00, 0x5B, 0xDA, 0x00, 0x00, 0xAB, 0xD5, 0x00, 0x00, 0xDB, 0xDB, 0x00, 0x01, 0xAB, 0xD5, 0x80, 
0x01, 0x5B, 0xDA, 0x80, 0x02, 0xAB, 0xD5, 0x40, 0x03, 0x53, 0xCA, 0xC0, 0x06, 0xA3, 0xC5, 0x60, 
0x05, 0x43, 0xC2, 0xA0, 0x0A, 0x83, 0xC1, 0x50, 0x0D, 0x03, 0xC0, 0xB0, 0x1A, 0x03, 0xC0, 0x58, 
0x14, 0x03, 0xC0, 0x28, 0x28, 0x03, 0xC0, 0x14, 0x30, 0x03, 0xC0, 0x0C, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


char *desk_rs_fstr[] = {
	desktop_string_299,
	desktop_string_300,
	desktop_string_301,
	desktop_string_302,
	desktop_string_303,
	desktop_string_304,
	desktop_string_305,
	desktop_string_306,
	desktop_string_307,
	desktop_string_308,
	desktop_string_309,
	desktop_string_310,
	desktop_string_311,
	desktop_string_312,
	desktop_string_313,
	desktop_string_314,
	desktop_string_315,
	desktop_string_316,
	desktop_string_317,
	desktop_string_318,
	desktop_string_319,
	desktop_string_320,
	desktop_string_321,
	desktop_string_322,
	desktop_string_323,
	desktop_string_324,
	desktop_string_325,
	desktop_string_326,
	desktop_string_327,
	desktop_string_328,
	desktop_string_329,
	desktop_string_330,
	desktop_string_331,
	desktop_string_332,
	desktop_string_333,
	desktop_string_334,
	desktop_string_335,
	desktop_string_336,
	desktop_string_337,
	desktop_string_338,
	desktop_string_339,
	desktop_string_340,
	desktop_string_341,
	desktop_string_342,
#ifndef TARGET_192
	desktop_string_343,
	desktop_string_344,
	desktop_string_345,
	desktop_string_346,
#endif
};


static BITBLK rs_bitblk[NUM_BB] = {
	{ CP IMAGE0, 4, 32, 0, 0, 1 },
	{ CP IMAGE1, 4, 32, 0, 0, 1 },
};


static TEDINFO rs_tedinfo[NUM_TI] = {
	{ desktop_string_40, desktop_string_41, desktop_string_42, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,19 }, /* FFNAME */
	{ desktop_string_43, desktop_string_44, desktop_string_45, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 11,17 }, /* FFSIZE */
	{ desktop_string_46, desktop_string_47, desktop_string_48, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 11,17 }, /* FFDATE */
	{ desktop_string_49, desktop_string_50, desktop_string_51, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 7,15 }, /* FFTIME */
	{ desktop_string_52, desktop_string_53, desktop_string_54, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 6,25 }, /* FFNUMFOL */
	{ desktop_string_55, desktop_string_56, desktop_string_57, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 6,23 }, /* FFNUMFIL */
	{ desktop_string_64, desktop_string_65, desktop_string_66, IBM, 0, TE_CNTR, 0x1100, 0x0, 0, 2,21 }, /* DIDRIVE */
	{ desktop_string_67, desktop_string_68, desktop_string_69, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,25 }, /* DIVOLUME */
	{ desktop_string_70, desktop_string_71, desktop_string_72, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 6,25 }, /* DINFOLDS */
	{ desktop_string_73, desktop_string_74, desktop_string_75, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 6,23 }, /* DINFILES */
	{ desktop_string_76, desktop_string_77, desktop_string_78, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 11,23 }, /* DIUSED */
	{ desktop_string_79, desktop_string_80, desktop_string_81, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 11,28 }, /* DIAVAIL */
	{ desktop_string_99, desktop_string_100, desktop_string_101, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,19 }, /* APPLNAME */
	{ desktop_string_103, desktop_string_104, desktop_string_105, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 39,39 }, /* APPLPARM */
	{ desktop_string_109, desktop_string_110, desktop_string_111, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 2,15 }, /* ID_ID */
	{ desktop_string_112, desktop_string_113, desktop_string_114, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 13,20 }, /* ID_LABEL */
	{ desktop_string_126, desktop_string_127, desktop_string_128, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,31 }, /* APNAME */
	{ desktop_string_129, desktop_string_130, desktop_string_131, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,23 }, /* APARGS */
	{ desktop_string_132, desktop_string_133, desktop_string_134, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 4,19 }, /* APDOCTYP */
	{ desktop_string_135, desktop_string_136, desktop_string_137, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 3,16 }, /* APFUNKEY */
	{ desktop_string_157, desktop_string_158, desktop_string_159, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 6,25 }, /* CDFOLDS */
	{ desktop_string_160, desktop_string_161, desktop_string_162, IBM, 0, TE_RIGHT, 0x1100, 0x0, 0, 6,23 }, /* CDFILES */
	{ desktop_string_166, desktop_string_167, desktop_string_168, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,27 }, /* CACURRNA */
	{ desktop_string_169, desktop_string_170, desktop_string_171, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,26 }, /* CACOPYNA */
	{ desktop_string_176, desktop_string_177, desktop_string_178, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,19 }, /* MKNAME */
#ifndef TARGET_192
	{ desktop_string_255, desktop_string_256, desktop_string_257, IBM, 0, TE_LEFT, 0x1100, 0x0, 0, 12,19 }, /* IW_NAME */
	{ desktop_string_291, desktop_string_292, desktop_string_293, IBM, 6, TE_LEFT, 0x1180, 0x0, -1, 12,20 }, /* FMTLABEL */
#endif
};


static OBJECT rs_object[NUM_OBS] = {
/* ADMENU */

	{ -1, 1, 7, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 0,0, 80,25 },
	{ 7, 2, 2, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 0,0, 80,513 },
	{ 1, 3, 6, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 2,0, 28,769 },
	{ 4, -1, -1, G_TITLE, OF_NONE, OS_NORMAL, C_UNION(desktop_string_0), 0,0, 6,769 }, /* DESKMENU */
	{ 5, -1, -1, G_TITLE, OF_NONE, OS_NORMAL, C_UNION(desktop_string_1), 6,0, 6,769 }, /* FILEMENU */
	{ 6, -1, -1, G_TITLE, OF_NONE, OS_NORMAL, C_UNION(desktop_string_2), 12,0, 7,769 }, /* VIEWMENU */
	{ 2, -1, -1, G_TITLE, OF_NONE, OS_NORMAL, C_UNION(desktop_string_3), 19,0, 9,769 }, /* OPTNMENU */
	{ 0, 8, 38, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 0,769, 80,23 },
	{ 17, 9, 16, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 2,0, 20,8 },
	{ 10, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_4), 0,0, 20,1 }, /* ABOUITEM */
	{ 11, -1, -1, G_STRING, OF_NONE, OS_DISABLED, C_UNION(desktop_string_5), 0,1, 20,1 },
	{ 12, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_6), 0,2, 20,1 },
	{ 13, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_7), 0,3, 20,1 },
	{ 14, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_8), 0,4, 20,1 },
	{ 15, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_9), 0,5, 20,1 },
	{ 16, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_10), 0,6, 20,1 },
	{ 8, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_11), 0,7, 20,1 },
	{ 30, 18, 29, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 8,0, 21,12 },
	{ 19, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_12), 0,0, 21,1 }, /* OPENITEM */
	{ 20, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_13), 0,1, 21,1 }, /* SHOWITEM */
	{ 21, -1, -1, G_STRING, OF_NONE, OS_DISABLED, C_UNION(desktop_string_14), 0,2, 21,1 }, /* L1ITEM */
	{ 22, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_15), 0,3, 21,1 }, /* NFOLITEM */
	{ 23, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_16), 0,4, 21,1 }, /* CLOSITEM */
	{ 24, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_17), 0,5, 21,1 }, /* CLSWITEM */
	{ 25, -1, -1, G_STRING, OF_NONE, OS_DISABLED, C_UNION(desktop_string_18), 0,6, 21,1 }, /* L2ITEM */
	{ 26, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_19), 0,7, 21,1 }, /* DELTITEM */
	{ 27, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_20), 0,8, 21,1 }, /* FORMITEM */
	{ 28, -1, -1, G_STRING, OF_NONE, OS_DISABLED, C_UNION(desktop_string_21), 0,9, 21,1 }, /* L3ITEM */
	{ 29, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_22), 0,10, 21,1 }, /* CLIITEM */
	{ 17, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_23), 0,11, 21,1 }, /* QUITITEM */
	{ 38, 31, 37, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 14,0, 19,7 },
	{ 32, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_24), 0,0, 19,1 }, /* ICONITEM */
	{ 33, -1, -1, G_STRING, OF_NONE, OS_DISABLED, C_UNION(desktop_string_25), 0,1, 19,1 }, /* L4ITEM */
	{ 34, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_26), 0,2, 19,1 }, /* NAMEITEM */
	{ 35, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_27), 0,3, 19,1 }, /* TYPEITEM */
	{ 36, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_28), 0,4, 19,1 }, /* SIZEITEM */
	{ 37, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_29), 0,5, 19,1 }, /* DATEITEM */
	{ 30, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_30), 0,6, 19,1 }, /* NSRTITEM */
	{ 7, 39, 46, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1100L), 21,0, 26,8 },
	{ 40, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_31), 0,0, 26,1 }, /* IICNITEM */
	{ 41, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_32), 0,1, 26,1 }, /* IAPPITEM */
	{ 42, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_33), 0,2, 26,1 }, /* IDSKITEM */
	{ 43, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_34), 0,3, 26,1 }, /* RICNITEM */
	{ 44, -1, -1, G_STRING, OF_NONE, OS_DISABLED, C_UNION(desktop_string_35), 0,4, 26,1 }, /* L5ITEM */
	{ 45, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_36), 0,5, 26,1 }, /* PREFITEM */
	{ 46, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_37), 0,6, 26,1 }, /* SAVEITEM */
	{ 38, -1, -1, G_STRING, OF_LASTOB, OS_NORMAL, C_UNION(desktop_string_38), 0,7, 26,1 }, /* RESITEM */

/* ADFFINFO */

	{ -1, 1, 13, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 38,14 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_39), 11,1, 1,1 }, /* FFTITLE */
	{ 3, -1, -1, G_FBOXTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[0]), 1,3, 27,1 }, /* FFNAME */
	{ 4, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[1]), 1,4, 25,1 }, /* FFSIZE */
	{ 5, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[2]), 1,5, 25,1 }, /* FFDATE */
	{ 6, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[3]), 1,6, 23,1 }, /* FFTIME */
	{ 7, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[4]), 1,7, 31,1 }, /* FFNUMFOL */
	{ 8, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[5]), 1,8, 31,1 }, /* FFNUMFIL */
	{ 9, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_58), 1,10, 10,1 },
	{ 12, 10, 11, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 12,10, 25,1 },
	{ 11, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_59), 0,0, 11,1 }, /* FFRWRITE */
	{ 9, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_60), 12,0, 13,1 }, /* FFRONLY */
	{ 13, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_61), 9,12, 8,1 }, /* FFOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_62), 22,12, 8,1 }, /* FFCNCL */

/* ADDISKIN */

	{ -1, 1, 8, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 38,12 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_63), 3,1, 16,1 }, /* DITITLE */
	{ 3, -1, -1, G_FBOXTEXT, 0x8000, OS_NORMAL, C_UNION(&rs_tedinfo[6]), 2,3, 32,1 }, /* DIDRIVE */
	{ 4, -1, -1, G_FBOXTEXT, 0x4000, OS_NORMAL, C_UNION(&rs_tedinfo[7]), 2,4, 32,1 }, /* DIVOLUME */
	{ 5, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[8]), 2,5, 32,1 }, /* DINFOLDS */
	{ 6, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[9]), 2,6, 32,1 }, /* DINFILES */
	{ 7, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[10]), 2,7, 32,1 }, /* DIUSED */
	{ 8, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[11]), 2,8, 32,1 }, /* DIAVAIL */
	{ 0, -1, -1, G_BUTTON, 0x27, OS_NORMAL, C_UNION(desktop_string_82), 15,10, 8,1 }, /* DIOK */

/* ADDINFO */

	{ -1, 1, 17, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x21100L), 0,0, 40,19 },
	{ 2, -1, -1, G_IMAGE, OF_TOUCHEXIT, OS_NORMAL, C_UNION(&rs_bitblk[0]), 3,1, 8192,16384 }, /* DEICON */
	{ 3, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_83), 15,1, 10,1 },
	{ 4, -1, -1, G_IMAGE, OF_NONE, OS_NORMAL, C_UNION(&rs_bitblk[1]), 33,1, 8192,16384 },
	{ 5, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_84), 14,2, 7,1 }, /* DELABEL */
	{ 6, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_85), 22,2, 5,1 }, /* DEVERSN */
	{ 7, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_86), 1,4, 1049,1 },
	{ 8, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_87), 1050,4, 4,1 }, /* DECOPYRT */
	{ 9, -1, -1, G_STRING, 0x8000, OS_NORMAL, C_UNION(desktop_string_88), 1,5, 38,1 },
	{ 10, -1, -1, G_STRING, 0x8000, OS_NORMAL, C_UNION(desktop_string_89), 1,7, 38,1 },
	{ 11, -1, -1, G_STRING, 0x8000, OS_NORMAL, C_UNION(desktop_string_90), 1,9, 38,1 },
	{ 12, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_91), 16,9, 0,1 },
	{ 13, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_92), 3,10, 29,1 },
	{ 14, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_93), 3,11, 33,1 },
	{ 15, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_94), 3,12, 18,1 },
	{ 16, -1, -1, G_STRING, 0x8000, OS_NORMAL, C_UNION(desktop_string_95), 1,14, 38,1 },
	{ 17, -1, -1, G_STRING, 0x8000, OS_NORMAL, C_UNION(desktop_string_96), 1,15, 38,1 },
	{ 0, -1, -1, G_BUTTON, 0x27, OS_NORMAL, C_UNION(desktop_string_97), 16,17, 8,1 }, /* DEOK */

/* ADOPENAP */

	{ -1, 1, 6, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,10 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_98), 11,1, 16,1 }, /* APPLOPEN */
	{ 3, -1, -1, G_FBOXTEXT, OF_EDITABLE, OS_NORMAL, C_UNION(&rs_tedinfo[12]), 1,3, 19,1 }, /* APPLNAME */
	{ 4, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_102), 1,5, 11,1 },
	{ 5, -1, -1, G_FBOXTEXT, OF_EDITABLE, OS_NORMAL, C_UNION(&rs_tedinfo[13]), 1,6, 38,1 }, /* APPLPARM */
	{ 6, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_106), 7,8, 8,1 }, /* APPLOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_107), 25,8, 8,1 }, /* APPLCNCL */

/* ADINSDSK */

	{ -1, 1, 16, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,19 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_108), 9,1, 20,1 },
	{ 3, -1, -1, G_FBOXTEXT, 0x4000, OS_NORMAL, C_UNION(&rs_tedinfo[14]), 2,3, 19,1 }, /* ID_ID */
	{ 4, -1, -1, G_FBOXTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[15]), 2,4, 29,1 }, /* ID_LABEL */
	{ 5, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_115), 2,6, 12,1 },
	{ 9, 6, 8, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 4,8, 32,1 },
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_116), 0,0, 10,1 }, /* ID_DRIVE */
	{ 8, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_117), 11,0, 10,1 }, /* ID_TRASH */
	{ 5, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_118), 22,0, 10,1 }, /* ID_PRINT */
	{ 13, 10, 12, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 16,10, 13,6 }, /* ID_IBOX */
	{ 11, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1101L), 1,1, 6,4 }, /* ID_ICON */
	{ 12, -1, -1, G_BUTTON, 0x41, OS_NORMAL, C_UNION(desktop_string_119), 9,1, 3,1 }, /* ID_UP */
	{ 9, -1, -1, G_BUTTON, 0x41, OS_NORMAL, C_UNION(desktop_string_120), 9,2, 3,1 }, /* ID_DOWN */
	{ 14, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_121), 2,11, 12,1 },
	{ 15, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_122), 2,17, 10,1 }, /* ID_OK */
	{ 16, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_123), 14,17, 11,1 }, /* ID_SKIP */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_124), 27,17, 11,1 }, /* ID_CNCL */

/* ADINSAPP */

	{ -1, 1, 27, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,22 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_125), 9,1, 19,1 }, /* INSAPP */
	{ 3, -1, -1, G_FBOXTEXT, 0x4000, OS_NORMAL, C_UNION(&rs_tedinfo[16]), 2,3, 32,1 }, /* APNAME */
	{ 4, -1, -1, G_FBOXTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[17]), 2,4, 31,1 }, /* APARGS */
	{ 5, -1, -1, G_FBOXTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[18]), 2,5, 23,1 }, /* APDOCTYP */
	{ 6, -1, -1, G_FBOXTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[19]), 2,6, 23,1 }, /* APFUNKEY */
	{ 7, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_138), 2,8, 14,1 },
	{ 10, 8, 9, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 17,8, 21,1 },
	{ 9, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_139), 0,0, 10,1 }, /* APAUTO */
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_140), 11,0, 10,1 }, /* APNORM */
	{ 11, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_141), 10,10, 17,1 }, /* APTYPE */
	{ 16, 12, 15, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 6,12, 26,1 },
	{ 13, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_142), 0,0, 5,1 }, /* APTOS */
	{ 14, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_143), 7,0, 5,1 }, /* APTTP */
	{ 15, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_144), 14,0, 5,1 }, /* APGEM */
	{ 11, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_145), 21,0, 5,1 }, /* APGTP */
	{ 17, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_146), 2,14, 13,1 },
	{ 20, 18, 19, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 16,14, 22,1 },
	{ 19, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_147), 0,0, 12,1 }, /* APDEFAPP */
	{ 17, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_148), 13,0, 9,1 }, /* APDEFWIN */
	{ 21, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_149), 2,16, 13,1 },
	{ 24, 22, 23, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 16,16, 22,1 },
	{ 23, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_150), 0,0, 11,1 }, /* APPMFULL */
	{ 21, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_151), 12,0, 10,1 }, /* APPMFILE */
	{ 25, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_152), 3,18, 15,1 }, /* APINSTAL */
	{ 26, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_153), 22,18, 15,1 }, /* APREMOVE */
	{ 27, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_154), 3,20, 15,1 }, /* APSKIP */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_155), 22,20, 15,1 }, /* APCANCEL */

/* ADCPYDEL */

	{ -1, 1, 5, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 34,8 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_156), 6,1, 1,1 }, /* CDTITLE */
	{ 3, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[20]), 2,3, 30,1 }, /* CDFOLDS */
	{ 4, -1, -1, G_FBOXTEXT, OF_NONE, OS_NORMAL, C_UNION(&rs_tedinfo[21]), 2,4, 30,1 }, /* CDFILES */
	{ 5, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_163), 8,6, 8,1 }, /* CDOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_164), 19,6, 8,1 }, /* CDCNCL */

/* ADCPALER */

	{ -1, 1, 6, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 34,8 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_165), 2,1, 25,1 }, /* CACOPY */
	{ 3, -1, -1, G_FBOXTEXT, 0x4000, OS_NORMAL, C_UNION(&rs_tedinfo[22]), 1,3, 31,1 }, /* CACURRNA */
	{ 4, -1, -1, G_FBOXTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[23]), 1,4, 31,1 }, /* CACOPYNA */
	{ 5, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_172), 1,6, 10,1 }, /* CAOK */
	{ 6, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_173), 12,6, 10,1 }, /* CACNCL */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_174), 23,6, 10,1 }, /* CASTOP */

/* ADMKDBOX */

	{ -1, 1, 4, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 27,7 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_175), 3,1, 10,1 },
	{ 3, -1, -1, G_FBOXTEXT, 0x8008, OS_NORMAL, C_UNION(&rs_tedinfo[24]), 1,3, 25,1 }, /* MKNAME */
	{ 4, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_179), 4,5, 8,1 }, /* MKOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_180), 16,5, 8,1 }, /* MKCNCL */

/* ADSETPRE */

	{ -1, 1, 20, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,15 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_181), 12,1, 15,1 }, /* SPTITLE */
	{ 3, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_182), 1,3, 16,1 },
	{ 6, 4, 5, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 26,3, 13,1 },
	{ 5, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_183), 0,0, 6,1 }, /* SPCDYES */
	{ 3, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_184), 7,0, 6,1 }, /* SPCDNO */
	{ 7, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_185), 1,5, 15,1 },
	{ 10, 8, 9, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 26,5, 13,1 },
	{ 9, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_186), 0,0, 6,1 }, /* SPCCYES */
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_187), 7,0, 6,1 }, /* SPCCNO */
	{ 11, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_188), 1,7, 19,1 },
	{ 14, 12, 13, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 26,7, 13,1 },
	{ 13, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_189), 0,0, 6,1 }, /* SPCOWYES */
	{ 11, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_190), 7,0, 6,1 }, /* SPCOWNO */
	{ 15, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_191), 1,9, 14,1 },
	{ 18, 16, 17, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 26,9, 13,1 },
	{ 17, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_192), 0,0, 6,1 }, /* SPSEYES */
	{ 15, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_193), 7,0, 6,1 }, /* SPSENO */
	{ 19, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_194), 4,11, 32,1 }, /* SPMORE */
	{ 20, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_195), 4,13, 10,1 }, /* SPOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_196), 26,13, 10,1 }, /* SPCNCL */

/* ADSETPR2 */

	{ -1, 1, 24, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,17 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_197), 12,1, 15,1 },
	{ 3, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_198), 1,3, 19,1 },
	{ 9, 4, 8, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 25,3, 14,1 },
	{ 5, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_199), 0,0, 2,1 }, /* SPDC1 */
	{ 6, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_200), 3,0, 2,1 }, /* SPDC2 */
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_201), 6,0, 2,1 }, /* SPDC3 */
	{ 8, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_202), 9,0, 2,1 }, /* SPDC4 */
	{ 3, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_203), 12,0, 2,1 }, /* SPDC5 */
	{ 10, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_204), 1,5, 19,1 },
	{ 13, 11, 12, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 21,5, 19,1 },
	{ 12, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_205), 0,0, 7,1 }, /* SPMNCLKY */
	{ 10, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_206), 8,0, 10,1 }, /* SPMNCLKN */
	{ 14, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_207), 1,7, 12,1 },
	{ 18, 15, 17, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 1,9, 38,1 },
	{ 16, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_208), 0,0, 20,1 }, /* SPTF_DEF */
	{ 17, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_209), 21,0, 8,1 }, /* SPTF12HR */
	{ 14, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_210), 30,0, 8,1 }, /* SPTF24HR */
	{ 19, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_211), 1,11, 12,1 },
	{ 23, 20, 22, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x0L), 1,13, 38,1 },
	{ 21, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_212), 0,0, 20,1 }, /* SPDF_DEF */
	{ 22, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_213), 21,0, 8,1 }, /* SPDFMMDD */
	{ 19, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_214), 30,0, 8,1 }, /* SPDFDDMM */
	{ 24, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_215), 4,15, 10,1 }, /* SPOK2 */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_216), 26,15, 10,1 }, /* SPCNCL2 */

/* ADTTREZ */

#ifndef TARGET_192
	{ -1, 1, 9, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 39,10 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_217), 9,1, 21,1 },
	{ 8, 3, 7, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 1,2, 37,5 },
	{ 4, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_218), 1,1, 11,1 }, /* TTREZSTL */
	{ 5, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_219), 13,1, 11,1 },
	{ 6, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_220), 25,1, 11,1 },
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_221), 1,3, 11,1 },
	{ 2, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_222), 13,3, 11,1 },
	{ 9, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_223), 8,8, 8,1 }, /* TTREZOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_224), 23,8, 8,1 }, /* TTREZCAN */

/* ADFALREZ */

	{ -1, 1, 30, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,18 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_225), 9,1, 21,1 },
	{ 24, 3, 23, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 15,2, 25,13 },
	{ 4, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_226), 1,1, 17,1 },
	{ 5, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_227), 20,1, 2,1 }, /* FREZTEXT */
	{ 6, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_228), 1,3, 1,1 }, /* FREZLIST */
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_229), 6,3, 1,1 },
	{ 8, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_230), 11,3, 1,1 },
	{ 9, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_231), 16,3, 1,1 },
	{ 10, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_232), 1,5, 1,1 },
	{ 11, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_233), 6,5, 1,1 },
	{ 12, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_234), 11,5, 1,1 },
	{ 13, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_235), 16,5, 1,1 },
	{ 14, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_236), 6,7, 1,1 },
	{ 15, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_237), 11,7, 1,1 },
	{ 16, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_238), 16,7, 1,1 },
	{ 17, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_239), 21,7, 1,1 },
	{ 18, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_240), 6,9, 1,1 },
	{ 19, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_241), 11,9, 1,1 },
	{ 20, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_242), 16,9, 1,1 },
	{ 21, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_243), 21,9, 1,1 },
	{ 22, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_244), 1,11, 7,1 },
	{ 23, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_245), 9,11, 7,1 },
	{ 2, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_246), 17,11, 7,1 },
	{ 25, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_247), 1,5, 9,1 }, /* FREZNAME */
	{ 26, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_248), 1,7, 9,1 },
	{ 27, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_249), 1,9, 9,1 },
	{ 28, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_250), 1,11, 9,1 },
	{ 29, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_251), 1,13, 13,1 },
	{ 30, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_252), 9,16, 8,1 }, /* FREZOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_253), 23,16, 8,1 }, /* FREZCAN */

/* ADINSWIN */

	{ -1, 1, 15, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,19 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_254), 10,1, 19,1 },
	{ 3, -1, -1, G_FBOXTEXT, 0x8008, OS_NORMAL, C_UNION(&rs_tedinfo[25]), 2,3, 32,1 }, /* IW_NAME */
	{ 4, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_258), 2,6, 12,1 },
	{ 7, 5, 6, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 15,6, 23,1 }, /* IW_RBOX */
	{ 6, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_259), 0,0, 11,1 }, /* IW_FOLD */
	{ 4, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_260), 12,0, 11,1 }, /* IW_FILE */
	{ 11, 8, 10, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 16,8, 13,6 }, /* IW_IBOX */
	{ 9, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1101L), 1,1, 6,4 }, /* IW_ICON */
	{ 10, -1, -1, G_BUTTON, 0x41, OS_NORMAL, C_UNION(desktop_string_261), 9,1, 3,1 }, /* IW_UP */
	{ 7, -1, -1, G_BUTTON, 0x41, OS_NORMAL, C_UNION(desktop_string_262), 9,2, 3,1 }, /* IW_DOWN */
	{ 12, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_263), 2,9, 12,1 },
	{ 13, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_264), 3,15, 15,1 }, /* IW_INST */
	{ 14, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_265), 22,15, 15,1 }, /* IW_REMV */
	{ 15, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_266), 3,17, 15,1 }, /* IW_SKIP */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_267), 22,17, 15,1 }, /* IW_CNCL */

/* ADAMIREZ */

	{ -1, 1, 19, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 40,18 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_268), 9,1, 21,1 },
	{ 3, -1, -1, G_STRING, 0x8000, OS_NORMAL, C_UNION(desktop_string_269), 12,3, 15,1 },
	{ 4, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_270), 1,6, 5,1 },
	{ 5, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_271), 8,6, 7,1 }, /* AMIREZ0 */
	{ 6, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_272), 16,6, 7,1 },
	{ 7, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_273), 24,6, 7,1 },
	{ 8, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_274), 32,6, 7,1 },
	{ 9, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_275), 8,8, 7,1 },
	{ 10, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_276), 16,8, 7,1 },
	{ 11, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_277), 24,8, 7,1 },
	{ 12, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_278), 32,8, 7,1 },
	{ 13, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_279), 1,9, 6,1 },
	{ 14, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_280), 16,10, 7,1 },
	{ 15, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_281), 24,10, 7,1 },
	{ 16, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_282), 32,10, 7,1 },
	{ 17, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_283), 1,12, 2,1 },
	{ 18, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_284), 8,12, 7,1 },
	{ 19, -1, -1, G_BUTTON, 0x7, OS_NORMAL, C_UNION(desktop_string_285), 9,16, 8,1 }, /* AMREZOK */
	{ 0, -1, -1, G_BUTTON, 0x25, OS_NORMAL, C_UNION(desktop_string_286), 23,16, 8,1 }, /* AMREZCAN */

/* ADFORMAT */

	{ -1, 1, 13, G_BOX, OF_NONE, OS_OUTLINED, C_UNION(0x11100L), 0,0, 32,17 },
	{ 2, -1, -1, G_STRING, OF_NONE, OS_NORMAL, C_UNION(desktop_string_287), 5,1, 22,1 },
	{ 3, -1, -1, G_STRING, 0x4000, OS_NORMAL, C_UNION(desktop_string_288), 1,3, 12,1 },
	{ 6, 4, 5, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 14,3, 9,1 },
	{ 5, -1, -1, G_BUTTON, 0x11, OS_SELECTED, C_UNION(desktop_string_289), 0,0, 4,1 }, /* FMT_DRVA */
	{ 3, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_290), 5,0, 4,1 }, /* FMT_DRVB */
	{ 7, -1, -1, G_FTEXT, 0x4008, OS_NORMAL, C_UNION(&rs_tedinfo[26]), 1,5, 25,1 }, /* FMTLABEL */
	{ 11, 8, 10, G_IBOX, OF_NONE, OS_NORMAL, C_UNION(0x1100L), 9,7, 14,5 },
	{ 9, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_294), 0,0, 14,1 }, /* FMT_SS */
	{ 10, -1, -1, G_BUTTON, 0x11, OS_NORMAL, C_UNION(desktop_string_295), 0,2, 14,1 }, /* FMT_DS */
	{ 7, -1, -1, G_BUTTON, 0x11, OS_SELECTED, C_UNION(desktop_string_296), 0,4, 14,1 }, /* FMT_HD */
	{ 12, -1, -1, G_BOX, OF_NONE, OS_NORMAL, C_UNION(0xFF1101L), 6,13, 20,1 }, /* FMT_BAR */
	{ 13, -1, -1, G_BUTTON, 0x5, OS_NORMAL, C_UNION(desktop_string_297), 6,15, 8,1 }, /* FMT_OK */
	{ 0, -1, -1, G_BUTTON, 0x27, OS_NORMAL, C_UNION(desktop_string_298), 18,15, 8,1 }, /* FMT_CNCL */
#endif
};


static OBJECT *rs_trindex[NUM_TREE] = {
	&rs_object[0], /* ADMENU */
	&rs_object[47], /* ADFFINFO */
	&rs_object[61], /* ADDISKIN */
	&rs_object[70], /* ADDINFO */
	&rs_object[88], /* ADOPENAP */
	&rs_object[95], /* ADINSDSK */
	&rs_object[112], /* ADINSAPP */
	&rs_object[140], /* ADCPYDEL */
	&rs_object[146], /* ADCPALER */
	&rs_object[153], /* ADMKDBOX */
	&rs_object[158], /* ADSETPRE */
	&rs_object[179], /* ADSETPR2 */
#ifndef TARGET_192
	&rs_object[204], /* ADTTREZ */
	&rs_object[214], /* ADFALREZ */
	&rs_object[245], /* ADINSWIN */
	&rs_object[261], /* ADAMIREZ */
	&rs_object[281], /* ADFORMAT */
#endif
};





TEDINFO desk_rs_tedinfo[RS_NTED];
OBJECT desk_rs_obj[RS_NOBS];

void desktop_rs_init(void)
{
#if RS_NOBS != 0
    /* Copy data from ROM to RAM: */
    memcpy(desk_rs_obj, rs_object_rom, RS_NOBS*sizeof(OBJECT));
    memcpy(desk_rs_tedinfo, rs_tedinfo_rom, RS_NTED*sizeof(TEDINFO));
#endid
}
