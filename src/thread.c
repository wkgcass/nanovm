#include "thread.h"

#include "nanovm.internal.h"
#include "util.h"
#include "memory.h"
#include <string.h>

nvm_thread_mgr_t* NanoVM_create_thread_mgr(nvm_ctx_t* ctx, int thread_cap) {
  nvm_thread_mgr_t* mgr = NanoVM_alloc(ctx, sizeof(nvm_thread_mgr_t));
  if (!mgr) {
    NanoVM_debug_log0("allocate thread manager failed");
    return NULL;
  }
  mgr->thread_cap = thread_cap;
  if (NanoVM_init_lock(&mgr->lock) < 0) {
    NanoVM_error_log0("init lock for thread manager failed");
    NanoVM_free(ctx, mgr);
    return NULL;
  }
  mgr->threads = NanoVM_alloc(ctx, sizeof(nvm_thread_t*) * thread_cap);
  if (!mgr->threads) {
    NanoVM_debug_log0("allocate thread array failed");
    NanoVM_free(ctx, mgr);
    return NULL;
  }
  memset(mgr->threads, 0, sizeof(nvm_thread_t*) * thread_cap);
  ctx->vm->thread_mgr = mgr;
  return mgr;
}

int NanoVM_release_thread_mgr(nvm_ctx_t* ctx) {
  nvm_thread_mgr_t* mgr = ctx->vm->thread_mgr;
  for (int i = 0; i < mgr->thread_cap; ++i) {
    if (mgr->threads[i]) {
      NanoVM_error_log0("still got thread(s) running, should not release");
      return -1;
    }
  }
  NanoVM_free(ctx, mgr->threads);
  NanoVM_free(ctx, mgr);
  return 0;
}

nvm_thread_t* NanoVM_create_thread(nvm_ctx_t* ctx, nvm_object_t* runnable) {
  nvm_thread_mgr_t* mgr = ctx->vm->thread_mgr;
  NanoVM_lock(&mgr->lock, ctx->tid);

  int index = -1;
  for (int i = 0; i < mgr->thread_cap; ++i) {
    if (!mgr->threads[i]) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    NanoVM_debug_log0("too many threads");
    return NULL;
  }
  nvm_thread_t* thread = NanoVM_alloc(ctx, sizeof(nvm_thread_t));
  if (!thread) {
    NanoVM_unlock(&mgr->lock, ctx->tid);
    NanoVM_debug_log0("allocate thread failed");
    return NULL;
  }
  mgr->threads[index] = thread;
  NanoVM_unlock(&mgr->lock, ctx->tid);
  memset(thread, 0, sizeof(nvm_thread_t));
  thread->idx = index;
  if (NanoVM_init_lock(&thread->lock) < 0) {
    NanoVM_debug_log0("init lock for thread failed");
    goto finalize;
  }
  thread->stack = NanoVM_create_stack(ctx);
  if (!thread->stack) {
    NanoVM_debug_log0("create stack for thread failed");
    goto finalize;
  }
  thread->stack->thread = thread;
  // thread->pthread is not initiated for now
  thread->state = NVM_THREAD_STATE_INIT;
  thread->tid = NanoVM_rand_int(NVM_MAX_TID);
  thread->runnable = runnable;
  NanoVM_ref(ctx, runnable);
  return thread;

finalize:
  mgr->threads[index] = NULL;
  NanoVM_free(ctx, thread);
  return NULL;
}

int NanoVM_start_thread(nvm_ctx_t* ctx, nvm_thread_t* thread) {
  // TODO depends on `code` module
  return 0;
}

int NanoVM_intr_thread(nvm_ctx_t* ctx, nvm_thread_t* thread) {
  if (thread->state == NVM_THREAD_STATE_DEAD) {
    return -1; // already dead
  }
  if (thread->state == NVM_THREAD_STATE_INTR) {
    return 0; // already interrupted
  }
  NanoVM_lock(&thread->lock, ctx->tid);
  if (thread->state == NVM_THREAD_STATE_INIT) {
    thread->state = NVM_THREAD_STATE_DEAD;
    NanoVM_unlock(&thread->lock, ctx->tid);
    return -1;
  }
  thread->state = NVM_THREAD_STATE_INTR;
  NanoVM_unlock(&thread->lock, ctx->tid);
  return 0;
}

int NanoVM_release_thread(nvm_ctx_t* ctx, nvm_thread_t* thread) {
  if (thread->state != NVM_THREAD_STATE_DEAD && thread->state != NVM_THREAD_STATE_INIT) {
    NanoVM_error_log0("thread still running");
    return -1;
  }
  nvm_thread_mgr_t* mgr = ctx->vm->thread_mgr;
  mgr->threads[thread->idx] = NULL;
  NanoVM_release_stack(ctx, thread->stack);
  NanoVM_unref(ctx, thread->runnable);
  NanoVM_free(ctx, thread);
  return 0;
}
