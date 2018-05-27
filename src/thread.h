#ifndef NANOVM_THREAD_H
#define NANOVM_THREAD_H 1

#include <pthread.h>

#include "nanovm.h"
#include "util.h"
#include "code.h"
#include "stack.h"

// init means the thread is created
// run  means the thread is running
// intr means the thread will be interrupted after the current insn finishes
// dead means the thread has finished
#define NVM_THREAD_STATE_INIT 0
#define NVM_THREAD_STATE_RUN  1
#define NVM_THREAD_STATE_INTR 2
#define NVM_THREAD_STATE_DEAD 3

#define NVM_MAX_TID 0xffff

typedef struct _nvm_thread_mgr_ nvm_thread_mgr_t;
typedef struct _nvm_thread_ nvm_thread_t;

typedef struct _nvm_thread_mgr_ {
      nvm_lock_t lock;
             int thread_cap;
  nvm_thread_t** threads;
} nvm_thread_mgr_t;

typedef struct _nvm_thread_ {
            int idx; // index in thread_mgr
     nvm_lock_t lock;
           char state;
      pthread_t pthread;
            int tid; // generated random id, not the os tid
   nvm_stack_t* stack;
  nvm_object_t* runnable;
} nvm_thread_t;

nvm_thread_mgr_t* NanoVM_create_thread_mgr (nvm_ctx_t* ctx, int thread_cap);
              int NanoVM_release_thread_mgr(nvm_ctx_t* ctx); // may fail if thread still running

nvm_thread_t* NanoVM_create_thread (nvm_ctx_t* ctx, nvm_object_t* runnable);
          int NanoVM_start_thread  (nvm_ctx_t* ctx, nvm_thread_t* thread);
          int NanoVM_intr_thread   (nvm_ctx_t* ctx, nvm_thread_t* thread); // return -1 when it's stopped, return 0 when marked as `intr`
          int NanoVM_release_thread(nvm_ctx_t* ctx, nvm_thread_t* thread); // return 0 when it's not running

#endif
