#include <unistd.h>
#include <string.h>

#include <barelibc/stdio.h>


static size_t stdin_read(FILE * file, unsigned char * s, size_t n) {
  size_t count = 0;

  if (file->pos + n <= file->end) {
    memcpy(s, file->pos, n);
    file->pos += n;
    return n;
  }

  count = file->end - file->pos;
  memcpy(s, file->pos, count);
  s += count;
  file->pos = file->buf;

  count += read(0, s, n-count);

  size_t size = read(0, file->buf, BUFSIZ);
  file->end = file->buf + size;
  if (size != BUFSIZ) {
    file->eof = 1;
  }

  return count;
}


static unsigned char __stdin_buf[BUFSIZ];

static FILE __stdin = {
  .read = stdin_read,
  .write = NULL,
  .buf = __stdin_buf,
  .pos = __stdin_buf,
  .end = __stdin_buf,
  .eof = 0,
};

FILE *stdin = &__stdin;
