#include "memory.h"

nvm_mem_mgr_t* NanoVM_create_mem_mgr(nvm_ctx_t* ctx, long heap_cap, long mem_cap) {
  // TODO
  NanoVM_debug_log0("NanoVM_create_mem_mgr");
  return NULL;
}

void NanoVM_release_mem_mgr(nvm_ctx_t* ctx, nvm_mem_mgr_t* mem_mgr) {
  // TODO
  NanoVM_debug_log0("NanoVM_release_mem_mgr");
}

nvm_object_t* NanoVM_alloc_heap(nvm_ctx_t* ctx, nvm_type_t* type) {
  // TODO
  NanoVM_debug_log0("NanoVM_alloc_heap");
  return NULL;
}

nvm_arr_hdr_t* NanoVM_alloc_heap_arr(nvm_ctx_t* ctx, nvm_arr_type_t* type, int len) {
  // TODO
  NanoVM_debug_log0("NanoVM_alloc_heap_arr");
  return NULL;
}

void* NanoVM_alloc(nvm_ctx_t* ctx, size_t size) {
  // TODO
  NanoVM_debug_log0("NanoVM_alloc");
  return NULL;
}

void NanoVM_free(nvm_ctx_t* ctx, void*  ptr) {
  // TODO
  NanoVM_debug_log0("NanoVM_free");
}

void NanoVM_ref(nvm_ctx_t* ctx, nvm_object_t* ref) {
  // TODO
  NanoVM_debug_log0("NanoVM_ref");
}

void NanoVM_unref(nvm_ctx_t* ctx, nvm_object_t* ref) {
  // TODO
  NanoVM_debug_log0("NanoVM_unref");
}
