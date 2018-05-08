#include "native.h"

int NanoVM_GLOBAL_init_native() {
  // TODO
  NanoVM_debug_log0("NanoVM_GLOBAL_init_native");
  return 0;
}

void NanoVM_GLOBAL_free_native() {
  // TODO
  NanoVM_debug_log0("NanoVM_GLOBAL_free_native");
}

int NanoVM_create_native_mgr(nvm_ctx_t* ctx) {
  // TODO
  NanoVM_debug_log0("NanoVM_create_native_mgr");
  return 0;
}

void NanoVM_release_native_mgr(nvm_ctx_t* ctx, nvm_native_mgr_t* native_mgr) {
  // TODO
  NanoVM_debug_log0("NanoVM_release_native_mgr");
}

nvm_native_meth_t* NanoVM_get_native_meth(nvm_ctx_t* ctx, nvm_meth_t* meth) {
  // TODO
  NanoVM_debug_log0("NanoVM_get_native_meth");
  return NULL;
}
