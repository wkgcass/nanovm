#include "thread.h"

int TEST_NanoVM_create_thread_mgr() {
  NanoVM_create_thread_mgr(0);
  return 0;
}

int TEST_NanoVM_release_thread_mgr() {
  NanoVM_release_thread_mgr(0, 0);
  return 0;
}

int TEST_NanoVM_create_thread() {
  NanoVM_create_thread(0);
  return 0;
}

int TEST_NanoVM_start_thread() {
  NanoVM_start_thread(0, 0);
  return 0;
}

int TEST_NanoVM_stop_thread() {
  NanoVM_stop_thread(0, 0);
  return 0;
}

int TEST_NanoVM_release_thread() {
  NanoVM_release_thread(0, 0);
  return 0;
}

int TEST_thread() {
  int err =
      TEST_NanoVM_create_thread_mgr()
    & TEST_NanoVM_release_thread_mgr()
    & TEST_NanoVM_create_thread()
    & TEST_NanoVM_start_thread()
    & TEST_NanoVM_stop_thread()
    & TEST_NanoVM_release_thread()
  ;
  return err;
}
