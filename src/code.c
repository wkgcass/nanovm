#include "code.internal.h"

int NanoVM_GLOBAL_init_code() {
  // TODO
  debug_log0("NanoVM_GLOBAL_init_code");
  return 0;
}

void NanoVM_GLOBAL_free_code() {
  // TODO
  debug_log0("NanoVM_GLOBAL_free_code");
}

nvm_code_mgr_t* NanoVM_create_code_mgr(nvm_ctx_t* ctx, int type_cap) {
  // TODO
  debug_log0("NanoVM_create_code_mgr");
  return NULL;
}

void NanoVM_release_code_mgr(nvm_ctx_t* ctx, nvm_code_mgr_t* code_mgr) {
  // TODO
  debug_log0("NanoVM_release_code_mgr");
}

int NanoVM_parse_code0(nvm_ctx_t* ctx, char** bytecode) {
  // TODO
  debug_log0("NanoVM_parse_code0");
  return 0;
}

nvm_meth_t* NanoVM_get_meth(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name, nvm_type_t* ret_type, int param_len, nvm_type_t** param_types) {
  // TODO
  debug_log0("NanoVM_get_meth");
  return NULL;
}

nvm_field_t* NanoVM_get_field(nvm_ctx_t* ctx, nvm_ref_type_t* ref_type, char* name) {
  // TODO
  debug_log0("NanoVM_get_field");
  return NULL;
}

nvm_prm_type_t* NanoVM_get_prm_type(nvm_ctx_t* ctx, char prm) {
  // TODO
  debug_log0("NanoVM_get_prm_type");
  return NULL;
}

nvm_ref_type_t* NanoVM_get_ref_type(nvm_ctx_t* ctx, char* name) {
  // TODO
  debug_log0("NanoVM_get_ref_type");
  return NULL;
}

nvm_arr_type_t* NanoVM_get_arr_type(nvm_ctx_t* ctx, nvm_type_t* comp_type) {
  // TODO
  debug_log0("NanoVM_get_arr_type");
  return NULL;
}

void NVM_CODE_release_type(nvm_ctx_t* ctx, nvm_type_t* type) {
  // TODO
  debug_log0("CODE_release_type");
}

void NVM_CODE_release_field(nvm_ctx_t* ctx, nvm_field_t* field) {
  // TODO
  debug_log0("CODE_release_field");
}

void NVM_CODE_release_meth(nvm_ctx_t* ctx, nvm_meth_t* meth) {
  // TODO
  debug_log0("CODE_release_meth");
}

void NVM_CODE_release_insn(nvm_ctx_t* ctx, nvm_insn_t* insn) {
  // TODO
  debug_log0("CODE_release_insn");
}

void NVM_CODE_release_ex(nvm_ctx_t* ctx, nvm_ex_t* ex) {
  // TODO
  debug_log0("CODE_release_ex");
}
