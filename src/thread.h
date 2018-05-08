#ifndef NANOVM_THREAD_H
#define NANOVM_THREAD_H 1

#include <pthread.h>

#include "nanovm.h"
#include "util.h"
#include "code.h"
#include "stack.h"

// only 2 states
// run  means the thread is running
// stop means the thread will be terminated after the current insn finishes

#define NVM_THREAD_STATE_RUN  1
#define NVM_THREAD_STATE_STOP 3
// STOP = (10 | 01), for that it's still running

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
} nvm_thread_t;

nvm_thread_mgr_t* NanoVM_create_thread_mgr (nvm_ctx_t* ctx);
             void NanoVM_release_thread_mgr(nvm_ctx_t* ctx, nvm_thread_mgr_t* thread_mgr);

nvm_thread_t* NanoVM_create_thread (nvm_ctx_t* ctx);
          int NanoVM_start_thread  (nvm_ctx_t* ctx, nvm_thread_t* thread);
          int NanoVM_stop_thread   (nvm_ctx_t* ctx, nvm_thread_t* thread); // only mark `stop`
         void NanoVM_release_thread(nvm_ctx_t* ctx, nvm_thread_t* thread);

#endif
