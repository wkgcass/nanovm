#include "stack.internal.h"

#include <string.h>
#include "zmalloc.h"
#include "err.h"

nvm_insn_handle_t* insn_handles;

int NanoVM_GLOBAL_init_stack() {
  insn_handles = zmalloc(sizeof(nvm_insn_handle_t) * NVM_INSN_HANDLE_SIZE);
  if (!insn_handles) {
    return -1;
  }
  // TODO init insn_handles
  return 0;
}

void NanoVM_GLOBAL_free_stack() {
  zfree(insn_handles);
  insn_handles = NULL;
}

nvm_stack_mgr_t* NanoVM_create_stack_mgr(nvm_ctx_t* ctx, int frame_cap) {
  nvm_stack_mgr_t* mgr = NanoVM_alloc(ctx, sizeof(nvm_stack_mgr_t));
  if (!mgr) {
    NanoVM_debug_log0("allocate stack manager failed");
    return NULL;
  }
  mgr->frame_cap = frame_cap;
  return mgr;
}

void NanoVM_release_stack_mgr(nvm_ctx_t* ctx) {
  nvm_stack_mgr_t* mgr = ctx->vm->stack_mgr;
  NanoVM_free(ctx, mgr);
}

nvm_stack_t* NanoVM_create_stack(nvm_ctx_t* ctx) {
  nvm_stack_t* stack = NanoVM_alloc(ctx, sizeof(nvm_stack_t));
  if (!stack) {
    NanoVM_debug_log0("allocate stack failed");
    return NULL;
  }
  stack->frame_cap = ctx->vm->stack_mgr->frame_cap;
  stack->frame_len = 0;
  stack->frames = NanoVM_alloc(ctx, sizeof(nvm_frame_t*) * ctx->vm->stack_mgr->frame_cap);
  if (!stack->frames) {
    NanoVM_debug_log0("allocate frames failed");
    NanoVM_free(ctx, stack);
    return NULL;
  }
  return stack;
}

void NanoVM_release_stack(nvm_ctx_t* ctx, nvm_stack_t* stack) {
  if (stack->frame_len) {
    NanoVM_debug_log0("the frame_len is not 0 when releasing the stack");
  }
  while (stack->frame_len) {
    NanoVM_frame_pop(ctx, stack);
  }
  NanoVM_free(ctx, stack->frames);
  NanoVM_free(ctx, stack);
}

nvm_frame_t* NanoVM_create_frame(nvm_ctx_t* ctx, nvm_stack_t* stack, nvm_meth_t* meth, int local_var_len, nvm_object_t** local_vars) {
  if (stack->frame_len == stack->frame_cap) {
    NanoVM_debug_log0("stack over flow: stack");
    errno = NVM_ERR_STK_OVR;
    return NULL;
  }

  nvm_frame_t* frame = NanoVM_alloc(ctx, sizeof(nvm_frame_t));
  if (!frame) {
    NanoVM_debug_log0("allocate frame failed");
    return NULL;
  }
  frame->stack = stack;
  frame->meth = meth;
  frame->op_stack_len = 0;
  frame->result.ex = NULL;
  frame->result.ret = NULL;

  // set these two to NULL, the NULL value would be checked when error occurred
  frame->local_vars = NULL;
  frame->op_stack = NULL;

  if (meth->acc & NVM_ACC_NATIVE) {
    frame->cur = NULL;
    frame->local_var_cap = local_var_len; // store input local var for native module to handle
    frame->local_vars = NanoVM_alloc(ctx, sizeof(nvm_object_t*) * local_var_len);
    if (!frame->local_vars) {
      NanoVM_debug_log1("allocate local variables failed, local_var_len: %d", local_var_len);
      goto err;
    }
    frame->op_stack_cap = 0;
    frame->op_stack = NULL;

    // for native methods, no need to consider wide types
    for (int i = 0; i < local_var_len; ++i) {
      frame->local_vars[i] = local_vars[i];
      NanoVM_ref(ctx, local_vars[i]);
    }
  } else {
    if (meth->insn_len == 0) {
      NanoVM_error_log0("method code attribute should not be empty when it's not native");
      goto err;
    } else {
      frame->cur = meth->insns[0];
    }
    frame->local_var_cap = meth->max_locals;
    size_t local_var_size = sizeof(nvm_object_t*) * meth->max_locals;
    frame->local_vars = NanoVM_alloc(ctx, local_var_size);
    if (!frame->local_vars) {
      NanoVM_debug_log1("allocate local variables failed, max_locals: %d", meth->max_locals);
      goto err;
    }
    memset(frame->local_vars, 0, local_var_size);

    frame->op_stack_cap = meth->max_stack;
    size_t op_stack_size = sizeof(nvm_object_t*) * meth->max_stack;
    frame->op_stack = NanoVM_alloc(ctx, op_stack_size);
    if (!frame->op_stack) {
      NanoVM_debug_log1("allocate op stack failed, max_stack: %d", meth->max_stack);
      goto err;
    }
    memset(frame->op_stack, 0, op_stack_size);

    // set local variables
    int curIdx = 0;
    for (int i = 0; i < local_var_len; ++i) {
      nvm_object_t* o = local_vars[i];
      if (NanoVM_set_local(ctx, frame, curIdx, o) < 0) {
        NanoVM_error_log0("set local variable failed when initiating frame");
        goto err;
      }
      ++curIdx;
      if (o->type->cat == NVM_TYPE_PRM) {
        nvm_prm_type_t* prm_type = (nvm_prm_type_t*)o->type;
        if (prm_type->prm_type == NVM_PRM_TYPE_LONG || prm_type->prm_type == NVM_PRM_TYPE_DOUBLE) {
          ++curIdx; // long and double are wide types
        }
      }
    }
  }

  int idx = stack->frame_len++;
  stack->frames[idx] = frame;
  frame->idx = idx;

  return frame;

err:
  if (!frame) {
    return NULL;
  }
  if (frame->local_vars) {
    NanoVM_free(ctx, frame->local_vars);
  }
  if (frame->op_stack) {
    NanoVM_free(ctx, frame->op_stack);
  }
  NanoVM_free(ctx, frame);
  return NULL;
}

