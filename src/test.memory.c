#include "memory.h"

int TEST_NanoVM_create_mem_mgr() {
  NanoVM_create_mem_mgr(0, 0L, 0L);
  return 0;
}

int TEST_NanoVM_release_mem_mgr() {
  NanoVM_release_mem_mgr(0);
  return 0;
}

int TEST_NanoVM_alloc_heap() {
  NanoVM_alloc_heap(0, 0);
  return 0;
}

int TEST_NanoVM_alloc_heap_arr() {
  NanoVM_alloc_heap_arr(0, 0, 0);
  return 0;
}

int TEST_NanoVM_alloc() {
  NanoVM_alloc(0, 0);
  return 0;
}

int TEST_NanoVM_free() {
  NanoVM_free(0, 0);
  return 0;
}

int TEST_NanoVM_ref() {
  NanoVM_ref(0, 0);
  return 0;
}

int TEST_NanoVM_unref() {
  NanoVM_unref(0, 0);
  return 0;
}

int TEST_memory() {
  int err =
      TEST_NanoVM_create_mem_mgr()
    & TEST_NanoVM_release_mem_mgr()
    & TEST_NanoVM_alloc_heap()
    & TEST_NanoVM_alloc_heap_arr()
    & TEST_NanoVM_alloc()
    & TEST_NanoVM_free()
    & TEST_NanoVM_ref()
    & TEST_NanoVM_unref()
  ;
  return err;
}
