#include "code.internal.h"

// public

int TEST_NanoVM_GLOBAL_init_code() {
  NanoVM_GLOBAL_init_code();
  return 0;
}

int TEST_NanoVM_GLOBAL_free_code() {
  NanoVM_GLOBAL_free_code();
  return 0;
}

int TEST_NanoVM_create_code_mgr() {
  NanoVM_create_code_mgr(0, 0);
  return 0;
}

int TEST_NanoVM_release_code_mgr() {
  NanoVM_release_code_mgr(0);
  return 0;
}

int TEST_NanoVM_parse_code0() {
  NanoVM_parse_code0(0, 0);
  return 0;
}

int TEST_NanoVM_get_meth() {
  NanoVM_get_meth(0, 0, 0, 0, 0, 0);
  return 0;
}

int TEST_NanoVM_get_field() {
  NanoVM_get_field(0, 0, 0);
  return 0;
}

int TEST_NanoVM_get_prm_type() {
  NanoVM_get_prm_type(0, 0);
  return 0;
}

int TEST_NanoVM_get_ref_type() {
  NanoVM_get_ref_type(0, 0);
  return 0;
}

int TEST_NanoVM_get_arr_type() {
  NanoVM_get_arr_type(0, 0);
  return 0;
}

// internal

int TEST_CODE_release_type() {
  CODE_release_type(0);
  return 0;
}

int TEST_CODE_release_field() {
  CODE_release_field(0);
  return 0;
}

int TEST_CODE_release_meth() {
  CODE_release_meth(0);
  return 0;
}

int TEST_CODE_release_insn() {
  CODE_release_insn(0);
  return 0;
}

int TEST_CODE_release_ex() {
  CODE_release_ex(0);
  return 0;
}

int TEST_code() {
  int err =
      TEST_NanoVM_GLOBAL_init_code()
    & TEST_NanoVM_GLOBAL_free_code()
    & TEST_NanoVM_create_code_mgr()
    & TEST_NanoVM_release_code_mgr()
    & TEST_NanoVM_parse_code0()
    & TEST_NanoVM_get_meth()
    & TEST_NanoVM_get_field()
    & TEST_NanoVM_get_prm_type()
    & TEST_NanoVM_get_ref_type()
    & TEST_NanoVM_get_arr_type()
    & TEST_CODE_release_type()
    & TEST_CODE_release_field()
    & TEST_CODE_release_meth()
    & TEST_CODE_release_insn()
    & TEST_CODE_release_ex()
  ;
  return err;
}
