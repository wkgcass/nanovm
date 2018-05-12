#include "memory.internal.h"

#include "zmalloc.h"

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

#define INIT_MEM_MGR(heap, mem)\
  nvm_mem_mgr_t* mem_mgr = NanoVM_create_mem_mgr(ctx, heap, mem);\
  TEST(mem_mgr != NULL, "mem_mgr should be created");

int TEST_NanoVM_create_mem_mgr() {
  INIT_CTX

  size_t used_a = zmalloc_used_memory();

  INIT_MEM_MGR(1024, 1024)

  size_t used_b = zmalloc_used_memory();
  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + sizeof(nvm_mem_mgr_t)), "the malloced size when inits is wrong");
  TEST(mem_mgr != NULL, "mem_mgr should be allocated");
  TEST(NanoVM_get_mem_usage(ctx->vm) == 0, "mem usage should be 0 when mem_mgr inits");
  TEST(NanoVM_get_heap_usage(ctx->vm) == 0, "heap usage should be 0 when mem_mgr inits");
  TEST(ctx->vm->mem_mgr != NULL, "mem_mgr should be filled into mem_mgr");
  return 0;
}

int TEST_NanoVM_release_mem_mgr() {
  INIT_CTX

  size_t used_a = zmalloc_used_memory();

  INIT_MEM_MGR(1024, 1024)

  NanoVM_release_mem_mgr(ctx);
  size_t used_b = zmalloc_used_memory();
  TEST(used_a == used_b, "memory should be the same before and after allocation");
  return 0;
}

int TEST_NanoVM_alloc_heap_primitive() {
  INIT_CTX

  INIT_MEM_MGR(1024, 1024)

  // primitive
  nvm_prm_type_t type;
  type.super.cat = NVM_TYPE_PRM;
  type.size = NanoVM_get_prm_type_size(NVM_PRM_TYPE_SHORT);
  type.prm_type = NVM_PRM_TYPE_SHORT;
  size_t used_a = zmalloc_used_memory();
  size_t usage_a = NanoVM_get_heap_usage(ctx->vm);
  nvm_prm_hdr_t* obj = (nvm_prm_hdr_t*)NanoVM_alloc_heap(ctx, (nvm_type_t*) (&type));
  size_t used_b = zmalloc_used_memory();
  size_t usage_b = NanoVM_get_heap_usage(ctx->vm);
  size_t expected_size = sizeof(nvm_prm_hdr_t) + type.size;
  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + expected_size), "allocated wrong size of space");
  TEST(usage_b - usage_a == used_b - used_a, "allocated wrong size of heap space");
  TEST(obj->super.type == (nvm_type_t*)&type, "allocated object type should be the given type");
  TEST(obj->super.cnt == 0, "object ref cnt should be 0 when first allocated");
  TEST(zmalloc_size(obj) == usage_b - usage_a, "object size is wrong");
  short res = 1;
  NanoVM_mem_get_prm(obj, &res);
  TEST(obj->super.cnt == 0, "ref cnt should be 0 when it's firstly created");
  TEST(((nvm_prm_type_t*)obj->super.type) == &type, "object type should be equal to the input type");
  TEST(res == 0, "initial value should be 0");
  return 0;
}

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

