#include <string.h>
#include <barelibc/stdio.h>
#include <barelibc/libc.h>

static size_t do_read(FILE *f, unsigned char *buf, size_t len)
{
	unsigned char *src = f->buf;
	size_t k = len+256;
	unsigned char *end = memchr(src, 0, k);
	if (end) k = end-src;
	if (k < len) len = k;
	memcpy(buf, src, len);
	f->pos = (void *)(src+len);
	f->end = (void *)(src+k);
	f->buf = src+k;
	return len;
}

int vsscanf(const char *restrict s, const char *restrict fmt, va_list ap)
{
	FILE f = {
		.buf = (void *)s,
                .pos = (void *)s,
		.read = do_read
	};
	return vfscanf(&f, fmt, ap);
}

weak_alias(vsscanf,__isoc99_vsscanf);
