#ifndef NANOVM_UTIL_H
#define NANOVM_UTIL_H 1

// base headers
#include <stddef.h>
#include "zmalloc.h"

typedef struct _nanovm_ nanovm_t;
typedef struct {
  nanovm_t* vm;
        int tid;
} nvm_ctx_t;

typedef struct {
  int tid; // it's the nanovm thread id, not os tid
  int cnt;
} nvm_lock_t;

typedef struct nvm_node_t{
  void* addr;
  struct nvm_node_t* next;
} nvm_node_t;

 int NanoVM_init_lock(nvm_lock_t* lock);
 int NanoVM_try_lock (nvm_lock_t* lock, int tid);
void NanoVM_lock     (nvm_lock_t* lock, int tid);
 int NanoVM_unlock   (nvm_lock_t* lock, int tid);

int NanoVM_rand_int(int max);

void NanoVM_write_log(int e, const char *fmt, ...);

nvm_node_t* NanoVM_init_linkedlist();

int NanoVM_ins_node(nvm_node_t* head, void* addr);

void* malloc_ref(nvm_node_t* head, size_t size);

void free_ref(nvm_node_t* head);

void NanoVM_del_all(nvm_node_t* head);
#define NanoVM_debug_log0(fmt) NanoVM_write_log(0, fmt)
#define NanoVM_debug_log1(fmt, a) NanoVM_write_log(0, fmt, a)
#define NanoVM_error_log0(fmt) NanoVM_write_log(1, fmt)
#define NanoVM_error_log1(fmt, a) NanoVM_write_log(1, fmt, a)

#endif
