#include <barelibc/shgetc.h>

int __uflow(FILE *f)
{
	unsigned char c;
	if (f->read(f, &c, 1)==1) return c;
	return EOF;
}

void __shlim(FILE *f, off_t lim)
{
	f->shlim = lim;
	f->shcnt = f->end - f->pos;
	if (lim && f->shcnt > lim)
		f->shend = f->pos + lim;
	else
		f->shend = f->end;
}

int __shgetc(FILE *f)
{
	int c;
	if (f->shlim && f->shcnt >= f->shlim || (c=__uflow(f)) < 0) {
		f->shend = 0;
		return EOF;
	}
	if (f->shlim && f->end - f->pos > f->shlim - f->shcnt - 1)
		f->shend = f->pos + (f->shlim - f->shcnt - 1);
	else
		f->shend = f->end;
	if (f->end) f->shcnt += f->end - f->pos + 1;
	if (f->pos[-1] != c) f->pos[-1] = c;
	return c;
}
