#include "memory.h"

mem_mgr_t* NanoVM_create_mem_mgr(nanovm_t* vm, long heap_cap, long mem_cap) {
  // TODO
  debug_log0("NanoVM_create_mem_mgr");
  return NULL;
}

void NanoVM_release_mem_mgr(mem_mgr_t* mem_mgr) {
  // TODO
  debug_log0("NanoVM_release_mem_mgr");
}

object_t* NanoVM_alloc_heap(nanovm_t* vm, type_t* type) {
  // TODO
  debug_log0("NanoVM_alloc_heap");
  return NULL;
}

arr_hdr_t* NanoVM_alloc_heap_arr(nanovm_t* vm, arr_type_t* type, int len) {
  // TODO
  debug_log0("NanoVM_alloc_heap_arr");
  return NULL;
}

void* NanoVM_alloc(nanovm_t* vm, size_t size) {
  // TODO
  debug_log0("NanoVM_alloc");
  return NULL;
}

void NanoVM_free(nanovm_t* vm, void*  ptr) {
  // TODO
  debug_log0("NanoVM_free");
}

void NanoVM_ref(nanovm_t* vm, object_t* ref) {
  // TODO
  debug_log0("NanoVM_ref");
}

void NanoVM_unref(nanovm_t* vm, object_t* ref) {
  // TODO
  debug_log0("NanoVM_unref");
}
