#ifndef __BARELIBC_SHGETC_H__
#define __BARELIBC_SHGETC_H__

#include <barelibc/stdio.h>

void __shlim(FILE *, off_t);
int __shgetc(FILE *);

#define shcnt(f) ((f)->shcnt + ((f)->pos - (f)->end))
#define shlim(f, lim) __shlim((f), (lim))
#define shgetc(f) (((f)->pos < (f)->shend) ? *(f)->pos++ : __shgetc(f))
#define shunget(f) ((f)->shend ? (void)(f)->pos-- : (void)0)

#endif /* __BARELIBC_SHGETC_H__ */
