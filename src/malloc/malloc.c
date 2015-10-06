#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <barelibc/malloc.h>


#define ROUND_UP(a,b)  (((uintptr_t)(a)+(uintptr_t)(b)-1) & (~((uintptr_t)(b)-1)))
#define ROUND_DOWN(a,b) ((uintptr_t)(a) & (~((uintptr_t)(b)-1)))

struct block_header {
  unsigned int prev_size : 24;
  unsigned int size      : 24;
  unsigned int free      : 1;
  unsigned int marked    : 1;
  unsigned int tag       : 14;
} __attribute__((packed));

struct free_block {
  struct block_header header;
  struct free_block *prev_block;
  struct free_block *next_block;
};

/* fli number of words
 *   2 16 +0 +2 +4 +6 +8 +10 +12 +14
 *   1  8 +0 +1 +2 +3 +4  +5  +6  +7
 *   0  0 +0 +1 +2 +3 +4  +5  +6  +7
 */

struct memory_pool {
  unsigned int pages     : 32;
  unsigned int fl_bitmap : 32;
  unsigned char sl_bitmap[24];
  struct free_block *free_block_list[24][8];
  struct block_header *last_block;
  size_t free_size;
  size_t free_blocks;
  size_t used_blocks;
} __attribute__((packed));


static struct memory_pool mempool = {0};

/* flsl1(0) -> 0
 * flsl1(1) -> 1
 * flsl1(2) -> 2
 * flsl1(3) -> 1
 * flsl1(4) -> 3
 */
static int flsl1(unsigned long x) {
  if (x) {
    return 8 * sizeof(unsigned long) - __builtin_clzl(x);
  } else {
    return 0;
  }
}

static void mapping_insert(size_t nwords, int *fl, int *sl) {
  int fli = flsl1(nwords >> 3);
  int sli = nwords;
  if (fli) {
    sli >>= fli - 1;
  }

  *fl = fli;
  *sl = sli & 0x07;
}

static size_t mapping_roundup(size_t nwords) {
  int fli = flsl1(nwords >> 3);
  if (!fli)
    return nwords;
  return ROUND_UP(nwords, 1 << (fli-1));
}

static void mapping_search(size_t nwords, int *fl, int *sl) {
  nwords = mapping_roundup(nwords);
  int fli = flsl1(nwords >> 3);
  int sli = nwords;
  if (fli) {
    sli >>= fli - 1;
  }

  *fl = fli;
  *sl = sli & 0x07;
}

static struct free_block *find_suitable_block(size_t nwords) {
  int fli, sli;
  mapping_search(nwords, &fli, &sli);

  if (mempool.fl_bitmap & (1 << fli)) {
    int slb = flsl1(mempool.sl_bitmap[fli] >> sli);
    if (slb) {
      return mempool.free_block_list[fli][sli+slb-1];
    }
  }

  int flb = flsl1(mempool.fl_bitmap >> (fli + 1));
  if (!flb)
    return NULL;

  fli += flb;
  int slb = flsl1(mempool.sl_bitmap[fli]);
  return mempool.free_block_list[fli][slb-1];
}

static void insert_block(struct free_block *block) {
  block->header.free = 1;
  block->prev_block = NULL;
  mempool.free_size += block->header.size;
  mempool.free_blocks += 1;

  int fli, sli;
  mapping_insert(block->header.size, &fli, &sli);
  block->next_block = mempool.free_block_list[fli][sli];

  if (block->next_block) {
    block->next_block->prev_block = block;
  }
  mempool.free_block_list[fli][sli] = block;
  mempool.sl_bitmap[fli] |= 1 << sli;
  mempool.fl_bitmap |= 1 << fli;
}

static void remove_block(struct free_block *block) {
  block->header.free = 0;
  mempool.free_size -= block->header.size;
  mempool.free_blocks -= 1;

  if (block->next_block) {
    block->next_block->prev_block = block->prev_block;
  }

  if (block->prev_block) {
    block->prev_block->next_block = block->next_block;
  } else {
    int fli, sli;
    mapping_insert(block->header.size, &fli, &sli);
    mempool.free_block_list[fli][sli] = block->next_block;

    if (!block->next_block) {
      mempool.sl_bitmap[fli] &= ~(1 << sli);
      if (!mempool.sl_bitmap[fli])
        mempool.fl_bitmap &= ~(1 << fli);
    }
  }
}