int TEST_NanoVM_alloc_heap_reference() {
  INIT_CTX
  
  INIT_MEM_MGR(1024, 1024)

  nvm_ref_type_t* some_demo_Type;
  nvm_prm_type_t* long_type;
  nvm_field_t* f1;
  nvm_field_t* f2;
  _build_demo_type(ctx, &some_demo_Type, &long_type, &f1, &f2);

  // alloc the type first to let _long_0 and _null initiated
  NanoVM_alloc_heap(ctx, (nvm_type_t*) some_demo_Type);

  // test alloc heap
  size_t used_a = zmalloc_used_memory();
  size_t usage_a = NanoVM_get_heap_usage(ctx->vm);
  nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*) some_demo_Type);
  TEST(obj != NULL, "allocation should not fail");
  size_t used_b = zmalloc_used_memory();
  size_t usage_b = NanoVM_get_heap_usage(ctx->vm);
  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + sizeof(nvm_ref_hdr_t) + 2 * sizeof(void*)), "allocated wrong size of memory");
  TEST(usage_b - usage_a == used_b - used_a, "allocated wrong size of heap memory");
  TEST(used_b - used_a == zmalloc_size(obj), "object size is wrong");
  TEST(obj->cnt == 0, "object ref cnt should be 0 when initialized");
  TEST(obj->type == ((nvm_type_t*) some_demo_Type), "object type should be equal to the input type");

  // test fields
  char* p0 = (char*) obj;
  void** pf1 = (void**) (p0 + sizeof(nvm_ref_hdr_t));
  void** pf2 = (void**) (p0 + sizeof(nvm_ref_hdr_t) + sizeof(void*));

  nvm_prm_hdr_t* p_prm_f1 = (nvm_prm_hdr_t*) *pf1;
  TEST(p_prm_f1->super.type == (nvm_type_t*)long_type, "field 1 should be type long");
  nvm_ref_hdr_t* p_ref_f2 = (nvm_ref_hdr_t*) *pf2;
  TEST(p_ref_f2->super.type->cat == NVM_TYPE_NULL, "field 2 should be null");
 
  // test set field
  nvm_prm_hdr_t* long_val;
  NanoVM_mem_get_field((nvm_ref_hdr_t*) obj, f1, (nvm_object_t**) &long_val);
  long l = 1L;
  NanoVM_mem_get_prm(long_val, &l);
  TEST(0 == l, "primitive should be initialized with 0");
  long_val = (nvm_prm_hdr_t*) NanoVM_alloc_heap(ctx, (nvm_type_t*) long_type);
  NanoVM_mem_set_prm_j(long_val, 3L);
  NanoVM_mem_get_prm(long_val, &l);
  TEST(3 == l, "the long primtive object should be set to given value");
  NanoVM_mem_put_field(ctx, (nvm_ref_hdr_t*) obj, f1, (nvm_object_t*)long_val);
  nvm_prm_hdr_t* retrieved_ptr;
  NanoVM_mem_get_field((nvm_ref_hdr_t*) obj, f1, (nvm_object_t**) &retrieved_ptr);
  TEST(retrieved_ptr == long_val, "the set pointer should be the same as retrieved");

  return 0;
}

int TEST_NanoVM_alloc_heap_arr() {
  INIT_CTX
  
  INIT_MEM_MGR(1024, 1024)
  
  nvm_ref_type_t* ref_type;
  nvm_prm_type_t* prm_type;
  _build_demo_type(ctx, &ref_type, &prm_type, NULL, NULL);
  
  nvm_arr_type_t* arr_ref_type = NanoVM_alloc(ctx, sizeof(nvm_arr_type_t));
  arr_ref_type->comp_type = (nvm_type_t*)ref_type;
  arr_ref_type->super.cat = NVM_TYPE_ARR;
  nvm_arr_type_t* arr_prm_type = NanoVM_alloc(ctx, sizeof(nvm_arr_type_t));
  arr_prm_type->comp_type = (nvm_type_t*)prm_type;
  arr_ref_type->super.cat = NVM_TYPE_ARR;
  
  nvm_arr_hdr_t* ref_arr = (nvm_arr_hdr_t*) NanoVM_alloc_heap_arr(ctx, arr_ref_type, 10);
  TEST(ref_arr->len == 10, "the ref array len should equal to the input");
  nvm_object_t* null_val = NanoVM_get_null(ctx);
  nvm_object_t* sixth_element; // take 6th elem in case the pointer calculation fails
  NanoVM_mem_get_array(ref_arr, 5, &sixth_element);
  TEST(sixth_element == null_val, "ref array init value should be null");
  for (int i = 0; i < 10; ++i) {
    nvm_object_t* e;
    NanoVM_mem_get_array(ref_arr, i, &e);
    TEST(e == sixth_element, "all null values should be the same");
  }
  
  nvm_arr_hdr_t* prm_arr = (nvm_arr_hdr_t*) NanoVM_alloc_heap_arr(ctx, arr_prm_type, 7);
  TEST(prm_arr->len == 7, "the prm array len should equal to the input");
  sixth_element = NULL;
  NanoVM_mem_get_array(prm_arr, 5, &sixth_element);
  TEST(prm_type == (nvm_prm_type_t*)sixth_element->type, "prm array should initiate values with the input component type");
  for (int i = 0; i < 7; ++i) {
    nvm_object_t* e;
    NanoVM_mem_get_array(prm_arr, i, &e);
    TEST(e == sixth_element, "all primitive initial 0 values should be the same");
    long l = 1;
    NanoVM_mem_get_prm((nvm_prm_hdr_t*)e, &l);
    TEST(l == 0, "primitives should be initiated with 0");
  }

  return 0;
}

