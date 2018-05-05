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
    code_mgr_t* code_mgr;
   stack_mgr_t* stack_mgr;
     mem_mgr_t* mem_mgr;
  thread_mgr_t* thread_mgr;
  native_mgr_t* native_mgr;
} nanovm_t;

#endif
