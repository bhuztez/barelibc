#include <string.h>
#include <barelibc/stdio.h>

#define MIN(a,b) ((a)<(b) ? (a) : (b))

char *fgets(char *restrict s, int n, FILE *restrict f)
{
	char *p = s;
	unsigned char *z;
	size_t k;
	int c;

	if (n--<=1) {
		if (n) return 0;
		*s = 0;
		return s;
	}

	while (n) {
		z = memchr(f->pos, '\n', f->end - f->pos);
		k = z ? z - f->pos + 1 : f->end - f->pos;
		k = MIN(k, n);
		memcpy(p, f->pos, k);
		f->pos += k;
		p += k;
		n -= k;
		if (z || !n) break;
		if ((c = fgetc(f)) < 0) {
			if (p==s || !feof(f)) s = 0;
			break;
		}
		n--;
		if ((*p++ = c) == '\n') break;
	}
	if (s) *p = 0;

	return s;
}
