#ifndef NANOVM_INTERNAL_H
#define NANOVM_INTERNAL_H 1

#include "nanovm.h"

#include "util.h"
#include "code.h"
#include "memory.h"
#include "stack.h"
#include "native.h"
#include "thread.h"

typedef struct _nanovm_ {
    nvm_code_mgr_t* code_mgr;
   nvm_stack_mgr_t* stack_mgr;
     nvm_mem_mgr_t* mem_mgr;
  nvm_thread_mgr_t* thread_mgr;
  nvm_native_mgr_t* native_mgr;
} nanovm_t;

#endif
