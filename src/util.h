#ifndef NANOVM_UTIL_H
#define NANOVM_UTIL_H 1

// base headers
#include "zmalloc.h"

typedef struct _nanovm_ nanovm_t;
typedef struct _nvm_thread_ nvm_thread_t;
typedef struct {
      nanovm_t* vm;
  nvm_thread_t* thread;
} nvm_ctx_t;

typedef struct {
  int tid; // it's the nanovm thread id, not os tid
} nvm_lock_t;

 int NanoVM_init_lock(nvm_lock_t* lock);
 int NanoVM_try_lock (nvm_lock_t* lock, int tid);
void NanoVM_lock     (nvm_lock_t* lock, int tid);
 int NanoVM_unlock   (nvm_lock_t* lock, int tid);

int NanoVM_rand_int(int max);

void write_log(int e, const char *fmt, ...);

#define debug_log0(fmt) write_log(0, fmt)
#define debug_log1(fmt, a) write_log(0, fmt, a)
#define error_log0(fmt) write_log(1, fmt)
#define error_log1(fmt, a) write_log(1, fmt, a)

#endif
