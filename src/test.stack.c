#include "nanovm.internal.h"
#include "stack.internal.h"
#include <string.h>
#include <errno.h>

#define INIT_STACK_MGR(frame_cap)\
  NanoVM_create_mem_mgr(ctx, 65536, 65536);\
  nvm_stack_mgr_t* stack_mgr = NanoVM_create_stack_mgr(ctx, frame_cap);\
  ctx->vm->stack_mgr = stack_mgr;\
  TEST(stack_mgr != NULL, "stack_mgr should be created");

void _build_frame(nvm_ctx_t* ctx, nvm_stack_t* stack, nvm_frame_t** frame_recv) {
  nvm_prm_type_t* long_type;
  nvm_ref_type_t* some_demo_Type;
  nvm_meth_t* meth; // (long,Object)
  _build_meth(ctx, &meth, &some_demo_Type, &long_type);
  nvm_object_t** objs = NanoVM_alloc(ctx, sizeof(void*) * 3);

  nvm_object_t* arg1 = NanoVM_alloc_heap(ctx, (nvm_type_t*)some_demo_Type);
  nvm_object_t* arg2 = NanoVM_alloc_heap(ctx, (nvm_type_t*)long_type);
  nvm_object_t* arg3 = NanoVM_alloc_heap(ctx, (nvm_type_t*)some_demo_Type->parent_type);

  objs[0] = arg1;
  objs[1] = arg2;
  objs[2] = arg3;

  nvm_frame_t* frame = NanoVM_create_frame(ctx, stack, meth, 3, objs);
  *frame_recv = frame;
}

void _build_fake_frame(nvm_ctx_t* ctx, nvm_stack_t* stack, nvm_frame_t** frame_recv) {
  nvm_frame_t* frame = NanoVM_alloc(ctx, sizeof(nvm_frame_t));
  memset(frame, 0, sizeof(nvm_frame_t));
  frame->idx = stack->frame_len;
  stack->frames[stack->frame_len++] = frame;
  frame->local_vars = NanoVM_alloc(ctx, sizeof(void*) * 12);
  memset(frame->local_vars, 0, sizeof(void*) * 12);
  frame->local_var_cap = 12;
  frame->op_stack = NanoVM_alloc(ctx, sizeof(void*) * 11);
  memset(frame->op_stack, 11, sizeof(void*) * 11);
  frame->op_stack_cap = 11;
  frame->stack = stack;

  *frame_recv = frame;
}

// public

int TEST_NanoVM_GLOBAL_init_stack() {
  size_t used_a = zmalloc_used_memory();
  NanoVM_GLOBAL_init_stack();
  size_t used_b = zmalloc_used_memory();
  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + sizeof(void*) * NVM_INSN_HANDLE_SIZE),
       "should allocate some memory to store function pointers");
  NanoVM_GLOBAL_free_stack();
  size_t used_c = zmalloc_used_memory();
  TEST(used_c == used_a, "the memory should be released");

  // init the stack, for the other cases to use
  NanoVM_GLOBAL_init_stack();
  return 0;
}

int TEST_NanoVM_create_stack_mgr() {
  INIT_CTX
  INIT_STACK_MGR(123)

  TEST(stack_mgr->frame_cap == 123, "stack manager should store frame_cap same as the input");
  return 0;
}

int TEST_NanoVM_release_stack_mgr() {
  INIT_CTX
  NanoVM_create_mem_mgr(ctx, 65536, 65536);
  size_t used_a = NanoVM_get_mem_usage(ctx->vm);
  NanoVM_create_stack_mgr(ctx, 1024);
  NanoVM_release_stack_mgr(ctx);
  size_t used_b = NanoVM_get_mem_usage(ctx->vm);
  TEST(used_b == used_a, "stack manager should be fully released");
  return 0;
}

