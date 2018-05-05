#ifndef NANOVM_STACK_H
#define NANOVM_STACK_H 1

#include "util.h"
#include "nanovm.h"
#include "code.h"
#include "memory.h"

typedef struct _stack_mgr_ stack_mgr_t;
typedef struct _stack_     stack_t;
typedef struct _result_    result_t;
typedef struct _frame_     frame_t;

typedef struct _stack_mgr_ {
  nanovm_t* vm;
        int frame_cap;
} stack_mgr_t;

typedef struct _stack_ {
        int frame_cap;
        int frame_len;
  frame_t** frames;
} stack_t;

typedef struct _result_ {
  object_t* ret; // method return value
  object_t* ex;  // method throw exception
} result_t;

typedef struct _frame_ {
    stack_t* stack; // in this stack

     meth_t* meth;
     insn_t* cur; // current instruction

         int op_stack_cap;
  object_t** op_stack;
         int local_var_cap;
  object_t** local_vars;
    result_t result;
} frame_t;

         int NanoVM_GLOBAL_init_stack();
        void NanoVM_GLOBAL_free_stack();
stack_mgr_t* NanoVM_create_stack_mgr (nanovm_t* vm, int frame_cap);
        void NanoVM_release_stack_mgr(stack_mgr_t* stack_mgr);

stack_t* NanoVM_create_stack (nanovm_t* vm);
    void NanoVM_release_stack(stack_t* stack);

frame_t* NanoVM_create_frame(stack_t* stack, meth_t* meth, int local_var_len, object_t** local_vars);
frame_t* NanoVM_frame_pop   (stack_t* stack);
     int NanoVM_frame_start (frame_t* frame, result_t* result);

object_t* NanoVM_op_pop (frame_t* frame);
      int NanoVM_op_push(frame_t* frame, object_t* var);

object_t* NanoVM_get_local(frame_t* frame, int index);
      int NanoVM_set_local(frame_t* frame, int index, object_t* var);
      int NanoVM_rm_local (frame_t* frame, int index);

#endif
