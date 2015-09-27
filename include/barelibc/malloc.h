#ifndef __BARELIBC_MALLOC_H__
#define __BARELIBC_MALLOC_H__

#define MMAP_BASE 0x10000000
#define PAGE_SIZE 4096

void __init_mempool();

#endif /* __BARELIBC_MALLOC_H__ */
