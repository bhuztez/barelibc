#ifndef __BARELIBC_STDIO_H__
#define __BARELIBC_STDIO_H__

#include <unistd.h>
#include <stdbool.h>
#include <stdarg.h>

#define EOF (-1)

#define BUFSIZ 1024
#define _IOFBF 0
#define _IONBF 2


typedef struct _IO_FILE FILE;

struct _IO_FILE {
  size_t (*read)(FILE *, unsigned char *, size_t);
  size_t (*write)(FILE *, const unsigned char *, size_t);
  unsigned char *buf;
  unsigned char *pos;
  unsigned char *end;
  bool eof;
  unsigned char *shend;
  off_t shlim, shcnt;
};

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

int feof(FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
char* fgets(char *s, int n, FILE *stream);
int fputs(const char *s, FILE *stream);
int fflush(FILE *stream);
size_t fwrite(const void * restrict ptr, size_t size, size_t nmemb, FILE * restrict stream);
int setvbuf(FILE * restrict stream, unsigned char * restrict buf, int mode, size_t size);
int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap);
int vscanf(const char *format, va_list ap);
int vfscanf(FILE *stream, const char *format, va_list ap);
int vsscanf(const char *str, const char *format, va_list ap);

#endif /* __BARELIBC_STDIO_H__ */
