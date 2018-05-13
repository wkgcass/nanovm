#include <stdio.h>

#include "util.h"
#include "memory.h"
#include "code.internal.h"
#include "stack.internal.h"

#define TEST(condition, fail_msg) if (!(condition)) { printf("FAILED:[%s] %s\n", __func__, fail_msg); return -1; }

#define INIT_CTX \
  nanovm_t vm;\
  nvm_ctx_t _ctx;\
  nvm_ctx_t* ctx = &_ctx;\
  ctx->vm = &vm;\

#ifdef HAVE_MALLOC_SIZE
#define ZM_PREFIX_SIZE (0)
#else
#if defined(__sun) || defined(__sparc) || defined(__sparc__)
#define ZM_PREFIX_SIZE (sizeof(long long))
#else
#define ZM_PREFIX_SIZE (sizeof(size_t))
#endif
#endif

size_t _calc_real_mem_4(size_t size) {
  size_t _n = size;
  if (_n&3) _n += 4-(_n&3);
  return _n;
}

size_t _calc_real_mem_8(size_t size) {
  size_t _n = size;
  if (_n&7) _n += 8-(_n&7);
  return _n;
}

size_t _calc_real_mem_16(size_t size) {
  size_t _n = size;
  if (_n&15) _n += 16-(_n&15);
  return _n;
}

// the allocated memory might be padded to 16 bytes or 8 bytes,
// so we see both 16x and 8x memory as expected result
// also, 4 is supported in case on 32bit system and compiler sees the long as 4 bytes
// finally, when actual == expected, it's definitly correct
// as defined below:
int _mem_same(size_t actual, size_t expected) {
  return actual == expected || actual == _calc_real_mem_16(expected) || actual == _calc_real_mem_8(expected) || actual == _calc_real_mem_4(expected);
}

// build primitive types
void _build_prm_type(nvm_ctx_t* ctx,
                     nvm_prm_type_t** int_type_recv, nvm_prm_type_t** long_type_recv, nvm_prm_type_t** float_type_recv, nvm_prm_type_t** double_type_recv,
                     nvm_prm_type_t** char_type_recv, nvm_prm_type_t** short_type_recv, nvm_prm_type_t** byte_type_recv, nvm_prm_type_t** bool_type_recv) {
  // int
  nvm_prm_type_t* int_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  int_type->super.cat = NVM_TYPE_PRM;
  int_type->prm_type = NVM_PRM_TYPE_INT;
  int_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_INT);

  // long
  nvm_prm_type_t* long_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  long_type->super.cat = NVM_TYPE_PRM;
  long_type->prm_type = NVM_PRM_TYPE_LONG;
  long_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_LONG);

  // float
  nvm_prm_type_t* float_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  float_type->super.cat = NVM_TYPE_PRM;
  float_type->prm_type = NVM_PRM_TYPE_FLOAT;
  float_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_FLOAT);

  // double
  nvm_prm_type_t* double_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  double_type->super.cat = NVM_TYPE_PRM;
  double_type->prm_type = NVM_PRM_TYPE_DOUBLE;
  double_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_DOUBLE);

  // char
  nvm_prm_type_t* char_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  char_type->super.cat = NVM_TYPE_PRM;
  char_type->prm_type = NVM_PRM_TYPE_CHAR;
  char_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_CHAR);

  // short
  nvm_prm_type_t* short_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  short_type->super.cat = NVM_TYPE_PRM;
  short_type->prm_type = NVM_PRM_TYPE_SHORT;
  short_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_SHORT);

  // byte
  nvm_prm_type_t* byte_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  byte_type->super.cat = NVM_TYPE_PRM;
  byte_type->prm_type = NVM_PRM_TYPE_BYTE;
  byte_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_BYTE);

  // bool
  nvm_prm_type_t* bool_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  bool_type->super.cat = NVM_TYPE_PRM;
  bool_type->prm_type = NVM_PRM_TYPE_BOOLEAN;
  bool_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_BOOLEAN);

  if (int_type_recv) *int_type_recv = int_type;
  if (long_type_recv) *long_type_recv = long_type;
  if (float_type_recv) *float_type_recv = float_type;
  if (double_type_recv) *double_type_recv = double_type;
  if (short_type_recv) *short_type_recv = short_type;
  if (char_type_recv) *char_type_recv = char_type;
  if (byte_type_recv) *byte_type_recv = byte_type;
  if (bool_type_recv) *bool_type_recv = bool_type;
}

