#include <sysexits.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <stdint.h>

#include <barelibc/stdio.h>
#include <barelibc/malloc.h>
#include <barelibc/syscall.h>

#define PAGES 2048

extern void (*const __init_array_start)() __attribute__((weak));
extern void (*const __init_array_end)() __attribute__((weak));
extern void (*const __fini_array_start)() __attribute__((weak));
extern void (*const __fini_array_end)() __attribute__((weak));

static _Noreturn void __exit(int ec) {
  for (;;) __syscall(SYS_exit, ec);
}

_Noreturn void exit(int ec) {
  uintptr_t a = (uintptr_t)&__fini_array_end;
  for (; a>(uintptr_t)&__fini_array_start; a-=sizeof(void(*)()))
    (*(void (**)())(a-sizeof(void(*)())))();

  fflush(NULL);
  __exit(ec);
}

int *__errno_location(void) {
  static int errno_val = 0;
  return &errno_val;
}

_Noreturn void __libc_start_main(int (*main)(int,char **,char **), int argc, char **argv) {
  char **envp = argv+argc+1;

  if(!__syscall(SYS_mmap, MMAP_BASE, PAGE_SIZE * PAGES, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0))
    __exit(EX_OSERR);

  if (__syscall(SYS_prctl, PR_SET_SECCOMP, 1))
    __exit(EX_OSERR);

  __init_mempool(PAGES);

  uintptr_t a = (uintptr_t)&__init_array_start;
  for (; a<(uintptr_t)&__init_array_end; a+=sizeof(void(*)()))
    (*(void (**)())a)();

  exit(main(argc, argv, envp));
}