static struct block_header *split_block(struct block_header *block, size_t nwords) {
  size_t orig_size = block->size;
  block->size = nwords;
  struct block_header *remain_block = (struct block_header *)((uintptr_t *)(block+1)+nwords);
  remain_block->prev_size = nwords;
  remain_block->size = orig_size - nwords - sizeof(struct block_header)/sizeof(uintptr_t);

  if (block == mempool.last_block)
    mempool.last_block = remain_block;

  return remain_block;
}

static struct block_header *get_blocks_begin() {
  return (struct block_header *)MMAP_BASE;
}

static struct block_header *get_blocks_end() {
  uintptr_t header = MMAP_BASE + PAGE_SIZE * mempool.pages;
  return (struct block_header *)header;
}

static struct block_header *find_prev_block(struct block_header *header) {
  return (struct block_header *)((uintptr_t *)header - header->prev_size) - 1;

}

static struct block_header *find_next_block(struct block_header *header) {
  return (struct block_header *)((uintptr_t *)(header+1) + header->size);
}

static struct block_header *merge_left_block(struct block_header *block) {
  struct block_header *left_block = find_prev_block(block);
  if ((left_block < get_blocks_begin()) || (!(left_block->free)))
    return block;

  remove_block((struct free_block *)left_block);
  left_block->size += block->size + sizeof(struct block_header)/sizeof(uintptr_t);

  if (block == mempool.last_block)
    mempool.last_block = left_block;

  return left_block;
}

static void merge_right_block(struct block_header *block) {
  struct block_header *right_block = find_next_block(block);
  if ((right_block >= get_blocks_end()) || (!(right_block->free)))
    return;

  remove_block((struct free_block *)right_block);
  block->size += right_block->size + sizeof(struct block_header)/sizeof(uintptr_t);

  if (right_block == mempool.last_block)
    mempool.last_block = block;

  return;
}

static struct block_header *merge_block(struct block_header *block) {
  block = merge_left_block(block);
  merge_right_block(block);
  return block;
}

static void free_after(struct block_header *block, size_t nwords) {
  if (sizeof(uintptr_t) * (block->size - nwords) > sizeof(struct free_block))
    insert_block((struct free_block *)split_block(block, nwords));
}

void __init_mempool(unsigned int pages) {
  struct block_header *header = (struct block_header *)MMAP_BASE;
  struct free_block *block = (struct free_block *)header;
  mempool.pages = pages;
  mempool.last_block = header;
  header->size = (PAGE_SIZE * pages - sizeof(struct block_header))/sizeof(uintptr_t);
  insert_block(block);
}

void *calloc(size_t nmemb, size_t size) {
  return malloc(nmemb * size);
}

void free(void *ptr) {
  if (!ptr) return;
  struct block_header *header = merge_block((struct block_header *)ptr - 1);
  insert_block((struct free_block *)header);
  mempool.used_blocks -= 1;
}

void *malloc(size_t size) {
  if (size == 0)
    return NULL;

  size_t nwords = ROUND_UP(size, sizeof(uintptr_t))/sizeof(uintptr_t);
  struct free_block *free_block = find_suitable_block(nwords);

  if (!free_block)
    return NULL;

  free_after(&(free_block->header), nwords);
  free_block->header.marked = 0;
  free_block->header.tag = 0;
  mempool.used_blocks += 1;
  return (void *)(&(free_block->header)+1);
}

void *realloc(void *ptr, size_t size) {
  if (!ptr)
    return malloc(size);

  if (size == 0) {
    free(ptr);
    return NULL;
  }

  struct block_header *header = (struct block_header *)ptr - 1;
  size_t nwords = ROUND_UP(size, sizeof(uintptr_t))/sizeof(uintptr_t);

  if (header != mempool.last_block)
    merge_right_block(header);

  if (nwords <= header->size) {
    free_after(header, nwords);
    return ptr;
  }

  void *newptr = malloc(size);
  if (!newptr)
    return NULL;

  memcpy(newptr, ptr, sizeof(uintptr_t) * header->size);
  free(ptr);
  return newptr;
}
