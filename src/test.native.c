#include "native.h"

int TEST_NanoVM_GLOBAL_init_native() {
  NanoVM_GLOBAL_init_native();
  return 0;
}

int TEST_NanoVM_GLOBAL_free_native() {
  NanoVM_GLOBAL_free_native();
  return 0;
}

int TEST_NanoVM_create_native_mgr() {
  NanoVM_create_native_mgr(0);
  return 0;
}

int TEST_NanoVM_release_native_mgr() {
  NanoVM_release_native_mgr(0, 0);
  return 0;
}

int TEST_NanoVM_get_native_meth() {
  NanoVM_get_native_meth(0, 0);
  return 0;
}

int TEST_native() {
  int err =
      TEST_NanoVM_GLOBAL_init_native()
    & TEST_NanoVM_GLOBAL_free_native()
    & TEST_NanoVM_create_native_mgr()
    & TEST_NanoVM_release_native_mgr()
    & TEST_NanoVM_get_native_meth()
  ;
  return err;
}
