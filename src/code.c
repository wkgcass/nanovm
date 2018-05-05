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

code_mgr_t* NanoVM_create_code_mgr(nanovm_t* vm, int type_cap) {
  // TODO
  debug_log0("NanoVM_create_code_mgr");
  return NULL;
}

void NanoVM_release_code_mgr(code_mgr_t* code_mgr) {
  // TODO
  debug_log0("NanoVM_release_code_mgr");
}

int NanoVM_parse_code0(nanovm_t* vm, char** bytecode) {
  // TODO
  debug_log0("NanoVM_parse_code0");
  return 0;
}

meth_t* NanoVM_get_meth(nanovm_t* vm, ref_type_t* ref_type, char* name, type_t* ret_type, int param_len, type_t** param_types) {
  // TODO
  debug_log0("NanoVM_get_meth");
  return NULL;
}

field_t* NanoVM_get_field(nanovm_t* vm, ref_type_t* ref_type, char* name) {
  // TODO
  debug_log0("NanoVM_get_field");
  return NULL;
}

prm_type_t* NanoVM_get_prm_type(nanovm_t* vm, char prm) {
  // TODO
  debug_log0("NanoVM_get_prm_type");
  return NULL;
}

ref_type_t* NanoVM_get_ref_type(nanovm_t* vm, char* name) {
  // TODO
  debug_log0("NanoVM_get_ref_type");
  return NULL;
}

arr_type_t* NanoVM_get_arr_type(nanovm_t* vm, type_t* comp_type) {
  // TODO
  debug_log0("NanoVM_get_arr_type");
  return NULL;
}

void CODE_release_type(type_t* type) {
  // TODO
  debug_log0("CODE_release_type");
}

void CODE_release_field(field_t* field) {
  // TODO
  debug_log0("CODE_release_field");
}

void CODE_release_meth(meth_t* meth) {
  // TODO
  debug_log0("CODE_release_meth");
}

void CODE_release_insn(insn_t* insn) {
  // TODO
  debug_log0("CODE_release_insn");
}

void CODE_release_ex(ex_t* ex) {
  // TODO
  debug_log0("CODE_release_ex");
}
