#include <unistd.h> 
#include <barelibc/stdio.h>


static size_t stdout_write(FILE * file, const unsigned char * s, size_t n) {
  size_t count = 0;

  if (file->pos) {
    count += write(1, file->buf, file->pos - file->buf);
    file->pos = file->buf;
  }

  if (s) {
    count += write(1, s, n);
  }

  return count;
}

static unsigned char __stdout_buf[BUFSIZ];

static FILE __stdout = {
  .read = NULL,
  .write = stdout_write,
  .buf = __stdout_buf,
  .pos = __stdout_buf,
  .end = __stdout_buf + BUFSIZ,
  .eof = 0,
};

FILE *stdout = &__stdout;
