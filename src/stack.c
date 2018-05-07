#include "stack.internal.h"

int NanoVM_GLOBAL_init_stack() {
  // TODO
  debug_log0("NanoVM_GLOBAL_init_stack");
  return 0;
}

void NanoVM_GLOBAL_free_stack() {
  // TODO
  debug_log0("NanoVM_GLOBAL_free_stack");
}

stack_mgr_t* NanoVM_create_stack_mgr(ctx_t* ctx, int frame_cap) {
  // TODO
  debug_log0("NanoVM_create_stack_mgr");
  return NULL;
}

void NanoVM_release_stack_mgr(ctx_t* ctx, stack_mgr_t* stack_mgr) {
  // TODO
  debug_log0("NanoVM_release_stack_mgr");
}

stack_t* NanoVM_create_stack(ctx_t* ctx) {
  // TODO
  debug_log0("NanoVM_create_stack");
  return NULL;
}

void NanoVM_release_stack(ctx_t* ctx, stack_t* stack) {
  // TODO
  debug_log0("NanoVM_release_stack");
}

frame_t* NanoVM_create_frame(ctx_t* ctx, stack_t* stack, meth_t* meth, int local_var_len, object_t** local_vars) {
  // TODO
  debug_log0("NanoVM_create_frame");
  return NULL;
}

frame_t* NanoVM_frame_pop(ctx_t* ctx, stack_t* stack) {
  // TODO
  debug_log0("NanoVM_frame_pop");
  return NULL;
}

int NanoVM_frame_start(ctx_t* ctx, frame_t* frame, result_t* result) {
  // TODO
  debug_log0("NanoVM_frame_start");
  return 0;
}

object_t* NanoVM_op_pop(ctx_t* ctx, frame_t* frame) {
  // TODO
  debug_log0("NanoVM_op_pop");
  return NULL;
}

int NanoVM_op_push(ctx_t* ctx, frame_t* frame, object_t* var) {
  // TODO
  debug_log0("NanoVM_op_push");
  return 0;
}

object_t* NanoVM_get_local(ctx_t* ctx, frame_t* frame, int index) {
  // TODO
  debug_log0("NanoVM_get_local");
  return NULL;
}

int NanoVM_set_local(ctx_t* ctx, frame_t* frame, int index, object_t* var) {
  // TODO
  debug_log0("NanoVM_set_local");
  return 0;
}

int NanoVM_rm_local(ctx_t* ctx, frame_t* frame, int index) {
  // TODO
  debug_log0("NanoVM_rm_local");
  return 0;
}

insn_handle_t STACK_get_insn_handle(ctx_t* ctx, insn_t* insn) {
  // TODO
  debug_log0("STACK_get_insn_handle");
  return NULL;
}

void STACK_release_stack(ctx_t* ctx, stack_t* stack) {
  // TODO
  debug_log0("STACK_release_stack");
}

void STACK_release_frame(ctx_t* ctx, frame_t* frame) {
  // TODO
  debug_log0("STACK_release_frame");
}