void NanoVM_frame_pop(nvm_ctx_t* ctx, nvm_stack_t* stack) {
  if (stack->frame_len == 0) {
    NanoVM_debug_log0("trying to pop an empty stack");
    return;
  }
  nvm_frame_t* frame = stack->frames[stack->frame_len-1];
  stack->frames[--stack->frame_len] = NULL;

  // unref objects
  for (int i = 0; i < frame->local_var_cap; ++i) {
    nvm_object_t* o = frame->local_vars[i];
    if (o) {
      NanoVM_unref(ctx, o);
    }
  }
  if (frame->op_stack_len) {
    NanoVM_debug_log0("op_stack is not empty when the frame is poped");
  }
  for (int i = 0; i < frame->op_stack_len; ++i) {
    nvm_object_t* o = frame->op_stack[i];
    NanoVM_unref(ctx, o);
  }

  // release the frame
  if (frame->local_vars) {
    NanoVM_free(ctx, frame->local_vars);
  }
  if (frame->op_stack) {
    NanoVM_free(ctx, frame->op_stack);
  }
  NanoVM_free(ctx, frame);
}

int NanoVM_frame_start(nvm_ctx_t* ctx, nvm_frame_t* frame, nvm_result_t* result) {
  // TODO
  NanoVM_debug_log0("NanoVM_frame_start");
  return 0;
}

nvm_object_t* NanoVM_op_pop(nvm_ctx_t* ctx, nvm_frame_t* frame, int do_unref) {
  if (frame->op_stack_len == 0) {
    NanoVM_error_log0("trying to pop an empty op-stack");
    return NULL;
  }
  nvm_object_t* obj = frame->op_stack[--frame->op_stack_len];
  if (do_unref) {
    NanoVM_unref(ctx, obj);
  }
  return obj;
}

int NanoVM_op_push(nvm_ctx_t* ctx, nvm_frame_t* frame, nvm_object_t* var) {
  if (frame->op_stack_len == frame->op_stack_cap) {
    NanoVM_error_log0("stack over flow: op stack");
    return -1;
  }
  frame->op_stack[frame->op_stack_len++] = var;
  NanoVM_ref(ctx, var);
  return 0;
}

nvm_object_t* NanoVM_get_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index) {
  if (index >= frame->local_var_cap || index < 0) {
    NanoVM_error_log1("trying to get local variable with out of bound index: %d", index);
    return NULL;
  }
  return frame->local_vars[index];
}

int NanoVM_set_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index, nvm_object_t* var) {
  if (index >= frame->local_var_cap || index < 0) {
    NanoVM_error_log1("trying to set local variable with out of bound index: %d", index);
    return -1;
  }
  // ref then unref, in case the input object is the same as the old object
  NanoVM_ref(ctx, var);
  nvm_object_t* old = frame->local_vars[index];
  if (old) {
    NanoVM_unref(ctx, old);
  }
  frame->local_vars[index] = var;
  return 0;
}

nvm_insn_handle_t NVM_STACK_get_insn_handle(nvm_insn_t* insn) {
  return insn_handles[insn->opcode];
}