int TEST_NanoVM_alloc() {
  INIT_CTX

  INIT_MEM_MGR(1024, 1024)

  size_t used_a = zmalloc_used_memory();
  size_t usage_a = NanoVM_get_mem_usage(ctx->vm);
  void* mem = NanoVM_alloc(ctx, sizeof(char) * 127);
  TEST(mem != NULL, "mem should be allocated");
  size_t used_b = zmalloc_used_memory();
  size_t usage_b = NanoVM_get_mem_usage(ctx->vm);
  size_t expected_size = sizeof(char) * 127;
  TEST(_mem_same(used_b - used_a, ZM_PREFIX_SIZE + expected_size), "allocated wrong size of space");
  TEST(zmalloc_size(mem) == used_b - used_a, "result mem size is wrong");
  TEST(usage_b - usage_a == used_b - used_a, "allocated wrong size of mem space");
  return 0;
}

int TEST_NanoVM_free() {
  INIT_CTX

  INIT_MEM_MGR(1024, 1024)

  void* mem = NanoVM_alloc(ctx, sizeof(char) * 127);
  TEST(mem != NULL, "mem should be allocated");

  size_t used_a = zmalloc_used_memory();
  size_t usage_a = NanoVM_get_mem_usage(ctx->vm);
  NanoVM_free(ctx, mem);
  size_t used_b = zmalloc_used_memory();
  size_t usage_b = NanoVM_get_mem_usage(ctx->vm);
  size_t expected_size = sizeof(char) * 127;
  TEST(_mem_same(used_a - used_b, ZM_PREFIX_SIZE + expected_size), "released wrong size of space");
  TEST(usage_a - usage_b == used_a - used_b, "released wrong size of mem space");
  return 0;
}

int TEST_NanoVM_ref() {
  INIT_CTX
  
  INIT_MEM_MGR(1024, 1024)

  nvm_prm_type_t* types[8];
  _build_prm_type(ctx, &types[0], &types[1], &types[2], &types[3], &types[4], &types[5], &types[6], &types[7]);

  for (int i = 0; i < 8; ++i) {
    nvm_arr_type_t* arr_type = NanoVM_alloc(ctx, sizeof(nvm_arr_type_t));
    arr_type->super.cat = NVM_TYPE_ARR;
    arr_type->comp_type = (nvm_type_t*)types[i];
    nvm_arr_hdr_t* arr = (nvm_arr_hdr_t*) NanoVM_alloc_heap_arr(ctx, arr_type, 1);
    nvm_object_t* o;
    NanoVM_mem_get_array(arr, 0, &o);
    TEST(o->cnt == 0, "primitive initial object ref cnt should be 0");
    NanoVM_ref(ctx, o);
    TEST(o->cnt == 0, "primitive initial object ref cnt should not change");
  }

  nvm_object_t* null_val = NanoVM_get_null(ctx);
  TEST(null_val->cnt == 0, "null ref cnt should be 0");
  NanoVM_ref(ctx, null_val);
  TEST(null_val->cnt == 0, "null ref cnt should not change");

  // prm/ref/arr type objects NanoVM_ref invocations are handled in the same way
  // so we only test one primitive type object
  nvm_prm_type_t* int_type = types[0];
  nvm_object_t* o = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);
  TEST(null_val->cnt == 0, "ref cnt should be 0");
  for (int i = 0; i < 10; ++i) {
    NanoVM_ref(ctx, o);
    TEST(o->cnt == (i + 1), "ref cnt should add up");
  }

  return 0;
}

