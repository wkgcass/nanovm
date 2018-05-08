#include "stack.internal.h"

int NanoVM_GLOBAL_init_stack() {
  // TODO
  NanoVM_debug_log0("NanoVM_GLOBAL_init_stack");
  return 0;
}

void NanoVM_GLOBAL_free_stack() {
  // TODO
  NanoVM_debug_log0("NanoVM_GLOBAL_free_stack");
}

nvm_stack_mgr_t* NanoVM_create_stack_mgr(nvm_ctx_t* ctx, int frame_cap) {
  // TODO
  NanoVM_debug_log0("NanoVM_create_stack_mgr");
  return NULL;
}

void NanoVM_release_stack_mgr(nvm_ctx_t* ctx, nvm_stack_mgr_t* stack_mgr) {
  // TODO
  NanoVM_debug_log0("NanoVM_release_stack_mgr");
}

nvm_stack_t* NanoVM_create_stack(nvm_ctx_t* ctx) {
  // TODO
  NanoVM_debug_log0("NanoVM_create_stack");
  return NULL;
}

void NanoVM_release_stack(nvm_ctx_t* ctx, nvm_stack_t* stack) {
  // TODO
  NanoVM_debug_log0("NanoVM_release_stack");
}

nvm_frame_t* NanoVM_create_frame(nvm_ctx_t* ctx, nvm_stack_t* stack, nvm_meth_t* meth, int local_var_len, nvm_object_t** local_vars) {
  // TODO
  NanoVM_debug_log0("NanoVM_create_frame");
  return NULL;
}

nvm_frame_t* NanoVM_frame_pop(nvm_ctx_t* ctx, nvm_stack_t* stack) {
  // TODO
  NanoVM_debug_log0("NanoVM_frame_pop");
  return NULL;
}

int NanoVM_frame_start(nvm_ctx_t* ctx, nvm_frame_t* frame, nvm_result_t* result) {
  // TODO
  NanoVM_debug_log0("NanoVM_frame_start");
  return 0;
}

nvm_object_t* NanoVM_op_pop(nvm_ctx_t* ctx, nvm_frame_t* frame) {
  // TODO
  NanoVM_debug_log0("NanoVM_op_pop");
  return NULL;
}

int NanoVM_op_push(nvm_ctx_t* ctx, nvm_frame_t* frame, nvm_object_t* var) {
  // TODO
  NanoVM_debug_log0("NanoVM_op_push");
  return 0;
}

nvm_object_t* NanoVM_get_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index) {
  // TODO
  NanoVM_debug_log0("NanoVM_get_local");
  return NULL;
}

int NanoVM_set_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index, nvm_object_t* var) {
  // TODO
  NanoVM_debug_log0("NanoVM_set_local");
  return 0;
}

int NanoVM_rm_local(nvm_ctx_t* ctx, nvm_frame_t* frame, int index) {
  // TODO
  NanoVM_debug_log0("NanoVM_rm_local");
  return 0;
}

nvm_insn_handle_t NVM_STACK_get_insn_handle(nvm_ctx_t* ctx, nvm_insn_t* insn) {
  // TODO
  NanoVM_debug_log0("STACK_get_insn_handle");
  return NULL;
}

void NVM_STACK_release_stack(nvm_ctx_t* ctx, nvm_stack_t* stack) {
  // TODO
  NanoVM_debug_log0("STACK_release_stack");
}

void NVM_STACK_release_frame(nvm_ctx_t* ctx, nvm_frame_t* frame) {
  // TODO
  NanoVM_debug_log0("STACK_release_frame");
}
