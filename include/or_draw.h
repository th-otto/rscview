typedef enum
{
	POPUP_NONE,
	POPUP_ORCS,
	POPUP_HONKA,
	POPUP_HONKA_BOX,
	POPUP_ORCS_SIMPLE,
	POPUP_ORCS_SINGLE,
	POPUP_ORCS_MULTIPLE,
	POPUP_ORCS_BOX
} popup_type;


void GetTextSize(_WORD *width, _WORD *height);
_WORD GetNumColors(void);
_WORD GetNumPlanes(void);
void GetScreenSize(_WORD *width, _WORD *height);

void TO_draw_obj(
	_WORD x, _WORD y, _WORD w, _WORD h,
	_UWORD type,
	OBSPEC obspec,
	_UWORD flags, _UWORD state,
	_BOOL for_menu,
	_BOOL is_root
);

extern _WORD od_handle;
extern EXTOB_MODE objc_mode;
extern _BOOL op_draw3d;
extern _BOOL od_clear;
extern _UWORD od_oldstate;
extern PARMBLKFUNC od_code;
extern PARMBLK od_parm;
extern GRECT od_pxy;
extern GRECT od_framerec;
extern GRECT od_outrec;
extern _WORD color_background;
extern _WORD color_activator;
extern _WORD od_ch;

void od_setfont(_WORD handle, _WORD size, _WORD color, _UWORD style);
void od_box(_WORD advance, _BOOL draw);
_BOOL color_3d(_UWORD flags, _UWORD state, _WORD *color, _WORD *pattern);
void shadow(_WORD framesize, _WORD framecol);
void draw_underline_char(_WORD x, _WORD y, _WORD color, const char *txt, _WORD offset);
void obj_draw_area(OBJECT *tree, _WORD start, _WORD depth, _BOOL for_menu);
void draw_text(char *txt, GRECT *gr, _WORD size, _WORD centered, _BOOL centver, _WORD textcol, _WORD textmode, _WORD offset, _WORD bold);
_UBYTE *calc_text(const char *ptmplt, _WORD tmplen, _UBYTE *ptext, _WORD txtlen, const char *pvalid, _BOOL reset, _UBYTE fillchar, _WORD max_cur_pos);

_WORD hdraw_button(_WORD state, _WORD flags, _WORD x, _WORD y, _WORD w, _WORD h, char *str, _BOOL extBut, _WORD offset, _WORD framesize);
_WORD hdraw_popup(_WORD type, _WORD x, _WORD y, _WORD w, _WORD h,
	OBSPEC obspec, _UWORD flags, _UWORD state, GRECT *gr);
void hdraw_popup_size(_WORD handle, _WORD x, _WORD y, _WORD w, _WORD h,
	OBSPEC obspec, _UWORD flags, _UWORD state, GRECT *gr);
_WORD hdraw_title(_WORD state, _WORD x, _WORD y, _WORD w, _WORD h, const char *titleStr);
_WORD hdraw_list(_WORD x, _WORD y, _WORD w, _WORD h,
	OBSPEC obspec, _UWORD flags, _UWORD state, GRECT *gr);

_WORD get_popup_selected(OBJECT *tree, _WORD head, _WORD *valp);

_BOOL objc_draw_init(void);
void objc_draw_exit(void);

void sysgem_init(void);
void sysgem_exit(void);

void sysgem_draw_button(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_radio(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_select(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_help(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_notebook(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_box(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, _BOOL is_root);
void sysgem_draw_boxchar(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state);
void sysgem_draw_frame(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, const char *text, _UWORD flags, _UWORD state);
void sysgem_draw_string(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_text(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state);
void sysgem_draw_mentry(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, char *text, _WORD shortcut);
void sysgem_draw_mbox(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state);
void sysgem_draw_edit(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state);
void sysgem_draw_listbox(_WORD x, _WORD y, _WORD w, _WORD h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state);
void sysgem_draw_bar(_WORD pb_x, _WORD pb_y, _WORD pb_w, _WORD pb_h, _UWORD type, OBSPEC obspec, _UWORD flags, _UWORD state, _WORD next_w, _WORD next_h, OBSPEC next_obspec);

void ob_draw_dialog(_WORD vdi_handle, OBJECT *tree, _WORD start, _WORD depth, const GRECT *gr, EXTOB_MODE mode);
void ob_draw_alert(_WORD vdi_handle, _WORD defbutton, const char *str, const GRECT *gr, EXTOB_MODE mode);
