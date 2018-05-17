#ifndef NANOVM_STACK_H
#define NANOVM_STACK_H 1

#include "util.h"
#include "nanovm.h"
#include "code.h"
#include "memory.h"

typedef struct _nvm_stack_mgr_ nvm_stack_mgr_t;
typedef struct _nvm_stack_     nvm_stack_t;
typedef struct _nvm_result_    nvm_result_t;
typedef struct _nvm_frame_     nvm_frame_t;

typedef struct _nvm_stack_mgr_ {
  int frame_cap;
} nvm_stack_mgr_t;

typedef struct _nvm_stack_ {
            int frame_cap;
            int frame_len;
  nvm_frame_t** frames;
} nvm_stack_t;

typedef struct _nvm_result_ {
  nvm_object_t* ret; // method return value
  nvm_object_t* ex;  // method throw exception
} result_t;

typedef struct _nvm_frame_ {
    nvm_stack_t* stack; // in this stack

     nvm_meth_t* meth;
     nvm_insn_t* cur; // current instruction

             int op_stack_cap;
  nvm_object_t** op_stack;
             int local_var_cap;
  nvm_object_t** local_vars;
    nvm_result_t result;
} nvm_frame_t;

             int NanoVM_GLOBAL_init_stack();
            void NanoVM_GLOBAL_free_stack();
nvm_stack_mgr_t* NanoVM_create_stack_mgr (nvm_ctx_t* ctx, int frame_cap);
            void NanoVM_release_stack_mgr(nvm_ctx_t* ctx, nvm_stack_mgr_t* stack_mgr);

nvm_stack_t* NanoVM_create_stack (nvm_ctx_t* ctx);
        void NanoVM_release_stack(nvm_ctx_t* ctx, nvm_stack_t* stack);

nvm_frame_t* NanoVM_create_frame(nvm_ctx_t* ctx, nvm_stack_t* stack, nvm_meth_t* meth, int local_var_len, nvm_object_t** local_vars);
nvm_frame_t* NanoVM_frame_pop   (nvm_ctx_t* ctx, nvm_stack_t* stack);
         int NanoVM_frame_start (nvm_ctx_t* ctx, nvm_frame_t* frame, nvm_result_t* result);

nvm_object_t* NanoVM_op_pop (nvm_ctx_t* ctx, nvm_frame_t* frame);
          int NanoVM_op_push(nvm_ctx_t* ctx, nvm_frame_t* frame, nvm_object_t* var);

nvm_object_t* NanoVM_get_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index);
          int NanoVM_set_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index, nvm_object_t* var);
          int NanoVM_rm_local (nvm_ctx_t* ctx, nvm_frame_t* frame, int index);

#endif
