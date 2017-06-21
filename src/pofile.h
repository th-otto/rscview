#ifndef __POFILE_H__
#define __POFILE_H__ 1

void po_init(const char *po_dir, int mustexist);
void po_exit(void);

int po_create_hash(const char *lang, nls_domain *domain, const char *po_dir);
void po_delete_hash(nls_domain *domain);
int po_get_charset_id(const char *name);

#endif /* __POFILE_H__ */