// build java.lang.Object
// and some.demo.Type extends java.lang.Object
// methods will not be built
void _build_demo_type(nvm_ctx_t* ctx,
                      nvm_ref_type_t** demo_type_recv, nvm_prm_type_t** long_type_recv,
                      nvm_field_t** f1_recv, nvm_field_t** f2_recv) {
  // create java.lang.Object
  nvm_ref_type_t* java_lang_Object = NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
  java_lang_Object->super.cat = NVM_TYPE_REF;
  java_lang_Object->field_len = 0;
  java_lang_Object->fields = NanoVM_alloc(ctx, 0);
  java_lang_Object->meth_len = 3;
  java_lang_Object->meths = NanoVM_alloc(ctx, sizeof(void*) * java_lang_Object->meth_len);
  java_lang_Object->name = "java/lang/Object";
  java_lang_Object->parent_type = NULL;
  java_lang_Object->size = 0;
  // methods are ignored, we won't use them when allocating

  // create some.demo.Type
  nvm_ref_type_t* some_demo_Type = NanoVM_alloc(ctx, sizeof(nvm_ref_type_t));
  some_demo_Type->super.cat = NVM_TYPE_REF;
  some_demo_Type->field_len = 2;
  some_demo_Type->fields = NanoVM_alloc(ctx, sizeof(void*) * some_demo_Type->field_len);
  some_demo_Type->meth_len = 0;
  some_demo_Type->meths = NanoVM_alloc(ctx, 0);
  some_demo_Type->name = "some/demo/Type";
  some_demo_Type->parent_type = java_lang_Object;
  some_demo_Type->size = sizeof(void*) * some_demo_Type->field_len + some_demo_Type->parent_type->size;

  // primitive long
  nvm_prm_type_t* long_type = NanoVM_alloc(ctx, sizeof(nvm_prm_type_t));
  long_type->super.cat = NVM_TYPE_PRM;
  long_type->prm_type = NVM_PRM_TYPE_LONG;
  long_type->size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_LONG);

  // fill field info into some.demo.Type
  // class Type { long f1; Object f2; }
  nvm_field_t* f1 = NanoVM_alloc(ctx, sizeof(nvm_field_t));
  f1->acc = 0;
  f1->dec_type = (nvm_type_t*) some_demo_Type;
  f1->name = "f1";
  f1->off = some_demo_Type->parent_type->size + 0;
  f1->static_v = NULL;
  f1->type = (nvm_type_t*) long_type;
  some_demo_Type->fields[0] = f1;
  nvm_field_t* f2 = NanoVM_alloc(ctx, sizeof(nvm_field_t));
  f2->acc = 0;
  f2->dec_type = (nvm_type_t*) some_demo_Type;
  f2->name = "f2";
  f2->off = some_demo_Type->parent_type->size + f1->off + sizeof(void*);
  f2->static_v = NULL;
  f2->type = (nvm_type_t*) java_lang_Object;
  some_demo_Type->fields[1] = f2;

  if (demo_type_recv) *demo_type_recv = some_demo_Type;
  if (long_type_recv) *long_type_recv = long_type;
  if (f1_recv) *f1_recv = f1;
  if (f2_recv) *f2_recv = f2;
}

// create a method:
// some.demo.Type#long demo_meth(long, Object)
void _build_meth(nvm_ctx_t* ctx, nvm_meth_t** meth_recv, nvm_ref_type_t** some_demo_Type_recv, nvm_prm_type_t** long_type_recv) {
  nvm_ref_type_t* some_demo_Type;
  nvm_prm_type_t* long_type;
  _build_demo_type(ctx, &some_demo_Type, &long_type, NULL, NULL);

  nvm_meth_t* meth = NanoVM_alloc(ctx, sizeof(nvm_meth_t));
  meth->acc = 0;
  meth->dec_type = (nvm_type_t*)some_demo_Type;
  meth->insn_len = 1;
  meth->insns = NanoVM_alloc(ctx, sizeof(void*));
  meth->insns[0] = NanoVM_alloc(ctx, sizeof(nvm_insn_t)); // not important, but need fill
  meth->max_locals = 12;
  meth->max_stack = 11;
  meth->name = "demo_meth";
  meth->param_len = 2;
  meth->param_types = NanoVM_alloc(ctx, sizeof(void*) * 2);
  meth->param_types[0] = (nvm_type_t*)long_type;
  meth->param_types[1] = (nvm_type_t*)some_demo_Type->parent_type; // java.lang.Object
  meth->ret_type = (nvm_type_t*)long_type;

  *some_demo_Type_recv = some_demo_Type;
  *long_type_recv = long_type;
  *meth_recv = meth;
}

#include "test.code.c"
#include "test.memory.c"
#include "test.nanovm.c"
#include "test.native.c"
#include "test.stack.c"
#include "test.thread.c"
#include "test.util.c"

int main(int argc, char** argv) {
  int err =
  TEST_code()
  | TEST_memory()
  | TEST_nanovm()
  | TEST_native()
  | TEST_stack()
  | TEST_thread()
  | TEST_util()
  ;
  if (err) {
    printf("SOME TESTS FAILED :(\n");
    return 1;
  } else {
    printf("ALL TESTS PASSED :)\n");
    return 0;
  }
}
