#include "memory.h"

mem_mgr_t* NanoVM_create_mem_mgr(ctx_t* ctx, long heap_cap, long mem_cap) {
  // TODO
  debug_log0("NanoVM_create_mem_mgr");
  return NULL;
}

void NanoVM_release_mem_mgr(ctx_t* ctx, mem_mgr_t* mem_mgr) {
  // TODO
  debug_log0("NanoVM_release_mem_mgr");
}

object_t* NanoVM_alloc_heap(ctx_t* ctx, type_t* type) {
  // TODO
  debug_log0("NanoVM_alloc_heap");
  return NULL;
}

arr_hdr_t* NanoVM_alloc_heap_arr(ctx_t* ctx, arr_type_t* type, int len) {
  // TODO
  debug_log0("NanoVM_alloc_heap_arr");
  return NULL;
}

void* NanoVM_alloc(ctx_t* ctx, size_t size) {
  // TODO
  debug_log0("NanoVM_alloc");
  return NULL;
}

void NanoVM_free(ctx_t* ctx, void*  ptr) {
  // TODO
  debug_log0("NanoVM_free");
}

void NanoVM_ref(ctx_t* ctx, object_t* ref) {
  // TODO
  debug_log0("NanoVM_ref");
}

void NanoVM_unref(ctx_t* ctx, object_t* ref) {
  // TODO
  debug_log0("NanoVM_unref");
}