int TEST_NanoVM_unref() {
  INIT_CTX

  INIT_MEM_MGR(1024, 1024)

  nvm_ref_type_t* some_demo_Type;
  nvm_prm_type_t* long_type;
  nvm_field_t* f1;
  nvm_field_t* f2;
  _build_demo_type(ctx, &some_demo_Type, &long_type, &f1, &f2);

  nvm_prm_type_t* types[8];
  _build_prm_type(ctx, &types[0], NULL, &types[2], &types[3], &types[4], &types[5], &types[6], &types[7]);
  types[1] = long_type;

  nvm_arr_type_t* long_arr_type = NanoVM_alloc(ctx, sizeof(nvm_arr_type_t));
  long_arr_type->comp_type = (nvm_type_t*)long_type;
  long_arr_type->super.cat = NVM_TYPE_ARR;

  // allocate a long array to let manager generate _long_0
  nvm_object_t* tmp = (nvm_object_t*)NanoVM_alloc_heap_arr(ctx, long_arr_type, 1);
  NanoVM_ref(ctx, tmp);
  NanoVM_unref(ctx, tmp); // release it
  // get null to let manager generate _null
  NanoVM_get_null(ctx);

  // test release primitives

  for (int i = 0; i < 8; ++i) {
    nvm_prm_type_t* prm_type = types[i];
    nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*)prm_type);
    NanoVM_ref(ctx, obj);

    size_t used_a = zmalloc_used_memory();
    size_t usage_a = NanoVM_get_heap_usage(ctx->vm);
    NanoVM_unref(ctx, obj);
    size_t used_b = zmalloc_used_memory();
    size_t usage_b = NanoVM_get_heap_usage(ctx->vm);
    TEST(_mem_same(used_a - used_b, ZM_PREFIX_SIZE + sizeof(nvm_prm_hdr_t) + prm_type->size), "object should be released when ref cnt is 0");
    TEST(usage_a - usage_b == used_a - used_b, "heap usage mismatch .");
  }

  // test references

  nvm_object_t* obj = NanoVM_alloc_heap(ctx, (nvm_type_t*)some_demo_Type);
  NanoVM_ref(ctx, obj);
  nvm_object_t* long_val = NanoVM_alloc_heap(ctx, (nvm_type_t*)long_type);
  NanoVM_mem_set_prm_j((nvm_prm_hdr_t*)long_val, 2L);
  NanoVM_mem_put_field(ctx, (nvm_ref_hdr_t*)obj, f1, long_val);
  TEST(long_val->cnt == 1, "ref count should increase after put field");
  long_val = NanoVM_alloc_heap(ctx, (nvm_type_t*)long_type);
  NanoVM_mem_set_prm_j((nvm_prm_hdr_t*)long_val, 1L);
  size_t used_a = zmalloc_used_memory();
  size_t usage_a = NanoVM_get_heap_usage(ctx->vm);
  NanoVM_mem_put_field(ctx, (nvm_ref_hdr_t*)obj, f1, long_val);
  size_t used_b = zmalloc_used_memory();
  size_t usage_b = NanoVM_get_heap_usage(ctx->vm);
  TEST(_mem_same(used_a - used_b, ZM_PREFIX_SIZE + sizeof(nvm_prm_hdr_t) + long_type->size),
       "old value should be unref after the field is set to another value");
  TEST(usage_a - usage_b == used_a - used_b, "heap usage mismatch ..");
  TEST(long_val->cnt == 1, "ref count should increase after put field ..");

  used_a = zmalloc_used_memory();
  usage_a = NanoVM_get_heap_usage(ctx->vm);
  NanoVM_unref(ctx, obj);
  used_b = zmalloc_used_memory();
  usage_b = NanoVM_get_heap_usage(ctx->vm);
  TEST(_mem_same(used_a - used_b, ZM_PREFIX_SIZE + sizeof(nvm_ref_hdr_t) + some_demo_Type->size + ZM_PREFIX_SIZE + sizeof(nvm_prm_hdr_t) + long_type->size),
       "object and it's dependers should be unrefed");
  TEST(usage_a - usage_b == used_a - used_b, "heap usage mismatch ...");

  // test array types

  nvm_arr_hdr_t* arr = NanoVM_alloc_heap_arr(ctx, (nvm_arr_type_t*)long_arr_type, 10);
  NanoVM_ref(ctx, (nvm_object_t*)arr);
  long_val = NanoVM_alloc_heap(ctx, (nvm_type_t*)long_type);
  NanoVM_mem_set_array(ctx, arr, 2, long_val);
  TEST(long_val->cnt == 1, "ref count should increase after set into array");
  long_val = NanoVM_alloc_heap(ctx, (nvm_type_t*)long_type);
  used_a = zmalloc_used_memory();
  usage_a = NanoVM_get_heap_usage(ctx->vm);
  NanoVM_mem_set_array(ctx, arr, 2, long_val);
  used_b = zmalloc_used_memory();
  usage_b = NanoVM_get_heap_usage(ctx->vm);
  TEST(_mem_same(used_a - used_b, ZM_PREFIX_SIZE + sizeof(nvm_prm_hdr_t) + long_type->size),
       "old value should be unref after the array element is set to another value");
  TEST(usage_a - usage_b == used_a - used_b, "heap usage mismatch ....");
  TEST(long_val->cnt == 1, "ref count should increase after set array element .");

  used_a = zmalloc_used_memory();
  usage_a = NanoVM_get_heap_usage(ctx->vm);
  NanoVM_unref(ctx, (nvm_object_t*)arr);
  used_b = zmalloc_used_memory();
  usage_b = NanoVM_get_heap_usage(ctx->vm);
  TEST(_mem_same(used_a - used_b, ZM_PREFIX_SIZE + sizeof(nvm_arr_hdr_t) + sizeof(void*) * 10 + ZM_PREFIX_SIZE + sizeof(nvm_prm_hdr_t) + long_type->size),
       "array and it's containing elements should be released");
  TEST(usage_a - usage_b == used_a - used_b, "heap usage mismatch .....");

  return 0;
}

