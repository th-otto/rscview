/*
 * font.h - font specific definitions
 *
 * Copyright (C) 2001 Lineo, Inc.
 * Copyright (C) 2004 by Authors (see below)
 * Copyright (C) 2015-2016 The EmuTOS development team
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FONT_H
#define FONT_H

#include <portab.h>

/* font style bits */

#define F_THICKEN 1
#define F_LIGHT 2
#define F_SKEW  4
#define F_UNDER 8
#define F_OUTLINE 16
#define F_SHADOW        32

/* prototypes */

void font_init(void);           /* initialize BIOS font ring */

#endif /* FONT_H */
