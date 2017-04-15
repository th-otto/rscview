#ifndef __POFILE_H__
#define __POFILE_H__ 1

int po_create_hash(const char *lang, nls_domain *domain);
void po_delete_hash(nls_domain *domain);

#endif /* __POFILE_H__ */
