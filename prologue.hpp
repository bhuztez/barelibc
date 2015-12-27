extern "C" {
  static int __syscall (int, ...);
};

#include <stdlib.h>

void* operator new   (size_t len) {return malloc(len);}
void* operator new[] (size_t len) {return malloc(len);}
void  operator delete   (void* p) {free(p);}
void  operator delete[] (void* p) {free(p);}
