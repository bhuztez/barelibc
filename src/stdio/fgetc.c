#include <barelibc/libc.h>
#include <barelibc/stdio.h>


int fgetc(FILE * stream) {
  unsigned char c;

  if (stream->read(stream, &c, 1) == 1)
    return c;

  return EOF;
}


weak_alias(fgetc, _IO_getc);
weak_alias(fgetc, getc);
