#include <barelibc/libc.h>
#include <barelibc/stdio.h>

int fputc(int c, FILE * stream) {
  unsigned char s = c;

  if (stream->pos + 1 > stream->end) {
    if (stream->write(stream, &s, 1) != 1)
      return EOF;
    return c;
  }

  *(stream->pos) = s;
  stream->pos += 1;
  return c;
}

weak_alias(fputc, _IO_putc);
weak_alias(fputc, putc);