int TEST_NanoVM_create_stack() {
  INIT_CTX
  INIT_STACK_MGR(2)

  size_t used_a = NanoVM_get_mem_usage(ctx->vm);
  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  size_t used_b = NanoVM_get_mem_usage(ctx->vm);
  TEST(stack->frame_cap == 2, "frame cap should be the same as it in stack_mgr");
  TEST(stack->frame_len == 0, "frame len should be 0 when initiated");
  TEST(stack->frames != NULL, "frames should be allocated");
  TEST(_mem_same(zmalloc_size(stack->frames), ZM_PREFIX_SIZE + sizeof(void*) * 2),
       "should allocate frames");
  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + sizeof(nvm_stack_t) + ZM_PREFIX_SIZE + sizeof(void*) * 2),
       "should allocate stack and frames");
  return 0;
}

int TEST_NanoVM_release_stack() {
  INIT_CTX
  INIT_STACK_MGR(20)

  size_t used_a = NanoVM_get_mem_usage(ctx->vm);
  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  nvm_frame_t* fake_frame = NanoVM_alloc(ctx, sizeof(nvm_frame_t));
  fake_frame->local_var_cap = 0;
  fake_frame->local_vars = NULL;
  fake_frame->op_stack_len = 0;
  fake_frame->op_stack = NULL;
  fake_frame->result.ex = NULL;
  fake_frame->result.ret = NULL;
  stack->frame_len = 1;
  stack->frames[0] = fake_frame;
  NanoVM_release_stack(ctx, stack);
  size_t used_b = NanoVM_get_mem_usage(ctx->vm);
  TEST(used_b == used_a, "the stack and it's inside frames should all be released");
  return 0;
}

int TEST_NanoVM_create_frame() {
  INIT_CTX
  INIT_STACK_MGR(20)

  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  nvm_frame_t* frame;
  _build_frame(ctx, stack, &frame);
  TEST(frame->cur == frame->meth->insns[0], "for non native methods, set current instruction to first insn");
  TEST(frame->local_var_cap == frame->meth->max_locals, "frame local var cap should be equal to meth max locals");
  // it's non static method, so should be invoked with an instance
  TEST(frame->local_vars[0]->type == frame->meth->dec_type, "frame->local_vars[0]->type should equal to frame->meth->dec_type");
  TEST(frame->local_vars[1]->type == frame->meth->param_types[0], "frame->local_vars[1]->type should equal to meth->param_type[0]");
  // long takes two slots
  TEST(frame->local_vars[3]->type == frame->meth->param_types[1], "frame->local_vars[3]->type should equal to meth->param_type[1]");
  TEST(frame->op_stack_len == 0, "frame op stack length should be 0");
  TEST(frame->op_stack_cap == frame->meth->max_stack, "frame op stack cap should be equal to meth max stack");
  TEST(frame->result.ret == NULL, "result.ret should be null");
  TEST(frame->result.ex == NULL, "result.ex should be null");
  TEST(frame->stack == stack, "stack should equal to the input stack");
  TEST(frame->idx == 0, "the first frame in the stack should get idx 0");

  nvm_frame_t* frame2;
  _build_frame(ctx, stack, &frame2);
  TEST(frame2->idx == 1, "the second frame in the stack should get idx 1");

  return 0;
}

int TEST_NanoVM_frame_pop() {
  INIT_CTX
  INIT_STACK_MGR(20)

  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  nvm_frame_t* frame;
  nvm_frame_t* frame2;
  size_t usage_a = NanoVM_get_mem_usage(ctx->vm);
  _build_fake_frame(ctx, stack, &frame);
  _build_fake_frame(ctx, stack, &frame2);
  TEST(stack->frame_len == 2, "frame len should be 2 after building 2 frames");
  NanoVM_frame_pop(ctx, stack);
  TEST(stack->frame_len == 1, "frame len should decrease to 1 after popped");
  NanoVM_frame_pop(ctx, stack);
  TEST(stack->frame_len == 0, "frame len should decrease to 0 after popped");
  size_t usage_b = NanoVM_get_mem_usage(ctx->vm);
  TEST(usage_a == usage_b, "frame mem should be released after it's popped");

  return 0;
}

