#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <barelibc/stdio.h>


size_t fwrite(const void * restrict ptr, size_t size, size_t nmemb, FILE * restrict stream) {
  if (!stream->write) {
    errno = EBADF;
    return -1;
  }

  size_t n = size * nmemb;
  if (stream->pos + n > stream->end) {
    return stream->write(stream, ptr, size*nmemb)/size;
  }

  memcpy(stream->pos, ptr, n);
  stream->pos += n;
  return nmemb;
}
