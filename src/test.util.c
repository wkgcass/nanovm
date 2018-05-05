#include "util.h"

int TEST_NanoVM_init_lock() {
  NanoVM_init_lock(0);
  return 0;
}

int TEST_NanoVM_try_lock() {
  NanoVM_try_lock(0);
  return 0;
}

int TEST_NanoVM_lock() {
  NanoVM_lock(0);
  return 0;
}

int TEST_NanoVM_unlock() {
  NanoVM_unlock(0);
  return 0;
}

int TEST_NanoVM_rand_int() {
  NanoVM_rand_int(0);
  return 0;
}

int TEST_util() {
  int err =
      TEST_NanoVM_init_lock()
    & TEST_NanoVM_try_lock()
    & TEST_NanoVM_lock()
    & TEST_NanoVM_unlock()
    & TEST_NanoVM_rand_int()
  ;
  return err;
}
