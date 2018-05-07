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

code_mgr_t* NanoVM_create_code_mgr(ctx_t* ctx, int type_cap) {
  // TODO
  debug_log0("NanoVM_create_code_mgr");
  return NULL;
}

void NanoVM_release_code_mgr(ctx_t* ctx, code_mgr_t* code_mgr) {
  // TODO
  debug_log0("NanoVM_release_code_mgr");
}

int NanoVM_parse_code0(ctx_t* ctx, char** bytecode) {
  // TODO
  debug_log0("NanoVM_parse_code0");
  return 0;
}

meth_t* NanoVM_get_meth(ctx_t* ctx, ref_type_t* ref_type, char* name, type_t* ret_type, int param_len, type_t** param_types) {
  // TODO
  debug_log0("NanoVM_get_meth");
  return NULL;
}

field_t* NanoVM_get_field(ctx_t* ctx, ref_type_t* ref_type, char* name) {
  // TODO
  debug_log0("NanoVM_get_field");
  return NULL;
}

prm_type_t* NanoVM_get_prm_type(ctx_t* ctx, char prm) {
  // TODO
  debug_log0("NanoVM_get_prm_type");
  return NULL;
}

ref_type_t* NanoVM_get_ref_type(ctx_t* ctx, char* name) {
  // TODO
  debug_log0("NanoVM_get_ref_type");
  return NULL;
}

arr_type_t* NanoVM_get_arr_type(ctx_t* ctx, type_t* comp_type) {
  // TODO
  debug_log0("NanoVM_get_arr_type");
  return NULL;
}

void CODE_release_type(ctx_t* ctx, type_t* type) {
  // TODO
  debug_log0("CODE_release_type");
}

void CODE_release_field(ctx_t* ctx, field_t* field) {
  // TODO
  debug_log0("CODE_release_field");
}

void CODE_release_meth(ctx_t* ctx, meth_t* meth) {
  // TODO
  debug_log0("CODE_release_meth");
}

void CODE_release_insn(ctx_t* ctx, insn_t* insn) {
  // TODO
  debug_log0("CODE_release_insn");
}

void CODE_release_ex(ctx_t* ctx, ex_t* ex) {
  // TODO
  debug_log0("CODE_release_ex");
}