int TEST_stack_over_flow() {
  INIT_CTX
  INIT_STACK_MGR(2)

  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  nvm_meth_t* meth;
  _build_meth(ctx, &meth, NULL, NULL);
  nvm_object_t** local = NanoVM_alloc(ctx, sizeof(void*) * 10);
  memset(local, 0, sizeof(void*) * 10);
  nvm_frame_t* frame = NanoVM_create_frame(ctx, stack, meth, 10, local);
  TEST(frame != NULL, "frame should be created 1");
  frame = NanoVM_create_frame(ctx, stack, meth, 10, local);
  TEST(frame != NULL, "frame should be created 2");
  frame = NanoVM_create_frame(ctx, stack, meth, 10, local);
  TEST(frame == NULL, "create frame should fail, and stack over flow");
  TEST(errno == NVM_ERR_STK_OVR, "errno should be set to NVM_ERR_STK_OVR");

  return 0;
}

int TEST_NanoVM_frame_start() {
  printf("TODO NanoVM_frame_start\n");
  return 0;
}

int TEST_NanoVM_op_push_pop() {
  INIT_CTX
  INIT_STACK_MGR(20)

  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  nvm_frame_t* frame;
  _build_fake_frame(ctx, stack, &frame);
  nvm_object_t* int_val = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);
  NanoVM_op_push(ctx, frame, int_val);
  TEST(int_val->cnt == 1, "the value pushed into op stack should have ref count 1");
  TEST(frame->op_stack_len == 1, "after pushing a value, the op stack len should increase");
  NanoVM_op_push(ctx, frame, int_val);
  TEST(int_val->cnt == 2, "the value pushed into op stack again should have ref count 2");
  TEST(frame->op_stack_len == 2, "after pushing a value, the op stack len should increase again");
  NanoVM_op_pop(ctx, frame, 1);
  TEST(int_val->cnt == 1, "the value popped from op stack should have ref count 1");
  TEST(frame->op_stack_len == 1, "after poping a value, the op stack len should decrease");
  NanoVM_op_pop(ctx, frame, 0);
  TEST(int_val->cnt == 1, "the value popped from op stack but not unrefed should have ref count 1");
  TEST(frame->op_stack_len == 0, "after poping a value, the op stack len should decrease again");

  return 0;
}

int TEST_NanoVM_get_set_local() {
  INIT_CTX
  INIT_STACK_MGR(20)

  nvm_stack_t* stack = NanoVM_create_stack(ctx);
  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  nvm_frame_t* frame;
  _build_fake_frame(ctx, stack, &frame);
  nvm_object_t* int_val = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);
  NanoVM_set_local(ctx, frame, 2, int_val);
  TEST(int_val->cnt == 1, "ref cnt should be 1 after set local");
  NanoVM_set_local(ctx, frame, 2, int_val);
  TEST(int_val->cnt == 1, "ref cnt should not change if set the same object to local");
  NanoVM_ref(ctx, int_val);
  TEST(int_val->cnt == 2, "manually ref the object");
  nvm_object_t* int_val2 = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);
  NanoVM_set_local(ctx, frame, 2, int_val2);
  TEST(int_val->cnt == 1, "ref cnt should decrease when the old local object replaced");
  TEST(int_val2->cnt == 1, "ref cnt should be 1 after set local");

  return 0;
}

// internal

int TEST_NVM_STACK_get_insn_handle() {
  printf("TODO NanoVM_get_insn_handle\n");
  return 0;
}

int TEST_stack() {
  int err =
      TEST_NanoVM_GLOBAL_init_stack()
    | TEST_NanoVM_create_stack_mgr()
    | TEST_NanoVM_release_stack_mgr()
    | TEST_NanoVM_create_stack()
    | TEST_NanoVM_release_stack()
    | TEST_NanoVM_create_frame()
    | TEST_NanoVM_frame_pop()
    | TEST_NanoVM_frame_start()
    | TEST_NanoVM_op_push_pop()
    | TEST_NanoVM_get_set_local()
    | TEST_NVM_STACK_get_insn_handle()
  ;
  return err;
}
