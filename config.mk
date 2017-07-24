#
# Tools
#

AM_V_RC = $(am__v_RC_@AM_V@)
am__v_RC_ = $(am__v_RC_@AM_DEFAULT_V@)
am__v_RC_0 = @echo "  RC      " $@;
am__v_RC_1 = 

AM_V_GEN = $(am__v_GEN_@AM_V@)
am__v_GEN_ = $(am__v_GEN_@AM_DEFAULT_V@)
am__v_GEN_0 = @echo "  GEN     " $@;
am__v_GEN_1 = 

AM_V_UDO = $(am__v_UDO_@AM_V@)
am__v_UDO_ = $(am__v_UDO_@AM_DEFAULT_V@)
am__v_UDO_0 = @echo "  UDO      $< -o $@";
am__v_UDO_1 = 
AM_Q_UDO = $(am__q_UDO_@AM_V@)
am__q_UDO_ = $(am__q_UDO_@AM_DEFAULT_V@)
am__q_UDO_0 = --quiet
am__q_UDO_1 = 

CP = cp -p
MV = mv
MKDIR_P = mkdir -p
CDPATH= 

# windows resource compiler
RC_DEFINES=	`echo -D__WIN32__ -DRC_INVOKED $(AM_CPPFLAGS) $(CPPFLAGS) $(DEFINES) $(INCLUDES) | sed -e 's/-D/--define /g' -e 's/-I/--include-dir /g'`
WINDRES=	$(AM_V_RC)$(RC) $(RC_DEFINES) $(RC_INCLUDES) --input-format rc --output-format coff -o $@ $<
# OS/2 resource compiler
OS2RC=		$(AM_V_RC)$(RC) -r $(AM_CPPFLAGS) $(CPPFLAGS) $(DEFINES) $< $@

# old Windows3 help compiler; obsolete since it is a 16bit program
# and can't be run on newer OS
#HC31=		hcp
# Windows4 help compiler
#HCRTF=		hcrtf
# Windows htmlhelp compiler
#HHC=		hhc
# TOS Hyp compiler
#HCP=		hcp
# OS/2 RTF to IPF converter
#RTF2IPF=	rtf2ipf$(EXEEXT)
# OS/2 IPF compiler
#IPFC=		ipfc$(EXEEXT)

PNG_CFLAGS = 
PNG_LIBS   = -lpng
Z_CFLAGS   = 
Z_LIBS     = -lz

#
# Common defines/libraries
#

CC_DEFINES=

DEFINES=	$(CC_DEFINES)

if OS_WIN32
DEFINES+=	-DSTRICT=1 \
		-DUSE_WINDOWS_RESOURCE=1
endif

AM_CPPFLAGS =	-I$(top_srcdir)/include

GUI_CPPFLAGS=	-DCONSOLE_VERSION=0 -DGUI_VERSION=1
CON_CPPFLAGS=	-DCONSOLE_VERSION=1 -DGUI_VERSION=0


#ifdef HPArchitecture
#X_EXTRA_LIBS +=	/usr/contrib/X11R6/lib/libXmu.a
#X_EXTRA_LIBS +=	/usr/contrib/X11R6/lib/libXaw.a
#AM_CPPFLAGS +=		-I/usr/contrib/X11R6/include
#endif

#
# Directories
#

if OS_TOS
TTP = .ttp
PRG = .prg
TXT = .txt
ARCHIVE_TAG = -mint
endif

if OS_WIN32
ARCHIVE_TAG = -win32
TXT = .txt
endif

if OS_UNIX
ARCHIVE_TAG = -linux
endif

if OS_OS2
ARCHIVE_TAG = -os2
TXT = .txt
endif

if OS_WIN32
CHMODX = /c/cygwin/bin/chmod 755
else
CHMODX = chmod 755
endif

CLEANFILES =

DISTCLEANFILES =

EXTRA_DIST =
