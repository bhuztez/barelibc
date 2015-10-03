#include <unistd.h>

#include <barelibc/syscall.h>

ssize_t write(int fd, const void *buf, size_t count) {
  return __syscall(SYS_write, fd, buf, count);
}
