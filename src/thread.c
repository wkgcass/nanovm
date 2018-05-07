#include "thread.h"

thread_mgr_t* NanoVM_create_thread_mgr(ctx_t* ctx) {
  // TODO
  debug_log0("NanoVM_create_thread_mgr");
  return NULL;
}

void NanoVM_release_thread_mgr(ctx_t* ctx, thread_mgr_t* thread_mgr) {
  // TODO
  debug_log0("NanoVM_release_thread_mgr");
}

nthread_t* NanoVM_create_thread(ctx_t* ctx) {
  // TODO
  debug_log0("NanoVM_create_thread");
  return NULL;
}

int NanoVM_start_thread(ctx_t* ctx, nthread_t* thread) {
  // TODO
  debug_log0("NanoVM_start_thread");
  return 0;
}

int NanoVM_stop_thread(ctx_t* ctx, nthread_t* thread) {
  // TODO
  debug_log0("NanoVM_stop_thread");
  return 0;
}

void NanoVM_release_thread(ctx_t* ctx, nthread_t* thread) {
  // TODO
  debug_log0("NanoVM_release_thread");
}
