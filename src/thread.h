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

#define THREAD_STATE_RUN  1
#define THREAD_STATE_STOP 3
// STOP = (10 | 01), for that it's still running

typedef struct _thread_mgr_ thread_mgr_t;
typedef struct _thread_ nthread_t;

typedef struct _thread_mgr_ {
    nanovm_t* vm;
       lock_t lock;
          int thread_cap;
  nthread_t** threads;
} thread_mgr_t;

typedef struct _thread_ {
  thread_mgr_t* thread_mgr;
            int idx; // index in thread_mgr
         lock_t lock;
           char state;
      pthread_t pthread;
            int tid; // generated random id, not the os tid
       stack_t* stack;
} nthread_t;

thread_mgr_t* NanoVM_create_thread_mgr (nanovm_t* vm);
         void NanoVM_release_thread_mgr(thread_mgr_t* thread_mgr);

nthread_t* NanoVM_create_thread (nanovm_t* vm);
       int NanoVM_start_thread  (nthread_t* thread);
       int NanoVM_stop_thread   (nthread_t* thread); // only mark `stop`
      void NanoVM_release_thread(nthread_t* thread);

#endif
