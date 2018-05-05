#include "nanovm.internal.h"

int TEST_NanoVM_GLOBAL_init() {
  NanoVM_GLOBAL_init();
  return 0;
}

int TEST_NanoVM_GLOBAL_free() {
  NanoVM_GLOBAL_free();
  return 0;
}

int TEST_NanoVM_create() {
  NanoVM_create(0);
  return 0;
}

int TEST_NanoVM_parse_code() {
  NanoVM_parse_code(0, 0);
  return 0;
}

int TEST_NanoVM_start() {
  NanoVM_start(0, 0);
  return 0;
}

int TEST_NanoVM_stop() {
  NanoVM_stop(0);
  return 0;
}

int TEST_NanoVM_release() {
  NanoVM_release(0);
  return 0;
}

int TEST_nanovm() {
  int err =
      TEST_NanoVM_GLOBAL_init()
    & TEST_NanoVM_GLOBAL_free()
    & TEST_NanoVM_parse_code()
    & TEST_NanoVM_create()
    & TEST_NanoVM_start()
    & TEST_NanoVM_stop()
    & TEST_NanoVM_release()
  ;
  return err;
}
