#include "stack.internal.h"

// public

int TEST_NanoVM_GLOBAL_init_stack() {
  NanoVM_GLOBAL_init_stack();
  return 0;
}

int TEST_NanoVM_GLOBAL_free_stack() {
  NanoVM_GLOBAL_free_stack();
  return 0;
}

int TEST_NanoVM_create_stack_mgr() {
  NanoVM_create_stack_mgr(0, 0);
  return 0;
}

int TEST_NanoVM_release_stack_mgr() {
  NanoVM_release_stack_mgr(0);
  return 0;
}

int TEST_NanoVM_create_stack() {
  NanoVM_create_stack(0);
  return 0;
}

int TEST_NanoVM_release_stack() {
  NanoVM_release_stack(0);
  return 0;
}

int TEST_NanoVM_create_frame() {
  NanoVM_create_frame(0, 0, 0, 0);
  return 0;
}

int TEST_NanoVM_frame_pop() {
  NanoVM_frame_pop(0);
  return 0;
}

int TEST_NanoVM_frame_start() {
  NanoVM_frame_start(0, 0);
  return 0;
}

int TEST_NanoVM_op_pop() {
  NanoVM_op_pop(0);
  return 0;
}

int TEST_NanoVM_op_push() {
  NanoVM_op_push(0, 0);
  return 0;
}

int TEST_NanoVM_get_local() {
  NanoVM_get_local(0, 0);
  return 0;
}

int TEST_NanoVM_set_local() {
  NanoVM_set_local(0, 0, 0);
  return 0;
}

int TEST_NanoVM_rm_local() {
  NanoVM_rm_local(0, 0);
  return 0;
}

// internal

int TEST_STACK_get_insn_handle() {
  STACK_get_insn_handle(0);
  return 0;
}

int TEST_STACK_release_stack() {
  STACK_release_stack(0);
  return 0;
}

int TEST_STACK_release_frame() {
  STACK_release_frame(0);
  return 0;
}

int TEST_stack() {
  int err =
      TEST_NanoVM_GLOBAL_init_stack()
    & TEST_NanoVM_GLOBAL_free_stack()
    & TEST_NanoVM_create_stack_mgr()
    & TEST_NanoVM_release_stack_mgr()
    & TEST_NanoVM_create_stack()
    & TEST_NanoVM_release_stack()
    & TEST_NanoVM_create_frame()
    & TEST_NanoVM_frame_pop()
    & TEST_NanoVM_frame_start()
    & TEST_NanoVM_op_pop()
    & TEST_NanoVM_op_push()
    & TEST_NanoVM_get_local()
    & TEST_NanoVM_set_local()
    & TEST_NanoVM_rm_local()
    & TEST_STACK_get_insn_handle()
    & TEST_STACK_release_stack()
    & TEST_STACK_release_frame()
  ;
  return err;
}
