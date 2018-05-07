#include <stdio.h>

#define TEST(condition, fail_msg) if (!(condition)) { printf("[%s] %s\n", __func__, fail_msg); return -1; }

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
    & TEST_memory()
    & TEST_nanovm()
    & TEST_native()
    & TEST_stack()
    & TEST_thread()
    & TEST_util()
  ;
  if (err) {
    printf("SOME TESTS FAILED :(\n");
    return 1;
  } else {
    printf("ALL TESTS PASSED :)\n");
    return 0;
  }
}
