void GetTextSize(_WORD *width, _WORD *height);
_WORD GetNumColors(void);

void ob_draw_dialog(_WORD vdi_handle, OBJECT *tree, _WORD start, _WORD depth, GRECT *gr, EXTOB_MODE mode);
void ob_draw_alert(_WORD vdi_handle, _WORD defbutton, const char *str, GRECT *gr, EXTOB_MODE mode);
