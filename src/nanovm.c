#include "nanovm.internal.h"

int NanoVM_GLOBAL_init() {
  // TODO
  debug_log0("NanoVM_GLOBAL_init");
  return 0;
}

void NanoVM_GLOBAL_free() {
  // TODO
  debug_log0("NanoVM_GLOBAL_free");
}

nanovm_t* NanoVM_create(nanovm_config_t* conf) {
  // TODO
  debug_log0("NanoVM_create");
  return NULL;
}

int NanoVM_parse_code(nanovm_t* vm, int bytecode_len, nanovm_bytecode_t* bytecodes) {
  // TODO
  debug_log0("NanoVM_parse_code");
  return 0;
}

int NanoVM_start(nanovm_t* nanovm, char* main_class) {
  // TODO
  debug_log0("NanoVM_start");
  return 0;
}

int NanoVM_stop(nanovm_t* nanovm) {
  // TODO
  debug_log0("NanoVM_stop");
  return 0;
}

void NanoVM_release(nanovm_t* nanovm) {
  // TODO
  debug_log0("NanoVM_release");
}
