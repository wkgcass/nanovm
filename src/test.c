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
    return 1;
  } else {
    return 0;
  }
}
