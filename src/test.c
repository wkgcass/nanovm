#include <stdio.h>

#define TEST(condition, fail_msg) if (!(condition)) { printf("FAILED:[%s] %s\n", __func__, fail_msg); return -1; }

#define INIT_CTX \
  nanovm_t vm;\
  nvm_ctx_t _ctx;\
  nvm_ctx_t* ctx = &_ctx;\
  ctx->vm = &vm;\

#include "util.h"

#include "test.code.c"
#include "test.memory.c"
#include "test.nanovm.c"
#include "test.native.c"
#include "test.stack.c"
#include "test.thread.c"
#include "test.util.c"

int main(int argc, char** argv) {
  int err =
      TEST_code()
    | TEST_memory()
    | TEST_nanovm()
    | TEST_native()
    | TEST_stack()
    | TEST_thread()
    | TEST_util()
  ;
  if (err) {
    printf("SOME TESTS FAILED :(\n");
    return 1;
  } else {
    printf("ALL TESTS PASSED :)\n");
    return 0;
  }
}