int TEST_oom() {
  INIT_CTX

  INIT_MEM_MGR(1, 1024)

  void* mem = NanoVM_alloc(ctx, 2048);
  TEST(mem == NULL, "mem should oom");

  nvm_prm_type_t* int_type;
  _build_prm_type(ctx, &int_type, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  mem = NanoVM_alloc_heap(ctx, (nvm_type_t*)int_type);
  TEST(mem == NULL, "heap should oom");

  return 0;
}

int TEST_NanoVM_mem_cap() {
  INIT_CTX

  INIT_MEM_MGR(1234, 5678)

  TEST(NanoVM_get_heap_cap(ctx->vm) == 1234, "heap cap should equal to input");
  TEST(NanoVM_get_mem_cap(ctx->vm) == 5678, "mem cap should euqal to input");

  return 0;
}

int TEST_memory() {
  int err =
      TEST_NanoVM_create_mem_mgr()
    | TEST_NanoVM_release_mem_mgr()
    | TEST_NanoVM_alloc_heap_primitive()
    | TEST_NanoVM_alloc_heap_reference()
    | TEST_NanoVM_alloc_heap_arr()
    | TEST_NanoVM_alloc()
    | TEST_NanoVM_free()
    | TEST_NanoVM_ref()
    | TEST_NanoVM_unref()
    | TEST_oom()
    | TEST_NanoVM_mem_cap()
  ;
  return err;
}
