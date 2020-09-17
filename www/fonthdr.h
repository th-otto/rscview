/*
 * fonthdr.h - the Font Header
 *
 * This file exists to centralise the definition of the font header,
 * which was previously defined in two different places.
 *
 * Copyright (C) 2015-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FONTHDR_H
#define FONTHDR_H

#if defined(__PUREC__) && !defined(uint16_t)
#define uint16_t unsigned short
#define int16_t short
#else
#include <stdint.h>
#endif

/* font header flags */

#define F_DEFAULT   1   /* this is the default font (face and size) */
#define F_HORZ_OFF  2   /* there is a horizontal offset table */
#define F_STDFORM   4   /* the font is in standard (Motorola) format */
#define F_MONOSPACE 8   /* the font is monospaced */

#define F_NO_CHAR 0xFFFFu

/* the font header describes a font */

#define FONT_NAME_LEN 32

typedef struct font_head Fonthead;
struct font_head {
    int16_t font_id;
    int16_t point;
    char name[FONT_NAME_LEN];
    uint16_t first_ade;
    uint16_t last_ade;
    uint16_t top;
    uint16_t ascent;
    uint16_t half;
    uint16_t descent;
    uint16_t bottom;
    uint16_t max_char_width;
    uint16_t max_cell_width;
    uint16_t left_offset;          /* amount character slants left when skewed */
    uint16_t right_offset;         /* amount character slants right */
    uint16_t thicken;              /* number of pixels to smear when bolding */
    uint16_t ul_size;              /* height of the underline */
    uint16_t lighten;              /* mask for lightening  */
    uint16_t skew;                 /* mask for skewing */
    uint16_t flags;                /* see above */

    const signed char *hor_table;  /* horizontal offsets */
    const uint16_t *off_table;     /* character offsets  */
    const uint16_t *dat_table;     /* character definitions (raster data) */
    uint16_t form_width;           /* width of raster in bytes */
    uint16_t form_height;          /* height of raster in lines */

    Fonthead *next_font;        /* pointer to next font */
};

extern const Fonthead uni10_font;
extern const Fonthead uni09_font;

#endif /* FONTHDR_H */
