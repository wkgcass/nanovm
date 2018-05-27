#include "thread.h"

#define INIT_THREAD_MGR(thread_cap)\
  NanoVM_create_mem_mgr(ctx, 1024, 1024);\
  NanoVM_create_stack_mgr(ctx, 10);\
  NanoVM_create_thread_mgr(ctx, thread_cap);\

int TEST_NanoVM_create_thread_mgr() {
  INIT_CTX
  NanoVM_create_mem_mgr(ctx, 1024, 1024);
  NanoVM_create_stack_mgr(ctx, 10);

  size_t used_a = NanoVM_get_mem_usage(ctx->vm);
  NanoVM_create_thread_mgr(ctx, 10);
  size_t used_b = NanoVM_get_mem_usage(ctx->vm);

  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + sizeof(nvm_thread_mgr_t) + ZM_PREFIX_SIZE + sizeof(void*) * 10),
       "create thread mgr should allocate corresponding memory");

  ctx->vm->thread_mgr->threads[0] = (void*) 123; // not 0
  TEST(NanoVM_release_thread_mgr(ctx) == -1, "should not release when thread still running");
  ctx->vm->thread_mgr->threads[0] = NULL;
  TEST(NanoVM_release_thread_mgr(ctx) == 0, "should release when all threads stopped");

  size_t used_c = NanoVM_get_mem_usage(ctx->vm);

  TEST(used_c == used_a, "memory should be released when release thread manager");
  return 0;
}

int TEST_NanoVM_create_thread() {
  INIT_CTX
  INIT_THREAD_MGR(20)

  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);

  size_t used_a = NanoVM_get_mem_usage(ctx->vm);
  nvm_thread_t* thread = NanoVM_create_thread(ctx, obj); // we set some dummy object, it won't be used here, only record the pointer
  size_t used_b = NanoVM_get_mem_usage(ctx->vm);

  // pthread not tested, won't be initiated here
  TEST(thread->idx == 0, "the first thread should point to idx 0");
  TEST(thread->lock.cnt == 0, "the lock should be initiated");
  TEST(thread->runnable == obj, "the runnable object should equal to the thread");
  TEST(thread->stack, "the stack should be created");
  TEST(thread->state == NVM_THREAD_STATE_INIT, "the state should be INIT");
  TEST(thread->tid < NVM_MAX_TID, "tid should be less than max_tid");
  TEST(obj->cnt == 1, "the thread should ref the runnable object");

  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + sizeof(nvm_thread_t) + ZM_PREFIX_SIZE + sizeof(nvm_stack_t) + ZM_PREFIX_SIZE + sizeof(void*) * 10),
       "should allocate the thread and stack");

  NanoVM_ref(ctx, obj); // prevent the object being released

  TEST(NanoVM_release_thread(ctx, thread) == 0, "release thread should success");
  size_t used_c = NanoVM_get_mem_usage(ctx->vm);

  TEST(used_c == used_a, "memory of the thread should be fully released");

  thread = NanoVM_create_thread(ctx, obj);
  thread = NanoVM_create_thread(ctx, obj);
  TEST(thread->idx == 1, "the second thread should point to idx 1");
  return 0;
}

int TEST_NanoVM_create_thread_fail() {
  INIT_CTX
  INIT_THREAD_MGR(1)

  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);

  nvm_thread_t* thread = NanoVM_create_thread(ctx, obj);
  TEST(thread, "first thread should be created");
  thread = NanoVM_create_thread(ctx, obj);
  TEST(!thread, "second thread should not be created");
  return 0;
}

int TEST_NanoVM_start_thread() {
  // TODO test start thread
  return 0;
}

int TEST_NanoVM_intr_thread() {
  INIT_CTX
  INIT_THREAD_MGR(1)

  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);

  nvm_thread_t* thread = NanoVM_create_thread(ctx, obj);
  TEST(NanoVM_intr_thread(ctx, thread) == -1, "thread should not be interrupted when it's in init state");
  TEST(thread->state == NVM_THREAD_STATE_DEAD, "state should be changed to dead");

  thread->state = NVM_THREAD_STATE_RUN;
  TEST(NanoVM_intr_thread(ctx, thread) == 0, "thread should be set to intr state if it's running");
  TEST(thread->state == NVM_THREAD_STATE_INTR, "state should be set to intr");

  return 0;
}

int TEST_NanoVM_release_thread_fail() {
  INIT_CTX
  INIT_THREAD_MGR(1)

  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);

  nvm_thread_t* thread = NanoVM_create_thread(ctx, obj);

  thread->state = NVM_THREAD_STATE_RUN;
  TEST(NanoVM_release_thread(ctx, thread) == -1, "releasing the thread should fail when it's running");
  return 0;
}

int TEST_thread() {
  int err =
      TEST_NanoVM_create_thread_mgr()
    | TEST_NanoVM_create_thread()
    | TEST_NanoVM_create_thread_fail()
    | TEST_NanoVM_start_thread()
    | TEST_NanoVM_intr_thread()
    | TEST_NanoVM_release_thread_fail()
  ;
  return err;
}
