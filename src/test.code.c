#include "code.internal.h"
#include "nanovm.internal.h"
#include "memory.internal.h"

// public
#define INIT_CODE_MGR(type_cap) \
nvm_code_mgr_t* code_mgr = NanoVM_create_code_mgr(ctx, type_cap);\
TEST(code_mgr != NULL, "code_mgr should be created");

#define INIT_MEM_MGR(heap, mem)\
  nvm_mem_mgr_t* mem_mgr = NanoVM_create_mem_mgr(ctx, heap, mem);\
  TEST(mem_mgr != NULL, "mem_mgr should be created");

int TEST_NanoVM_GLOBAL_init_code() {
  int result = NanoVM_GLOBAL_init_code();
  TEST(result != -1, "init code failed");
  return 0;
}

int TEST_NanoVM_GLOBAL_free_code() {
  NanoVM_GLOBAL_free_code();
  return 0;
}

int TEST_NanoVM_create_code_mgr() {
  INIT_CTX
  INIT_MEM_MGR(1024, 1024)
  INIT_CODE_MGR(16)
  TEST(mem_mgr != NULL, "mem_mgr should be allocated");
  TEST(code_mgr != NULL, "code_mgr should be allocated");
  return 0;
}

int TEST_NanoVM_release_code_mgr() {
  INIT_CTX
  INIT_MEM_MGR(1024, 1024)
  INIT_CODE_MGR(16)
  TEST(mem_mgr != NULL, "mem_mgr should be allocated");
  TEST(code_mgr != NULL, "code_mgr should be allocated");
  NanoVM_release_code_mgr(ctx);
  return 0;
}

int TEST_NanoVM_parse_code0() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  return 0;
}

int TEST_NanoVM_get_meth() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  nvm_ref_type_t* ref_type = NanoVM_get_ref_type(ctx, "Test");
  nvm_type_t* ret_type = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_INT);
  TEST(ret_type != NULL, "ret_type should be found");
  nvm_type_t** param_types = NanoVM_alloc(ctx, sizeof(void*) * 2);
  param_types[0] = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_FLOAT);
  param_types[1] = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_INT);
  nvm_meth_t* meth = NanoVM_get_meth(ctx, ref_type, "getMethod", ret_type, 2, param_types);
  TEST(meth != NULL, "meth should be found");
  return 0;
}

int TEST_NanoVM_get_field() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  nvm_field_t* field = NanoVM_get_field(ctx, (nvm_ref_type_t*) code_mgr->types[10], "name");
  TEST(field != NULL, "field should be found");
  return 0;
}

int TEST_NanoVM_get_prm_type() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  nvm_prm_type_t* prm_type = NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_INT);
  TEST(prm_type, "prm_type should be found");

  return 0;
}

int TEST_NanoVM_get_ref_type() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  char* name = "Test";
  nvm_ref_type_t* ref_type = NanoVM_get_ref_type(ctx, name);
  TEST(ref_type != NULL, "ref_type should be found");
  return 0;
}

int TEST_NanoVM_get_arr_type() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  nvm_type_t* prm_type = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_BYTE);
  TEST(prm_type != NULL, "prm_type should be found");
  nvm_arr_type_t* arr_type = NanoVM_get_arr_type(ctx, prm_type);
  TEST(arr_type != NULL, "arr type should be found");
  return 0;
}

// internal

int TEST_NVM_CODE_release_type() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  char* name = "Test";
  nvm_ref_type_t* ref_type = NanoVM_get_ref_type(ctx, name);
  TEST(ref_type != NULL, "ref_type should be found");
  NVM_CODE_release_type(ctx, (nvm_type_t*) ref_type);
  ref_type = NanoVM_get_ref_type(ctx, name);
  TEST(ref_type == NULL, "ref_type should be null");
  return 0;
}

int TEST_NVM_CODE_release_field() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  nvm_field_t* field = NanoVM_get_field(ctx, (nvm_ref_type_t*) code_mgr->types[10], "name");
  TEST(field != NULL, "field should be found");
  NVM_CODE_release_field(ctx, field);
  field = NanoVM_get_field(ctx, (nvm_ref_type_t*) code_mgr->types[10], "name");
  TEST(field == NULL, "field should be null");
  return 0;
}

int TEST_NVM_CODE_release_meth() {
  INIT_CTX
  INIT_MEM_MGR(1024 * 1024, 1024 * 1024);
  INIT_CODE_MGR(16)
  char* file_name = "/Users/apple/CLionProjects/cfr/Test.class";
  FILE* file = fopen(file_name, "r");
  fseek(file, 0, SEEK_END);
  long len = ftell(file);
  unsigned char* data = zmalloc(len);
  fseek(file, 0, SEEK_SET);
  fread(data, 1, len, file);
  nvm_bytecode_t* bytecode = (nvm_bytecode_t*) zmalloc(sizeof(nvm_bytecode_t));
  bytecode[0].bytecode = data;
  bytecode[0].len = len;
  int bytecode_len = 1;
  int result = NanoVM_parse_code0(ctx, bytecode_len, bytecode);
  TEST(result == 0, "bytecode parse error");
  nvm_ref_type_t* ref_type = NanoVM_get_ref_type(ctx, "Test");
  nvm_type_t* ret_type = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_INT);
  TEST(ret_type != NULL, "ret_type should be found");
  nvm_type_t** param_types = NanoVM_alloc(ctx, sizeof(void*) * 2);
  param_types[0] = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_FLOAT);
  param_types[1] = (nvm_type_t*) NanoVM_get_prm_type(ctx, NVM_PRM_TYPE_INT);
  nvm_meth_t* meth = NanoVM_get_meth(ctx, ref_type, "getMethod", ret_type, 2, param_types);
  TEST(meth != NULL, "meth should be found");
  NVM_CODE_release_meth(ctx, meth);
  meth = NanoVM_get_meth(ctx, ref_type, "getMethod", ret_type, 2, param_types);
  TEST(meth == NULL, "meth should be NULL");
  return 0;
}

int TEST_NVM_CODE_release_insn() {
  //TODO
  return 0;
}

int TEST_NVM_CODE_release_ex() {
  //TODO
  return 0;
}

int TEST_code() {
  int err =
          TEST_NanoVM_GLOBAL_init_code()
          | TEST_NanoVM_GLOBAL_free_code()
          | TEST_NanoVM_create_code_mgr()
          | TEST_NanoVM_release_code_mgr()
          | TEST_NanoVM_parse_code0()
          | TEST_NanoVM_get_meth()
          | TEST_NanoVM_get_field()
          | TEST_NanoVM_get_prm_type()
          | TEST_NanoVM_get_ref_type()
          | TEST_NanoVM_get_arr_type()
          | TEST_NVM_CODE_release_type()
          | TEST_NVM_CODE_release_field()
          | TEST_NVM_CODE_release_meth()
          | TEST_NVM_CODE_release_insn()
          | TEST_NVM_CODE_release_ex();
  return err;
}
